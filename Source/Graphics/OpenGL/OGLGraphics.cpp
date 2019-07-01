//
// Created by liuhao on 2019-04-15.
//

#include "../../Graphics/Graphics.h"
#include "../../Graphics/GraphicsEvents.h"
#include "../../Graphics/GraphicsImpl.hpp"
#include "../../Graphics/ConstantBuffer.h"
#include "../../Graphics/RenderSurface.h"
#include "../../Resource/ResourceCache.h"
#include "../TextureCube.hpp"

#include <SDL.h>

namespace Urho3D
{
	// note , corresponds CompareMode enum
	static const unsigned glCmpFunc[] = {
			GL_ALWAYS,
			GL_EQUAL,
			GL_NOTEQUAL,
			GL_LESS,
			GL_LEQUAL,
			GL_GREATER,
			GL_GEQUAL,
	};

	// todo, why duplicated?
	static const unsigned glSrcBlend[] =
	{
		GL_ONE,
		GL_ONE,
		GL_DST_COLOR,
		GL_SRC_ALPHA,
		GL_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_DST_ALPHA,
		GL_ONE,
		GL_SRC_ALPHA
	};

	static const unsigned glDestBlend[] =
	{
		GL_ZERO,
		GL_ONE,
		GL_ZERO,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE,
		GL_ONE
	};

	// todo
	static const unsigned glBlendOp[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT
	};

#ifndef GL_ES_VERSION_2_0
	static const unsigned glFillMode[] =
	{
			GL_FILL,
			GL_LINE,
			GL_POINT
	};

	// todo, 为什么在 GraphicsDef.h 中定义了 StencilOp, 还要再这里定义类似的这个？
	// ref: SetStencilTest function
	static const unsigned glStencilOps[] =
	{
			GL_KEEP,
			GL_ZERO,
			GL_REPLACE,
			GL_INCR_WRAP,
			GL_DECR_WRAP
	};
#endif

	//todo, why 4 float?
	// ref VertexElementType in GraphicsDefs.h
	static const unsigned glElementTypes[] =
	{
		GL_INT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_BYTE
	};

	// ref VertexElementType in GraphicsDefs.h
	static const unsigned glElementComponents[] = {
			1,
			1,
			2,
			3,
			4,
			4,
			4
	};


	static String extensions;

	bool CheckExtension(const String& name)
	{
		if(extensions.Empty())
			extensions = (const char*)glGetString(GL_EXTENSIONS);
		return extensions.Contains(name);
	}

	static void GetGLPrimitiveType(unsigned elementCount, PrimitiveType type, unsigned& primitiveCount,
									GLenum& glPrimitiveType)
	{
		switch (type)
		{
			case TRIANGLE_LIST:
				primitiveCount = elementCount / 3;
				glPrimitiveType = GL_TRIANGLES;
				break;

			case LINE_LIST:
				primitiveCount = elementCount / 2;
				glPrimitiveType = GL_LINES;
				break;

			case POINT_LIST:
				primitiveCount = elementCount;
				glPrimitiveType = GL_POINTS;
				break;

			case TRIANGLE_STRIP:
				primitiveCount = elementCount - 2;
				glPrimitiveType = GL_TRIANGLE_STRIP;
				break;

			case LINE_STRIP:
				primitiveCount = elementCount - 1;
				glPrimitiveType = GL_LINE_STIPPLE;
				break;

			case TRIANGLE_FAN:
				primitiveCount = elementCount - 2;
				glPrimitiveType = GL_TRIANGLE_FAN;
				break;
		}
	}

	const Vector2 Graphics::pixelUVOffset(0.0f, 0.0f);
	bool Graphics::gl3Support = false;

	Graphics::Graphics(Context *context) :
			Object(context),
			impl_(new GraphicsImpl()),
			position_(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED),
			shadowMapFormat_(GL_DEPTH_COMPONENT16),
			hiresShadowMapFormat_(GL_DEPTH_COMPONENT24),
			shaderPath_("Shaders/GLSL/"),
			shaderExtension_(".glsl"),
			orientations_("LandscapeLeft LandscapeRight"),
#ifndef GL_ES_VERSION_2_0
			apiName_("GL2")
#else
			apiName_("GLES2")
#endif
	{
		SetTextureUnitMappings();
		ResetCachedState();

		context_->RequireSDL(SDL_INIT_VIDEO);

		RegisterGraphicsLibrary(context_);
	}

	void Graphics::SetTextureUnitMappings()
	{
		textureUnits_["DiffMap"] = TU_DIFFUSE;
		textureUnits_["DiffCubeMap"] = TU_DIFFUSE;
		textureUnits_["AlbedoBuffer"] = TU_ALBEDOBUFFER;
		textureUnits_["NormalMap"] = TU_NORMAL;
		textureUnits_["NormalBuffer"] = TU_NORMALBUFFER;
		textureUnits_["SpecMap"] = TU_SPECULAR;
		textureUnits_["EmissiveMap"] = TU_EMISSIVE;
		textureUnits_["EnvMap"] = TU_ENVIRONMENT;
		textureUnits_["EnvCubeMap"] = TU_ENVIRONMENT;
		textureUnits_["LightRampMap"] = TU_LIGHTRAMP;
		textureUnits_["LightSpotMap"] = TU_LIGHTSHAPE;
		textureUnits_["LightCubeMap"] = TU_LIGHTSHAPE;
		textureUnits_["ShadowMap"] = TU_SHADOWMAP;
#ifndef GL_ES_VERSION_2_0
		textureUnits_["VolumeMap"] = TU_VOLUMEMAP;
		//todo, what is this?
		textureUnits_["FaceSelectCubeMap"] = TU_FACESELECT;
		textureUnits_["IndirectionCubeMap"] = TU_INDIRECTION;
		textureUnits_["DepthBuffer"] = TU_DEPTHBUFFER;
		//todo, what is this?
		textureUnits_["LightBuffer"] = TU_LIGHTBUFFER;
		textureUnits_["ZoneCubeMap"] = TU_ZONE;
		textureUnits_["ZoneVolumeMap"] = TU_ZONE;

#endif

	}

