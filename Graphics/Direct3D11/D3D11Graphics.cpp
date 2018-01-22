//
// Created by liuhao1 on 2018/1/17.
//

#include <SDL/include/SDL.h>
#include <SDL/include/SDL_syswm.h>
#include "../Graphics.h"

namespace Urho3D
{
	const Vector2 Graphics::pixelUVOffset(0.0f, 0.0f);
	bool Graphics::gl3Support = false;

	static HWND GetWindowHandle(SDL_Window* window)
	{
		SDL_SysWMinfo sysInfo;

		SDL_VERSION(&sysInfo.version);
		SDL_GetWindowWMInfo(window, &sysInfo);
		return sysInfo.info.win.window;
	}

	Graphics::Graphics(Context *context) :
			Object(context),
			impl_(new GraphicsImpl()),
			window_(nullptr),
			externalWindow_(nullptr),
			width_(0),
			height_(0),
			position_(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED),
			multiSample_(1),
			fullScreen_(false),
			borderless_(false),
			resizable_(false),
			highDPI_(false),
			vsync_(false),
			monitor_(0),
			refreshRate_(0),
			tripleBuffer_(false),
			flushGPU_(false),
			forceGL2_(false),
			sRGB_(false),
			anisotronpySupport_(false),
			dxtTextureSupport_(false),
			etcTextureSupport_(false),
			pvrtcTextureSupport_(false),
			hardwareShadowSupport_(false),
			lightPrepassSupport_(false),
			deferredSupport_(false),
			instancingSupport_(false),
			sRGBSupport_(false),
			sRGBWriteSupport_(false),
			numPrimitives_(0),
			numBatches_(0),
			maxScratchBufferRequest_(0),
			defaultTextureFilterMode_(FILTER_TRILINEAR),
			defaultTextureAnisotropy_(4),
			shaderPath_("Shader/HLSL/"),
			shaderExtension_(".hlsl"),
			orientations_("LandscapeLeft LandscapeRight"),
			apiName_("D3D11")
	{
		SetTextureUnitMappings();
		ResetCachedState();

		context_->RequireSDL(SDL_INIT_VIDEO);

		RegisterGraphicsLibrary(context_);
	}

	void Graphics::ResetCachedState()
	{
		for(unsigned i=0; i<MAX_VERTEX_STREAMS; ++i)
		{
			vertexBuffers_[i] = nullptr;
			impl_->vertexBuffers_[i] = nullptr;
			impl_->vertexSize_[i] = 0;
			impl_->vertexOffsets_[i] = 0;
		}

		for(unsigned i=0; i<MAX_TEXTURE_UNITS; ++i)
		{
			textures_[i] = nullptr;
			impl_->shaderResourceView_[i] = nullptr;
			impl_->samplers_[i] = nullptr;
		}

		for(unsigned i=0; i<MAX_RENDERTARGETS; ++i)
		{
			renderTargets_[i] = nullptr;
			impl_->renderTargetViews_[i] = nullptr;
		}

		for(unsigned i=0; i<MAX_SHADER_PARAMETER_GROUPS; ++i)
		{
			impl_->constantBuffers_[VS][i] = nullptr;
			impl_->constantBuffers_[PS][i] = nullptr;
		}

		depthStencil_ = nullptr;
		impl_->depthStencilView_ = nullptr;
		viewport_ = IntRect(0, 0, width_, height_);

		indexBuffer_ = nullptr;
		vertexDeclarationHash_ = 0;
		primitiveType_ = 0;
		vertexShader_ = nullptr;
		pixelShader_ = nullptr;
		blendMode_ = BLEND_REPLACE;
		alphaToCoverage_ = false;
		colorWrite_ = true;
		cullMode_ = CULL_CCW;
		constantDepthBias_ = 0.0f;
		depthTestMode_ = CMP_LESSEQUAL;
		depthWrite_ = true;
		fillMode_ = FILL_SOLID;
		lineAntiAlias_ = false;
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
		impl_->renderTargetDirty_ = true;
		impl_->texturesDirty_ = true;
		impl_->vertexDeclarationDirty_ = true;
		impl_->blendStateDirty_ = true;
		impl_->depthStateDirty_ = true;
		impl_->rasterizerStateDirty_ = true;
		impl_->scissorRectDirty_ = true;
		impl_->stencilRefDirty_ = true;
		impl_->blendStateHash_ = M_MAX_UNSIGNED;
		impl_->depthStateHash_ = M_MAX_UNSIGNED;
		impl_->rasterizerStateHash_ = M_MAX_UNSIGNED;
		impl_->firstDirtyTexture_ = impl_->lastDirtyTexture_ = M_MAX_UNSIGNED;
		impl_->firstDirtyVB_ = impl_->lastDirtyVB_ = M_MAX_UNSIGNED;
		impl_->dirtyConstantBuffers_.Clear();
	}

	void Graphics::SetTextureUnitMappings()
	{
		textureUnits_["DiffMap"] = TU_DIFFUSE;
		textureUnits_["DiffCubeMap"] = TU_DIFFUSE;
		textureUnits_["NormalMap"] = TU_NORMAL;
		textureUnits_["SpecMap"] = TU_SPECULAR;
		textureUnits_["EmissiveMap"] = TU_EMISSIVE;
		textureUnits_["EnvMap"] = TU_ENVIRONMENT;
		textureUnits_["EnvCubeMap"] = TU_ENVIRONMENT;
		textureUnits_["LightRampMap"] = TU_LIGHTRAMP;
		textureUnits_["LightSpotMap"] = TU_LIGHTSHAPE;
		textureUnits_["LightCubeMap"] = TU_LIGHTSHAPE;
		textureUnits_["ShadowMap"] = TU_SHADOWMAP;
		textureUnits_["FaceSelectCubeMap"] = TU_FACESELECT;
		textureUnits_["IndirectionCubeMap"] = TU_INDIRECTION;
		textureUnits_["VolumeMap"] = TU_VOLUMEMAP;
		textureUnits_["ZoneCubeMap"] = TU_ZONE;
		textureUnits_["ZoneVolumeMap"] = TU_ZONE;
	}

	bool Graphics::OpenWindow(int width, int height, bool resizable, bool borderless)
	{
		if(!externalWindow_)
		{
			unsigned flags = 0;
			if(resizable)
				flags |= SDL_WINDOW_RESIZABLE;
			if(borderless)
				flags |= SDL_WINDOW_BORDERLESS;
			window_ = SDL_CreateWindow(windowTitle_.CString(), position_.x_, position_.y_, width, height, flags);
		} else {
			window_ = SDL_CreateWindowFrom(externalWindow_, 0);
		}

		if(!window_)
		{
			URHO3D_LOGERRORF("Could not create window, root cause: '%s'", SDL_GetError());
			return false;
		}

		SDL_GetWindowPosition(window_, &position_.x_, &position_.y_);
		CreateWindowIcon();
		return true;
	}

	void Graphics::AdjustWindow(int &newWidth, int &newHeight, bool &newFullScreen, bool &newBorderless, int &monitor)
	{
		if(!externalWindow_)
		{
			if(!newWidth || !newHeight)
			{
				SDL_MaximizeWindow(window_);
				SDL_GetWindowSize(window_, &newWidth, &newHeight);
			} else {
				SDL_Rect display_rect;
				SDL_GetDisplayBounds(monitor, &display_rect);

				if(newFullScreen || (newBorderless && newWidth >= display_rect.w && newHeight >= display_rect.h ))
				{
					SDL_SetWindowPosition(window_, display_rect.x, display_rect.y);
				}

				SDL_SetWindowSize(window_, newWidth, newHeight);
			}

			// Hack fix: on SDL 2.0.4 a fullscreen->windowed transition results in a maximized window when the D3D device is reset. so hide before
			SDL_HideWindow(window_);

			SDL_SetWindowFullscreen(window_, newFullScreen ? SDL_WINDOW_FULLSCREEN : 0);
			SDL_SetWindowBordered(window_, newBorderless ? SDL_FALSE : SDL_TRUE);
			SDL_ShowWindow(window_);
		}
		else
		{
			SDL_GetWindowSize(window_, &newWidth, &newHeight);
			newFullScreen = false;
		}
	}