	void Graphics::ResetCachedState()
	{
		for(auto& vertexBuffer : vertexBuffers_)
			vertexBuffer = nullptr;

		for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
		{
			textures_[i] = nullptr;
			impl_->textureTypes_[i] = 0;
		}

		for(auto& renderTarget : renderTargets_)
			renderTarget = nullptr;

		depthStencil_ = nullptr;
		viewport_ = IntRect(0, 0, 0, 0);
		indexBuffer_ = nullptr;
		vertexShader_ = nullptr;
		pixelShader_ = nullptr;
		blendMode_ = BLEND_REPLACE;
		alphaToCoverage_ = false;
		colorWrite_ = true;
		cullMode_ = CULL_NONE;
		constantDepthBias_ = 0.0f;
		slopeScaleDepthBias_ = 0.0f;
		depthTestMode_ = CMP_ALWAYS;
		depthWrite_ = false;
		lineAntiAlias_ = false;
		fillMode_ = FILL_SOLID;
		scissorTest_ = false;
		scissorRect_ = IntRect::ZERO;
		stencilTest_ = false;
		stencilTestMode_ = CMP_ALWAYS;
		stencilPass_ = OP_KEEP;
		stencilFail_ = OP_KEEP;
		stencilZFail_ = OP_KEEP;
		stencilRef_ = 0;
		stencilCompareMask_ = M_MAX_UNSIGNED;
		stencilWriteMask_ = M_MAX_UNSIGNED;
		useClipPlane_ = false;

		impl_->shaderProgram_ = nullptr;
		impl_->lastInstanceOffset_ = 0;
		impl_->activeTexture_ = 0;
		impl_->enabledVertexAttributes_ = 0;
		impl_->usedVertexAttributes_ = 0;
		impl_->instancingVertexAttributes_ = 0;
		impl_->boundFBO_ = impl_->systemFBO_;
		impl_->boundVBO_ = 0;
		impl_->boundUBO_ = 0;
		impl_->sRGBWrite_ = false;

		if(impl_->context_)
		{
			glEnable(GL_DEPTH_TEST);
			SetCullMode(CULL_CCW);
			SetDepthTest(CMP_LESSEQUAL);
			SetDepthWrite(true);
		}

		for(auto& constantBuffer : impl_->constantBuffers_)
			constantBuffer = nullptr;

		impl_->dirtyConstantBuffers_.Clear();
	}

	void Graphics::SetCullMode(CullMode mode)
	{
		if(mode != cullMode_)
		{
			if(mode == CULL_NONE)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				glCullFace(mode == CULL_CCW ? GL_FRONT : GL_BACK);
			}
			cullMode_ = mode;
		}
	}

	void Graphics::SetDepthTest(CompareMode mode)
	{
		if(mode != depthTestMode_)
		{
			glDepthFunc(glCmpFunc[mode]);
			depthTestMode_ = mode;
		}
	}

	void Graphics::SetDepthWrite(bool enable)
	{
		if(enable != depthWrite_)
		{
			glDepthMask(enable ? GL_TRUE : GL_FALSE);
			depthWrite_ = enable;
		}
	}

	void Graphics::SetFillMode(FillMode mode)
	{
#ifndef GL_ES_VERSION_2_0
		if(mode != fillMode_)
		{
			glPolygonMode(GL_FRONT_AND_BACK, glFillMode[mode]);
			fillMode_ = mode;
		}
#endif
	}


	Graphics::~Graphics()
	{
		Close();

		delete impl_;

		context_->ReleaseSDL();
	}

	void Graphics::Close()
	{
		if(!IsInitialized())
			return;
		Release(true, true);
	}

	void Graphics::Release(bool clearGPUObject, bool closeWindow)
	{
		if(!window_)
			return;

		MutexLock lock(gpuObjectMutex_);

		if(clearGPUObject)
		{
			impl_->shaderPrograms_.Clear();

			for(PODVector<GPUObject*>::Iterator i = gpuObjects_.Begin(); i != gpuObjects_.End(); ++i)
			{
				(*i)->Release();
			}
			gpuObjects_.Clear();
		}
		else
		{
			for(PODVector<GPUObject*>::Iterator i = gpuObjects_.Begin(); i != gpuObjects_.End(); ++i)
			{
				(*i)->OnDeviceLost();
			}

			impl_->shaderPrograms_.Clear();
			SendEvent(E_DEVICELOST);
		}

		CleanupFrameBuffers();
		impl_->depthTextures_.Clear();

		if(impl_->context_)
		{
			if(!clearGPUObject)
				URHO3D_LOGINFO("OpenGL context lost");

			SDL_GL_DeleteContext(impl_->context_);
			impl_->context_ = nullptr;
		}

		if(closeWindow)
		{
			SDL_ShowCursor(SDL_TRUE);
			if(!externalWindow_ || clearGPUObject)
			{
				SDL_DestroyWindow(window_);
				window_ = nullptr;
			}
		}
	}

	void Graphics::CleanupFrameBuffers()
	{
		if(!IsDeviceLost())
		{
			BindFramebuffer(impl_->systemFBO_);
			impl_->boundFBO_ = impl_->systemFBO_;
			impl_->fboDirty_ = true;

			for(HashMap<unsigned long long, FrameBufferObject>::Iterator i = impl_->frameBuffers_.Begin();
				i != impl_->frameBuffers_.End(); ++i)
			{
				DeleteFramebuffer(i->second_.fbo_);
			}

			if(impl_->resolveSrcFBO_)
				DeleteFramebuffer(impl_->resolveSrcFBO_);

			if(impl_->resolveDestFBO_)
				DeleteFramebuffer(impl_->resolveDestFBO_);
		}
		else
		{
			impl_->boundFBO_ = 0;
		}

		impl_->resolveSrcFBO_ = 0;
		impl_->resolveDestFBO_ = 0;
		impl_->frameBuffers_.Clear();
	}

	bool Graphics::IsDeviceLost() const
	{
		return impl_->context_ == nullptr;
	}

	void Graphics::DeleteFramebuffer(unsigned fbo)
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glDeleteFramebuffersEXT(1, &fbo);
		else
#endif
			glDeleteFramebuffers(1, &fbo);
	}

	void Graphics::BindFramebuffer(unsigned fbo)
	{
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		else
#endif
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	unsigned Graphics::CreateFrameBuffer()
	{
		unsigned newFbo = 0;
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
			glGenFramebuffersEXT(1, &newFbo);
		else
#endif
			glGenFramebuffers(1, &newFbo);
	}

	void Graphics::PrepareDraw()
	{
#ifndef GL_ES_VERSION_2_0
		if(gl3Support)
		{
			for(auto i = impl_->dirtyConstantBuffers_.Begin(); i != impl_->dirtyConstantBuffers_.End(); ++i)
			{
				(*i)->Apply();
			}
		}
#endif

//todo, when fboDirty_ is true?
		if(impl_->fboDirty_)
		{
			impl_->fboDirty_ = false;

			//Note, First check if no framebuffer is needed. In that case simply return backbuffer rendering
			bool noFbo = !depthStencil_;
			if(noFbo)
			{
				for(auto& renderTarget : renderTargets_)
				{
					if(renderTarget)
					{
						noFbo = false;
						break;
					}
				}
			}

			if(noFbo)
			{
				if(impl_->boundFBO_ != impl_->systemFBO_)
				{
					BindFramebuffer(impl_->systemFBO_);
					impl_->boundFBO_ = impl_->systemFBO_;
				}
#ifndef GL_ES_VERSION_2_0
				if(sRGBWriteSupport_)
				{
					bool sRGBWrite = sRGB_;
					if(sRGBWrite != impl_->sRGBWrite_)
					{
						if(sRGBWrite)
							glEnable(GL_FRAMEBUFFER_SRGB_EXT);
						else
							glDisable(GL_FRAMEBUFFER_SRGB_EXT);

						impl_->sRGBWrite_ = sRGBWrite;
					}
				}
#endif
				return;
			}

			//Search a new framebuffer based on format & size, or create new
			IntVector2 rtSize = Graphics::GetRenderTargetDimensions();
			unsigned format = 0;
			if(renderTargets_[0])
				format = renderTargets_[0]->GetParentTexture()->GetFormat();
			else if(depthStencil_)
				format = depthStencil_->GetParentTexture()->GetFormat();

			auto fboKey = (unsigned long long)format << 32u | rtSize.x_ << 16u | rtSize.y_;
			auto i = impl_->frameBuffers_.Find(fboKey);
			if(i == impl_->frameBuffers_.End())
			{
				FrameBufferObject newFbo;
				newFbo.fbo_ = CreateFrameBuffer();
				i = impl_->frameBuffers_.Insert(MakePair(fboKey, newFbo));
			}

			if(impl_->boundFBO_ != i->second_.fbo_)
			{
				BindFramebuffer(i->second_.fbo_);
				impl_->boundFBO_ = i->second_.fbo_;
			}

#ifndef GL_ES_VERSION_2_0
			// Setup readbuffers & drawBuffers if needed
			if(i->second_.readBuffers_ != GL_NONE)
			{
				glReadBuffer(GL_NONE);
				i->second_.readBuffers_ = GL_NONE;
			}
			unsigned newDrawBuffers = 0;
			for(unsigned j = 0; j < MAX_RENDERTARGETS; ++j)
			{
				if(renderTargets_[j])
					newDrawBuffers |= 1u <<j;
			}

			if(newDrawBuffers != i->second_.drawBuffers_)
			{
				//Check for no color renderTargets (depth rendering only)
				if(!newDrawBuffers)
					glDrawBuffer(GL_NONE);
				else
				{
					int drawBufferIds[MAX_RENDERTARGETS];
					unsigned drawBufferCount = 0;

					for(unsigned j = 0; j< MAX_RENDERTARGETS; ++j)
					{
						if(renderTargets_[j])
						{
							if(!gl3Support)
								drawBufferIds[drawBufferCount++] = GL_COLOR_ATTACHMENT0_EXT + j;
							else
								drawBufferIds[drawBufferCount++] = GL_COLOR_ATTACHMENT0 + j;
						}
					}
					glDrawBuffers(drawBufferCount, (const GLenum*)drawBufferIds);
				}

				i->second_.drawBuffers_ = newDrawBuffers;
			}
#endif
			for(unsigned j=0; j< MAX_RENDERTARGETS; ++j)
			{
				if(renderTargets_[j])
				{
					Texture* texture = renderTargets_[j]->GetParentTexture();
					//Bind either a renderbuffer or texture, depending on what is available
					unsigned renderBufferID = renderTargets_[j]->GetRenderBuffer();
					if(!renderBufferID)
					{
						// If texture's parameters are dirty, update before attaching
						if(texture->GetParameterDirty())
						{
							SetTextureForUpdate(texture);
							texture->UpdateParameters();
							SetTexture(0, nullptr);
						}

						if(i->second_.colorAttachments_[j] != renderTargets_[j])
						{
							BindColorAttachment(j, renderTargets_[j]->GetTarget(), texture->GetGPUObjectName(), false);
							i->second_.colorAttachments_[j] = renderTargets_[j];
						}
					}
					else
					{
						if(i->second_.colorAttachments_[j] != renderTargets_[j])
						{
							BindColorAttachment(j, renderTargets_[j]->GetTarget(), renderBufferID, true);
							i->second_.colorAttachments_[j] = renderTargets_[j];
						}
					}
				}
				else
				{
					if(i->second_.colorAttachments_[j])
					{
						BindColorAttachment(j, GL_TEXTURE_2D, 0, false);
						i->second_.colorAttachments_[j] = nullptr;
					}
				}
			}

			if(depthStencil_)
			{
				// Bind either a renderbuffer or a depth texture, depending on what is available
				Texture* texture = depthStencil_->GetParentTexture();
#ifndef GL_ES_VERSION_2_0
				bool hasTencil = texture->GetFormat() == GL_DEPTH24_STENCIL8_EXT;
#else
				bool hasStencil = texture->GetFormat() == GL_DEPTH24_STENCIL8_OES;
#endif
				unsigned renderBufferID = depthStencil_->GetRenderBuffer();
				if(!renderBufferID)
				{
					if(texture->GetParameterDirty())
					{
						SetTextureForUpdate(texture);
						texture->UpdateParameters();
						SetTexture(0, nullptr);
					}

					if(i->second_.depthAttachment_ != depthStencil_)
					{
						BindDepthAttachment(texture->GetGPUObjectName(), false);
						BindStencilAttachment(hasTencil ? texture->GetGPUObjectName() : 0, false);
						i->second_.depthAttachment_ = depthStencil_;
					}
				}
				else
				{
					if(i->second_.depthAttachment_ != depthStencil_)
					{
						BindDepthAttachment(renderBufferID, true);
						BindStencilAttachment(hasTencil ? renderBufferID : 0, true);
						i->second_.depthAttachment_ = depthStencil_;
					}
				}
			}
			else // no depthstencil
			{
				if(i->second_.depthAttachment_)
				{
					BindDepthAttachment(0, false);
					BindStencilAttachment(0, false);
					i->second_.depthAttachment_ = nullptr;
				}
			}

#ifndef GL_ES_VERSION_2_0
			if(sRGBWriteSupport_)
			{
				bool sRGBWrite = renderTargets_[0] ? renderTargets_[0]->GetParentTexture()->GetSRGB() : sRGB_;
				if(sRGBWrite != impl_->sRGBWrite_)
				{
					if(sRGBWrite)
						glEnable(GL_FRAMEBUFFER_SRGB_EXT);
					else
						glDisable(GL_FRAMEBUFFER_SRGB_EXT);
					impl_->sRGBWrite_ = sRGBWrite;
				}
			}
#endif
		}

		if (impl_->vertexBuffersDirty_)
		{
			unsigned assignedLocations = 0;
			for(unsigned i= MAX_VERTEX_STREAMS -1; i>= 0; --i)
			{
				VertexBuffer* buffer = vertexBuffers_[i];
				if(!buffer || !buffer->GetGPUObjectName() || impl_->vertexAttributes_)
					continue;
				const PODVector<VertexElement>& elements = buffer->GetElements();

				for(auto j = elements.Begin(); j != elements.End(); ++j)
				{
					const VertexElement& element = *j;
					auto k = impl_->vertexAttributes_->Find(MakePair((unsigned char)element.semantic_, element.index_));
					if(k != impl_->vertexAttributes_->End())
					{
						unsigned location = k->second_;
						unsigned locationMask = 1u << location;
						if(assignedLocations & location)
							continue;   //Already assigned by higher index vertex buffer

						assignedLocations |= locationMask;
						if(!(impl_->enabledVertexAttributes_ & locationMask))
						{
							glEnableVertexAttribArray(location);
							impl_->enabledVertexAttributes_ |= locationMask;
						}

						// Enable/disable instancing divisor as necessary
						unsigned dataStart = element.offset_;
						if(element.perInstance_)
						{
							dataStart += impl_->lastInstanceOffset_ * buffer->GetVertexSize();
							if(!(impl_->instancingVertexAttributes_ & locationMask))
							{
								SetVertexAttribDivisor(location, 1);
								impl_->instancingVertexAttributes_ |= locationMask;
							}
						}
						else
						{
							if(impl_->instancingVertexAttributes_ & locationMask)
							{
								SetVertexAttribDivisor(location, 0);
								impl_->instancingVertexAttributes_ &= ~locationMask;
							}
						}

						SetVBO(buffer->GetGPUObjectName());
						glVertexAttribPointer(location, glElementComponents[element.type_], glElementTypes[element.type_],
											element.type_ == TYPE_UBYTE4_NORM ? GL_TRUE : GL_FALSE, (unsigned)buffer->GetVertexSize(),
											  (const void*)(size_t)dataStart);
					}
				}
			}

			unsigned disableVertexAttributes = impl_->enabledVertexAttributes_ & (~impl_->usedVertexAttributes_);
			unsigned location = 0;
			while (disableVertexAttributes)
			{
				if(disableVertexAttributes & 1u)
				{
					glDisableVertexAttribArray(location);
					impl_->enabledVertexAttributes_ &= ~(1u << location);
				}
				location++;
				disableVertexAttributes >>= 1;
			}
			impl_->vertexBuffersDirty_ = false;
		}
	}

	void Graphics::BindColorAttachment(unsigned index, unsigned target, unsigned object, bool isRenderBuffer)
	{
		if(!object)
			isRenderBuffer = false;
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
		{
			if(!isRenderBuffer)
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index, target, object, 0);
			else
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index, GL_RENDERBUFFER_EXT, object);
		}
		else