	bool Graphics::CreateDevice(int width, int height, int multiSample)
	{
		if(!impl_->device_)
		{
			HRESULT hr = D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE_HARDWARE,
					nullptr,
					0,
					nullptr,
					0,
					D3D11_SDK_VERSION,
					&impl_->device_,
					nullptr,
					&impl_->deviceContext_
			);

			if(FAILED(hr))
			{
				URHO3D_SAFE_RELEASE(impl_->device_);
				URHO3D_SAFE_RELEASE(impl_->deviceContext_);
				URHO3D_LOGD3DERROR("Failed to create D3D11 device", hr);
				return false;
			}

			CheckFeatureSupport();
			SetFlushGPU(flushGPU_);
		}

		PODVector<int> multiSampleLevels = GetMultiSampleLevels();
		if(!multiSampleLevels.Contains(multiSample))
			multiSample = 1;

		if(impl_->swapChain_)
		{
			impl_->swapChain_->Release();
			impl_->swapChain_ = nullptr;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		memset(&swapChainDesc, 0, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = (UINT)width;
		swapChainDesc.BufferDesc.Height = (UINT)height;
		swapChainDesc.BufferDesc.Format = sRGB_ ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = GetWindowHandle(window_);
		swapChainDesc.SampleDesc.Count = (UINT)multiSample;
		swapChainDesc.SampleDesc.Quality = impl_->GetMultiSampleQuality(swapChainDesc.BufferDesc.Format, multiSample);
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGIDevice* dxgiDevice = nullptr;
		impl_->device_->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter = nullptr;
		dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory = nullptr;
		dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
		HRESULT hr = dxgiFactory->CreateSwapChain(impl_->device_, &swapChainDesc, &impl_->swapChain_);

		// After creating the swap chain, disable automic Alt-Enter fullscreen/windowed switching
		// (the application will switch manually if it wants to)
		dxgiFactory->MakeWindowAssociation(GetWindowHandle(window_), DXGI_MWA_NO_ALT_ENTER);

		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();

		if(FAILED(hr))
		{
			URHO3D_SAFE_RELEASE(impl_->swapChain_);
			URHO3D_LOGD3DERROR("Failed to create D3D11 swap chain", hr);
			return false;
		}

		multiSample_ = multiSample;
		return true;
	}

	bool Graphics::UpdateSwapChain(int width, int height)
	{
		bool success = true;
		ID3D11RenderTargetView* nullView = nullptr;
		impl_->deviceContext_->OMSetRenderTargets(1, &nullView, nullptr);
		if(impl_->defaultRenderTargetView_)
		{
			impl_->defaultRenderTargetView_->Release();
			impl_->defaultRenderTargetView_ = nullptr;
		}
		if(impl_->defaultDepthStencilView_)
		{
			impl_->defaultDepthStencilView_->Release();
			impl_->defaultDepthStencilView_ = nullptr;
		}
		if(impl_->defaultDepthTexture_)
		{
			impl_->defaultDepthTexture_->Release();
			impl_->defaultDepthTexture_ = nullptr;
		}
		if(impl_->resolveTexture_)
		{
			impl_->resolveTexture_->Release();
			impl_->resolveTexture_ = nullptr;
		}

		impl_->depthStencilView_ = nullptr;
		for(unsigned i=0; i<MAX_RENDERTARGETS; ++i)
		{
			impl_->renderTargetViews_[i] = nullptr;
		}

		impl_->renderTargetDirty_ = true;
		impl_->swapChain_->ResizeBuffers(1, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// Create default rendertarget view representing the backbuffer
		ID3D11Texture2D* backbufferTexture;
		HRESULT  hr = impl_->swapChain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backbufferTexture);
		if(FAILED(hr))
		{
			URHO3D_SAFE_RELEASE(backbufferTexture);
			URHO3D_LOGD3DERROR("Failed to get backbuffer texture", hr);
			success = false;
		} else{
			hr = impl_->device_->CreateRenderTargetView(backbufferTexture, nullptr, &impl_->defaultRenderTargetView_);
			backbufferTexture->Release();
			if(FAILED(hr))
			{
				URHO3D_SAFE_RELEASE(impl_->defaultRenderTargetView_);
				URHO3D_LOGD3DERROR("Failed to create backbuffer rendertarget view", hr);
				success = false;
			}
		}

		// Create default depth-stencil texture and view
		D3D11_TEXTURE2D_DESC depthDesc;
		memset(&depthDesc, 0, sizeof(depthDesc));
		depthDesc.Width = (UINT)width;
		depthDesc.Height = (UINT)height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = (UINT)multiSample_;
		depthDesc.SampleDesc.Quality = impl_->GetMultiSampleQuality(depthDesc.Format, multiSample_);
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hr = impl_->device_->CreateTexture2D(&depthDesc, nullptr, &impl_->defaultDepthTexture_);
		if(FAILED(hr))
		{
			URHO3D_SAFE_RELEASE(impl_->defaultDepthTexture_);
			URHO3D_LOGD3DERROR("Failed to create backbuffer depth-stencil texture", hr);
			success = false;
		} else {
			hr = impl_->device_->CreateDepthStencilView(impl_->defaultDepthTexture_, nullptr, &impl_->defaultDepthStencilView_);
			if(FAILED(hr))
			{
				URHO3D_SAFE_RELEASE(impl_->defaultDepthStencilView_);
				URHO3D_LOGD3DERROR("Failed to create backbuffer depth-stencil view", hr);
				success = false;
			}
		}

		// Update internally held backbuffer size
		width_ = width;
		height_ = height;

		ResetRenderTargets();
		return success;
	}

	RenderSurface *Graphics::GetRenderTarget(unsigned index) const
	{
		return index < MAX_RENDERTARGETS ? renderTargets_[index] : nullptr;
	}

	void Graphics::ResetRenderTarget(unsigned index)
	{
		SetRenderTarget(index, (RenderSurface*) nullptr);
	}

	void Graphics::SetRenderTarget(unsigned index, RenderSurface *renderTarget)
	{
		if(index >= MAX_RENDERTARGETS)
			return;

		if(renderTarget != renderTargets_[index])
		{
			renderTargets_[index] = renderTarget;
			impl_->renderTargetDirty_ = true;

			// If the rendertarget is also bound as a texture, replace with back texture or null
			if(renderTarget)
			{
				Texture* parentTexture = renderTarget->GetParentTexture();

				for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
				{
					if(textures_[i] == parentTexture)
						SetTexture(i, textures_[i]->GetBackupTexture());
				}

				// If multisampled, mark the texture & surface needing resolve
				// todo why ??
				if(parentTexture->GetMultiSample() > 1 && parentTexture->GetAutoResolve())
				{
					parentTexture->SetResolveDirty(true);
					renderTarget->SetResolveDirty(true);
				}

				if(parentTexture->GetLevels() > 1)
					parentTexture->SetLevelsDirty();
			}
		}
	}

	void Graphics::ResetDepthStencil()
	{
		SetDepthStencil((RenderSurface*) nullptr);
	}

	void Graphics::SetDepthStencil(RenderSurface *depthStencil)
	{
		if(depthStencil != depthStencil_)
		{
			depthStencil_ = depthStencil;
			impl_->renderTargetDirty_ = true;
		}
	}

	Texture *Graphics::GetTexture(unsigned index) const
	{
		return index < MAX_TEXTURE_UNITS ? textures_[index] : nullptr;
	}

	void Graphics::SetTexture(unsigned index, Texture *texture)
	{
		if(index >= MAX_TEXTURE_UNITS)
			return;

		// Check if texture is currently bound as a rendertarget. In that case, use its backup texture, or blank if not defined
		if(texture)
		{
			// todo, why check at index 0, not renderTargets_[index] ??
			if(renderTargets_[0] && renderTargets_[0]->GetParentTexture() == texture)
				texture = texture->GetBackupTexture();
			else
			{
				// Resolve multisampled texture new as necessary
				if(texture->GetMultiSample() > 1 && texture->GetAutoResolve() && texture->IsResolveDirty())
				{
					if(texture->GetType() == Texture2D::GetTypeStatic())
						ResolveToTexture(static_cast<Texture2D*>(texture));
					if(texture->GetType() == TextureCube::GetTypeStatic())
						ResolveToTexture(static_cast<TextureCube*>(texture));
				}
			}

			if(texture->GetLevelDirty())
				texture->RegenerateLevels();
		}

		if(texture && texture->GetParameterDirty())
		{
			texture->UpdateParameters();
			// Force reassign
			textures_[index] = nullptr;
		}

		if(texture != textures_[index])
		{
			if(impl_->firstDirtyTexture_ == M_MAX_UNSIGNED)
				impl_->firstDirtyTexture_ = impl_->lastDirtyTexture_ = index;
			else
			{
				if(index < impl_->firstDirtyTexture_)
					impl_->firstDirtyTexture_ = index;
				if(index > impl_->lastDirtyTexture_)
					impl_->lastDirtyTexture_ = index;
			}

			textures_[index] = texture;
			impl_->shaderResourceView_[index] = texture ? (ID3D11ShaderResourceView*) texture->GetShaderResourceView() : nullptr;
			impl_->samplers_[index] = texture ? (ID3D11SamplerState*)texture->GetSampler() : nullptr;
			impl_->texturesDirty_ = true;
		}
	}

	ShaderVariation *Graphics::GetShader(ShaderType type, const String &name, const String &defines) const
	{
		return nullptr;
	}

	ShaderVariation *Graphics::GetShader(ShaderType type, const char *name, const char *defines) const
	{
		return nullptr;
	}

	VertexBuffer *Graphics::GetVertexBuffer(unsigned index) const
	{
		return index < MAX_VERTEX_STREAMS ? vertexBuffers_[index] : nullptr;
	}

	ShaderProgram *Graphics::GetShaderProgram() const
	{
		return impl_->shaderProgram_;
	}

	TextureUnit Graphics::GetTextureUnit(const String &name)
	{
		auto it = textureUnits_.Find(name);
		return it != textureUnits_.End() ? it->second_ : MAX_TEXTURE_UNITS;
	}

	const String &Graphics::GetTextureUnitName(TextureUnit unit)
	{
		for(auto it = textureUnits_.Begin(); it != textureUnits_.End(); ++it)
		{
			if(it->second_ == unit)
				return it->first_;
		}
		return String::EMPTY;
	}

	void Graphics::SetVertexBuffer(VertexBuffer *buffer)
	{
		// Note: this is not multi-instance safe
		static PODVector<VertexBuffer*> vertexBuffers(1);
		vertexBuffers[0] = buffer;
		SetVertexBuffers(vertexBuffers);
	}

	bool Graphics::SetVertexBuffers(const PODVector<VertexBuffer *> &buffers, unsigned int instanceOffset)
	{
		if(buffers.Size() > MAX_TEXTURE_UNITS)
		{
			URHO3D_LOGERROR("Too many vertex buffers");
			return false;
		}

		for(unsigned i=0; i<MAX_VERTEX_STREAMS; ++i)
		{
			VertexBuffer* buffer = nullptr;
			bool changed = false;

			buffer = i < buffers.Size() ? buffers[i] : nullptr;
			if(buffer)
			{
				const PODVector<VertexElement>& elements = buffer->GetElements();
				// Check if buffer has per-instance data
				bool hasInstanceData = elements.Size() && elements[0].perInstance_;
				unsigned offset = hasInstanceData ? instanceOffset * buffer->GetVertexSize() : 0;

				if(buffer != vertexBuffers_[i] || offset != impl_->vertexOffsets_[i])
				{
					vertexBuffers_[i] = buffer;
					impl_->vertexBuffers_[i] = (ID3D11Buffer*)buffer->GetGPUObject();
					impl_->vertexSize_[i] = buffer->GetVertexSize();
					impl_->vertexOffsets_[i] = offset;
					changed = true;
				}
			}
			else if(vertexBuffers_[i])
			{
				vertexBuffers_[i] = nullptr;
				impl_->vertexBuffers_[i] = nullptr;
				impl_->vertexSize_[i] = 0;
				impl_->vertexOffsets_[i] = 0;
				changed = true;
			}

			if(changed)
			{
				impl_->vertexDeclarationDirty_ = true;

				if(impl_->firstDirtyVB_ == M_MAX_UNSIGNED)
					impl_->firstDirtyVB_ = impl_->lastDirtyVB_ = i;

				else
				{
					if(i < impl_->firstDirtyVB_)
						impl_->firstDirtyVB_ = i;
					if(i > impl_->lastDirtyVB_)
						impl_->lastDirtyVB_ = i;
				}
			}
		}
		return true;
	}

	bool Graphics::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>> &buffers, unsigned int instanceOffset)
	{
		return false;
	}

	void Graphics::SetIndexBuffer(IndexBuffer *buffer)
	{

	}


}