#endif
		{
			if(!isRenderBuffer)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, object, 0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, object);
		}
	}


	void Graphics::SetStencilTest(bool enable, CompareMode mode, StencilOp pass, StencilOp fail, StencilOp zFail,
	                              unsigned int stencilRef, unsigned int compareMask, unsigned int writeMask)
	{
#ifndef GL_ES_VERSION_2_0
		if(enable != stencilTest_)
		{
			if(enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			stencilTest_ = enable;
		}

		if(enable)
		{
			if(mode != stencilTestMode_ || stencilRef != stencilRef_ || compareMask != stencilCompareMask_)
			{
				glStencilFunc(glCmpFunc[mode], stencilRef, compareMask);
				stencilTestMode_ = mode;
				stencilRef_ = stencilRef;
				stencilCompareMask_ = compareMask;
			}
			if(writeMask != stencilWriteMask_)
			{
				glStencilMask(writeMask);
				stencilWriteMask_ = writeMask;
			}
			if(pass != stencilPass_ || fail != stencilFail_ || zFail != stencilZFail_)
			{
				glStencilOp(glStencilOps[fail], glStencilOps[zFail], glStencilOps[pass]);
				stencilPass_ = pass;
				stencilZFail_ = zFail;
				stencilFail_ = fail;
			}
		}
#endif
	}

	void Graphics::SetVertexBuffer(VertexBuffer *buffer)
	{
		static PODVector<VertexBuffer*> vertexBuffers(1);
		vertexBuffers[0] = buffer;
		SetVertexBuffers(vertexBuffers);
	}

	bool Graphics::SetVertexBuffers(const PODVector<VertexBuffer *> &buffers, unsigned int instanceOffset)
	{
		// todo, why do this?
		if(buffers.Size() > MAX_VERTEX_STREAMS)
		{
			URHO3D_LOGERROR("Too many vertex buffers");
			return false;
		}

		if(instanceOffset != impl_->lastInstanceOffset_)
		{
			impl_->lastInstanceOffset_ = instanceOffset;
			impl_->vertexBuffersDirty_ = true;
		}

		for(unsigned i =0; i< MAX_VERTEX_STREAMS; ++i)
		{
			VertexBuffer* buffer = nullptr;
			if(i < buffers.Size())
				buffer = buffers[i];
			if(buffer != vertexBuffers_[i])
			{
				vertexBuffers_[i] = buffer;
				impl_->vertexBuffersDirty_ = true;
			}
		}
		return true;
	}

	ShaderVariation *Graphics::GetShader(ShaderType type, const char *name, const char *defines) const
	{
		if(lastShaderName_ != name || !lastShader_)
		{
			auto* cache = GetSubsystem<ResourceCache>();
			String fullShaderName = shaderPath_ + name + shaderExtension_;
			if(lastShaderName_ == name && !cache->Exists(fullShaderName))
				return nullptr;

			lastShader_ = cache->GetResource<Shader>(fullShaderName);
			lastShaderName_ = name;
		}

		return lastShader_ ? lastShader_->GetVariation(type, defines) : nullptr;
	}

	void Graphics::SetShaders(ShaderVariation *vs, ShaderVariation *ps)
	{
		if(vs == vertexShader_ && ps == pixelShader_)
			return;

		// Compile the shaders now if not yet compiled. If already attempted, do not retry
		if(vs && !vs->GetGPUObjectName())
		{
			if(vs->GetCompilerOutput().Empty())
			{
				bool success = vs->Create();
				if(success)
				{
					URHO3D_LOGDEBUG("Compiled vertex shader " + vs->GetFullName());
				}
				else
				{
					URHO3D_LOGERROR("Failed to compile vertex shader " + vs->GetFullName() + ":\n" + vs->GetCompilerOutput());
					vs = nullptr;
				}
			}
			else
				vs = nullptr;
		}

		if(ps && !ps->GetGPUObjectName())
		{
			if(ps->GetCompilerOutput().Empty())
			{
				bool success = ps->Create();
				if(success)
				{
					URHO3D_LOGDEBUG("Compiled pixel shader " + ps->GetFullName());
				}
				else
				{
					URHO3D_LOGERROR("Failed to compile pixel shader " + ps->GetFullName() + ":\n" + ps->GetCompilerOutput());
					ps = nullptr;
				}
			}
			else
				ps = nullptr;
		}

		if(!vs || !ps)
		{
			glUseProgram(0);
			vertexShader_ = nullptr;
			pixelShader_ = nullptr;
			impl_->shaderProgram_ = nullptr;
		}
		else
		{
			vertexShader_ = vs;
			pixelShader_ = ps;

			Pair<ShaderVariation*, ShaderVariation*> combination(vs, ps);
			ShaderProgramMap::Iterator i = impl_->shaderPrograms_.Find(combination);

			if(i != impl_->shaderPrograms_.End())
			{
				// Use the existing linked program
				if(i->second_->GetGPUObjectName())
				{
					glUseProgram(i->second_->GetGPUObjectName());
					impl_->shaderProgram_ = i->second_;
				}
				else
				{
					glUseProgram(0);
					impl_->shaderProgram_ = nullptr;
				}
			}
			else
			{
				SharedPtr<ShaderProgram> newProgram(new ShaderProgram(this, vs, ps));
				if(newProgram->Link())
				{
					URHO3D_LOGDEBUG("LINKED VERTEX SHADER " + vs->GetFullName() + " and pixel shader " + ps->GetFullName());
					impl_->shaderProgram_ = newProgram;
				}
				else
				{
					URHO3D_LOGERROR("Failed to link vertex shader " + vs->GetFullName() + " and pixel shader " + ps->GetFullName());
					glUseProgram(0);
					impl_->shaderProgram_ = nullptr;
				}

				impl_->shaderPrograms_[combination] = newProgram;
			}

			// Update the clip plane uniform n GL3 and  set constant buffers
#ifndef GL_ES_VERSION_2_0
			if(gl3Support && impl_->shaderProgram_)
			{
				const SharedPtr<ConstantBuffer>* constBuffers = impl_->shaderProgram_->GetConstantBuffers();
				for(unsigned i=0; i< MAX_SHADER_PARAMETER_GROUPS * 2; ++i)
				{
					auto buffer = constBuffers[i].Get();
					if(buffer != impl_->constantBuffers_[i])
					{
						unsigned object = buffer ? buffer->GetGPUObjectName() : 0;
						glBindBufferBase(GL_UNIFORM_BUFFER, i, object);
						impl_->boundUBO_ = object;
						impl_->constantBuffers_[i] = buffer;
						ShaderProgram::ClearGlobalParameterSource((ShaderParameterGroup)(i % MAX_SHADER_PARAMETER_GROUPS));
					}
				}

				SetShaderParameter(VSP_CLIPPLANE, useClipPlane_ ? clipPlane_ : Vector4(0.0f, 0.0f, 0.0f, 1.0f));
			}
#endif

			if(shaderPrecache_)
				shaderPrecache_->StoreShader(vertexShader_, pixelShader_);

			if(impl_->shaderProgram_)
			{
				impl_->usedVertexAttributes_ = impl_->shaderProgram_->GetUsedVertexAttributes();
				impl_->vertexAttributes_ = &impl_->shaderProgram_->GetVertexAttributes();
			}
			else
			{
				impl_->usedVertexAttributes_ = 0;
				impl_->vertexAttributes_ = nullptr;
			}

			impl_->vertexBuffersDirty_ = true;
		}
	}

	bool Graphics::NeedParameterUpdate(ShaderParameterGroup group, const void *source)
	{
		return impl_->shaderProgram_ ? impl_->shaderProgram_->NeedParameterUpdate(group, source) : false;
	}

	void Graphics::SetShaderParameter(StringHash param, const Matrix3x4 &matrix)
	{
		if(impl_->shaderProgram_)
		{
			const auto info = impl_->shaderProgram_->GetParameter(param);
			if(info)
			{
				static Matrix4 fullMatrix;
				fullMatrix.m00_ = matrix.m00_;
				fullMatrix.m01_ = matrix.m01_;
				fullMatrix.m02_ = matrix.m02_;
				fullMatrix.m03_ = matrix.m03_;

				fullMatrix.m10_ = matrix.m10_;
				fullMatrix.m11_ = matrix.m11_;
				fullMatrix.m12_ = matrix.m12_;
				fullMatrix.m13_ = matrix.m13_;

				fullMatrix.m20_ = matrix.m20_;
				fullMatrix.m21_ = matrix.m21_;
				fullMatrix.m22_ = matrix.m22_;
				fullMatrix.m23_ = matrix.m23_;


				if(info->bufferPtr_)
				{
					ConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
					{
						impl_->dirtyConstantBuffers_.Push(buffer);
					}
					buffer->SetParameter(info->offset_, sizeof(Matrix4), &fullMatrix);
					return;
				}

				glUniformMatrix4fv(info->location_, 1, GL_FALSE, fullMatrix.Data());
			}
		}
	}

	void Graphics::SetShaderParameter(StringHash param, const Matrix4 &matrix)
	{
		if(impl_->shaderProgram_)
		{
			const auto info = impl_->shaderProgram_->GetParameter(param);
			if(info)
			{
				// todo, if constant buffer exist, set it int constantbuffer, otherwise, set it in location?
				if(info->bufferPtr_)
				{
					ConstantBuffer* buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
					{
						impl_->dirtyConstantBuffers_.Push(buffer);
					}
					buffer->SetParameter(info->offset_, sizeof(Matrix4), &matrix);
					return;
				}

				glUniformMatrix4fv(info->location_, 1, GL_FALSE, matrix.Data());
			}
		}
	}

	void Graphics::SetShaderParameter(StringHash param, const Color &color)
	{
		SetShaderParameter(param, color.Data(), 4);
	}

	void Graphics::SetShaderParameter(StringHash param, const float *data, unsigned count)
	{
		//todo
	}

	void Graphics::SetShaderParameter(StringHash param, float value)
	{
		if(impl_->shaderProgram_)
		{
			const auto info = impl_->shaderProgram_->GetParameter(param);
			if(info)
			{
				if(info->bufferPtr_)
				{
					auto buffer = info->bufferPtr_;
					if(!buffer->IsDirty())
					{
						impl_->dirtyConstantBuffers_.Push(buffer);
					}
					buffer->SetParameter(info->offset_, sizeof(float), &value);
					return;
				}
				glUniform1fv(info->location_, 1, &value);
			}
		}
	}

	void Graphics::SetBlendMode(BlendMode mode, bool alphaToCoverage)
	{
		if(mode != blendMode_)
		{
			if(mode == BLEND_REPLACE)
				glDisable(GL_BLEND);
			else
			{
				glEnable(GL_BLEND);
				glBlendFunc(glSrcBlend[mode], glDestBlend[mode]);
				glBlendEquation(glBlendOp[mode]);
			}

			blendMode_ = mode;
		}

		if(alphaToCoverage != alphaToCoverage_)
		{
			if(alphaToCoverage)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			alphaToCoverage_ = alphaToCoverage;
		}
	}

	void Graphics::SetScissorTest(bool enable, const IntRect &rect)
	{
		IntVector2 rtSize(GetRenderTargetDimensions());
		IntVector2 viewPos(viewport_.left_, viewport_.top_);

		if(enable)
		{
			IntRect intRect;
			intRect.left_ = Clamp(rect.left_ + viewPos.x_, 0, rtSize.x_ - 1);
			intRect.top_ = Clamp(rect.top_ + viewPos.y_, 0, rtSize.y_ - 1);
			intRect.right_ = Clamp(rect.right_ + viewPos.x_, 0, rtSize.x_);
			intRect.bottom_ = Clamp(rect.bottom_ + viewPos.y_, 0, rtSize.y_);

			if(intRect.right_ == intRect.left_)
				intRect.right_++;
			if(intRect.bottom_ == intRect.top_)
				intRect.bottom_++;
			if(intRect.right_ < intRect.left_ || intRect.bottom_ < intRect.top_)
				enable = false;

			if(enable && scissorRect_ != intRect)
			{
				// Use d3d convention, ie, 0 is top
				//todo, why
				glScissor(intRect.left_, rtSize.y_ - intRect.bottom_, intRect.Width(), intRect.Height());
				scissorRect_ = intRect;
			}
		}
		else
		{
			scissorRect_ = IntRect::ZERO;
		}

		if(enable != scissorTest_)
		{
			if(enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
			scissorTest_ = enable;
		}

	}

	void Graphics::SetTexture(unsigned index, Texture *texture)
	{
		if(index >= MAX_TEXTURE_UNITS)
			return;

		//Note: check if texture is currently bound as rendertarget. In that case, use its backup texture, or blank if not defined
		if(texture)
		{
			if(renderTargets_[0] && renderTargets_[0]->GetParentTexture() == texture)
				texture = texture->GetBackupTexture();
			else
			{
				if(texture->GetMultiSample() > 1 && texture->GetAutoResolve() && texture->IsResolveDirty())
				{
					if(texture->GetType() == Texture2D::GetTypeStatic())
						ResolveToTexture(static_cast<Texture2D*>(texture));
					if(texture->GetType() == TextureCube::GetTypeStatic())
						ResolveToTexture(static_cast<TextureCube*>(texture));
				}
			}
		}

		if(textures_[index] != texture)
		{
			if(impl_->activeTexture_ != index)
			{
				glActiveTexture(GL_TEXTURE0+ index);
				impl_->activeTexture_ = index;
			}

			if(texture)
			{
				unsigned glType = texture->GetTarget();
				if(impl_->textureTypes_[index] && impl_->textureTypes_[index] != glType)
					glBindTexture(impl_->textureTypes_[index], 0);
				glBindTexture(glType, texture->GetGPUObjectName());
				impl_->textureTypes_[index] = glType;

				if(texture->GetParameterDirty())
					texture->UpdateParameters();
				if(texture->GetLevelDirty())
					texture->RegenerateLevels();
			}
			else if(impl_->textureTypes_[index])
			{
				glBindTexture(impl_->textureTypes_[index], 0);
				impl_->textureTypes_[index] = 0;
			}

			textures_[index] = texture;
		}
		else
		{
			if(texture && (texture->GetParameterDirty() || texture->GetLevelDirty()))
			{
				if(impl_->activeTexture_ != index)
				{
					glActiveTexture(GL_TEXTURE0 + index);
					impl_->activeTexture_ = index;
				}
				glBindTexture(texture->GetTarget(), texture->GetGPUObjectName());
				if(texture->GetParameterDirty())
					texture->UpdateParameters();
				if(texture->GetLevelDirty())
					texture->RegenerateLevels();
			}
		}
	}

	void Graphics::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		if(!vertexCount)
			return;

		PrepareDraw();

		unsigned primitiveCount;
		GLenum glPrimitiveType;

		GetGLPrimitiveType(vertexCount, type, primitiveCount, glPrimitiveType);
		glDrawArrays(glPrimitiveType, vertexStart, vertexCount);

		numPrimitives_ += primitiveCount;
		++numBatches_;
	}

	void Graphics::SetVBO(unsigned object)
	{
		if(impl_->boundFBO_ != object)
		{
			if(object)
				glBindBuffer(GL_ARRAY_BUFFER, object);
			impl_->boundFBO_ = object;
		}
	}

	void Graphics::BindDepthAttachment(unsigned object, bool isRenderBuffer)
	{
		if(!object)
			isRenderBuffer = false;
#ifndef GL_ES_VERSION_2_0
		if(!gl3Support)
		{
			if(isRenderBuffer)
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, object, 0);
			else
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, object);
		}
		else
#endif
		{
			if(!isRenderBuffer)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, object, 0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, object);
		}
	}

	void Graphics::BindStencilAttachment(unsigned object, bool isRenderBuffer)
	{
		//todo
	}


	void Graphics::SetVertexAttribDivisor(unsigned location, unsigned divisor)
	{
#ifndef GL_ES_VERSION_2_0
		if(gl3Support && instancingSupport_)
			glVertexAttribDivisor(location, divisor);
		else if(instancingSupport_)
			glVertexAttribDivisorARB(location, divisor);
#endif
	}

	bool Graphics::ResolveToTexture(Texture2D *texture)
	{
#ifndef GL_ES_VERSION_2_0
		if(!texture)
			return false;
		RenderSurface* surface = texture->GetRenderSurface();
		if(!surface || !surface->GetRenderBuffer())
			return false;

		texture->SetResolveDirty(false);
		surface->SetResolveDirty(false);

		// Note: use separate FBO for resolve to not disturb the currently set rendertargets
		if(!impl_->resolveSrcFBO_)
			impl_->resolveSrcFBO_ = CreateFrameBuffer();
		if(!impl_->resolveDestFBO_)
			impl_->resolveDestFBO_ = CreateFrameBuffer();

		if(!gl3Support)
		{
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, impl_->resolveSrcFBO_);
			glFramebufferRenderbufferEXT(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, surface->GetRenderBuffer());
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, impl_->resolveDestFBO_);
			glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture->GetGPUObjectName(), 0);
			glBlitFramebufferEXT(0, 0, texture->GetWidth(), texture->GetHeight(), 0, 0, texture->GetWidth(), texture->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
		}
		else
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, impl_->resolveSrcFBO_);
			glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, surface->GetRenderBuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, impl_->resolveDestFBO_);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetGPUObjectName(), 0);
			glBlitFramebuffer(0, 0, texture->GetWidth(), texture->GetHeight(), 0, 0, texture->GetWidth(), texture->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		BindFramebuffer(impl_->boundFBO_);
		return true;
#else
		return false;
#endif
	}

	ShaderProgram *Graphics::GetShaderProgram() const
	{
		return impl_->shaderProgram_;
	}

	ConstantBuffer *Graphics::GetOrCreateConstantBuffer(ShaderType type, unsigned index, unsigned size)
	{
		unsigned key = (index << 16u) | size;
		auto i = impl_->allConstantBuffers_.Find(key);
		if(i == impl_->allConstantBuffers_.End())
		{
			i = impl_->allConstantBuffers_.Insert(MakePair(key, SharedPtr<ConstantBuffer>(new ConstantBuffer(context_))));
			i->second_->SetSize(size);
		}
		return i->second_.Get();
	}

	void Graphics::SetUBO(unsigned object)
	{
#ifndef GL_ES_VERSION_2_0
		if(impl_->boundFBO_ != object)
		{
			if(object)
				glBindBuffer(GL_UNIFORM_BUFFER, object);
			impl_->boundUBO_ = object;
		}
#endif
	}

	TextureUnit Graphics::GetTextureUnit(const String &name)
	{
		auto i = textureUnits_.Find(name);
		if(i != textureUnits_.End())
			return i->second_;
		else
			return MAX_TEXTURE_UNITS;
	}


}

