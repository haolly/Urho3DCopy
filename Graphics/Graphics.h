//
// Created by liuhao1 on 2018/1/4.
//

#ifndef URHO3DCOPY_GRAPHICS_H
#define URHO3DCOPY_GRAPHICS_H

#include <d3d11.h>
#include <SDL/include/SDL_video.h>
#include "../Core/Object.h"
#include "../Resource/Image.h"
#include "GraphicsDefs.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "ShaderVariation.h"
#include "Direct3D11/D3D11GraphicsImpl.h"
#include "RenderSurface.h"
#include "Texture2D.h"
#include "../Math/Rect.h"
#include "../Math/Matrix4.hpp"

namespace Urho3D
{
	// CPU side scratch buffer for vertex data updates
	struct ScratchBuffer
	{
		ScratchBuffer() :
			size_(0),
			reserved_(false)
		{
		}

		SharedArrayPtr<unsigned char> data_;
		unsigned size_;
		bool reserved_;
	};

	class Graphics : public Object
	{
		URHO3D_OBJECT(Graphics, Object);
	public:
		Graphics(Context* context);
		virtual ~Graphics() override ;

		void SetExternalWindow(void* window);
		void SetWindowTitle(const String& windowTitle);
		void SetWindowIcon(Image* windowIcon);
		void SetWindowPosition(const IntVector2& position);
		void SetWindowPosition(int x, int y);

		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool resizable, bool highDPI,
					bool vsync, bool tripleBuffer, int multiSample, int monitor, int refreshRate);

		bool SetMode(int width, int height);
		void SetSRGB(bool enable);
		//Note, Dither, 抖动
		// Set whether rendering output is dithered. Default true on OpenGL. No effect on Direct3D.
		void SetDither(bool enable);

		//Set whether to flush the GPU command buffer to prevent multiple frames being queued and uneven frame timesteps.
		void SetFlushGPU(bool enable);

		void SetForceGL2(bool enable);
		void SetOrientations(const String& orientations);
		bool ToggleFullscreen();

		void Close();
		bool TakeScreenShot(Image& destImage);

		bool BeginFrame();
		void EndFrame();
		void Clear(ClearTargetFlags flags, const Color& color = Color(0.0f, 0.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

		bool ResolveToTexture(Texture2D* texture);
		bool ResolveToTexture(TextureCube* texture);

		void Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount);
		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount);
		void Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex, unsigned minVertex,
		          unsigned vertexCount);

		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount,
		                   unsigned instanceCount);
		void DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex,
		                   unsigned minVertex, unsigned vertexCount, unsigned instanceCount);

		void SetVertexBuffer(VertexBuffer* buffer);
		bool SetVertexBuffers(const PODVector<VertexBuffer*>& buffers, unsigned instanceOffset = 0);
		bool SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>>& buffers, unsigned instanceOffset = 0);

		void SetIndexBuffer(IndexBuffer* buffer);

		void SetShaders(ShaderVariation* vs, ShaderVariation* ps);

		void SetShaderParameter(StringHash param, const float* data, unsigned count);
		void SetShaderParameter(StringHash param, float value);
		void SetShaderParameter(StringHash param, int value);
		void SetShaderParameter(StringHash param, bool value);
		void SetShaderParameter(StringHash param, const Color& color);
		void SetShaderParameter(StringHash param, const Vector2& vector);
		void SetShaderParameter(StringHash param, const Matrix3& matrix);
		void SetShaderParameter(StringHash param, const Vector3& vector);
		void SetShaderParameter(StringHash param, const Vector4& vector);
		void SetShaderParameter(StringHash param, const Matrix4& matrix);
		void SetShaderParameter(StringHash param, const Matrix3x4& matrix);
		void SetShaderParameter(StringHash param, const Variant& value);

		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source);
		bool HasShaderParameter(StringHash param);
		bool HasTextureUnit(TextureUnit unit);

		void ClearParamterSource(ShaderParameterGroup group);
		void ClearParameterSources();
		void ClearTransformSources();


		void SetTexture(unsigned index, Texture* texture);
		void SetTextureForUpdate(Texture* texture);
		void SetTextureParamtersDirty();
		void SetDefaultTextureFilterMode(TextureFilterMode mode);
		void SetDefaultTextureAnisotropy(unsigned level);
		void ResetRenderTargets();
		void ResetRenderTarget(unsigned index);
		void ResetDepthStencil();
		void SetRenderTarget(unsigned index, RenderSurface* renderTarget);
		void SetRenderTarget(unsigned index, Texture2D* texture);
		void SetDepthStencil(RenderSurface* depthStencil);
		void SetDepthStencil(Texture2D* texture);
		void SetViewport(const IntRect& rect);
		void SetBlendMode(BlendMode mode, bool alphaToCoverage = false);
		void SetColorWrite(bool enable);
		void SetCullMode(CullMode mode);
		void SetDepthBias(float constantBias, float slopScaledBias);
		void SetDepthTest(CompareMode mode);
		void SetDepthWrite(bool enable);
		void SetFillMode(FillMode mode);
		void SetLineAntiAlias(bool enable);
		void SetScissorTest(bool enable, const Rect& rect = Rect::FULL, bool borderInclusive = true);
		void SetScissorTest(bool enable, const IntRect& rect);
		//todo

		bool IsInitialized() const;
		GraphicsImpl* GetImpl() const { return impl_; }
		void* GetExternalWindow() const { return externalWindow_; }
		SDL_Window* GetWindow() const { return window_; }

		const String& GetWindowTitle() const { return windowTitle_; }
		const String& GetApiName() const { return apiName_; }

		IntVector2 GetWindowPosition() const;
		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }

		int GetMultiSample() const { return multiSample_; }

		IntVector2 GetSize() const { return IntVector2(width_, height_); }
		bool GetFullScreen() const { return fullScreen_; }

		bool GetBorderless() const { return borderless_; }

		bool GetResizable() const { return resizable_; }

		bool GetHighDPI() const { return highDPI_; }

		bool GetVSync() const { return vsync_; }

		int GetRefreshRate() const { return refreshRate_; }

		int GetMonitor() const { return monitor_; }

		//todo

		bool IsDeviceLost() const;
		unsigned GetNumPrimitives() const { return numPrimitives_; }
		unsigned GetNumBatches() const { return numBatches_; }
		unsigned GetDummyColorFormat() const { return dummyColorFormat_; }
		unsigned GetShadowMapFormat() const { return shadowMapFormat_; }
		//todo

		bool GetSRGBSupport() const { return sRGBSupport_; }
		bool GetSRGBWriteSupport() const { return sRGBWriteSupport_; }

		PODVector<IntVector3> GetResolutions(int monitor) const;
		PODVector<int> GetMultiSampleLevels() const;
		IntVector2 GetDesktopResolution(int monitor) const;
		int GetMonitorCount() const;
		int GetCurrentMonitor() const;
		bool GetMaximized() const;
		Vector3 GetDisplayDPI(int monitor = 0) const;

		unsigned GetFormat(CompressedFormat format) const;

		ShaderVariation* GetShader(ShaderType type, const String& name, const String& defines = String::EMPTY) const;
		ShaderVariation* GetShader(ShaderType type, const char* name, const char* defines) const;
		VertexBuffer* GetVertexBuffer(unsigned index) const;
		IndexBuffer* GetIndexBuffer() const { return indexBuffer_; }

		ShaderVariation* GetVertexShader() const { return vertexShader_; }
		ShaderVariation* GetPixelShader() const { return pixelShader_; }

		ShaderProgram* GetShaderProgram() const;
		TextureUnit GetTextureUnit(const String& name);
		const String& GetTextureUnitName(TextureUnit unit);

		Texture* GetTexture(unsigned index) const;

		TextureFilterMode GetDefaultTextureFilterMode() const
		{
			return defaultTextureFilterMode_;
		}

		unsigned GetDefaultTextureAnisotropy() const
		{
			return defaultTextureAnisotropy_;
		}

		RenderSurface* GetRenderTarget(unsigned index) const;

		RenderSurface* GetDepthStencil() const { return depthStencil_; }

		IntRect GetViewport() const { return viewport_; }

		//todo
		const String& GetShaderCacheDir() const { return shaderCacheDir_; }
		IntVector2 GetRenderTargetDimensions() const;

		void OnWindowResized();
		void OnWindowMoved();
		void Restore();
		void Maxmize();
		void Minimize();
		void Raise() const;

		void AddGPUObject(GPUObject* object);
		void RemoveGPUObject(GPUObject* object);
		void* ReserveScratchBuffer(unsigned size);
		void FreeScratchBuffer(void* buffer);
		void CleanupShaderPrograms(ShaderVariation* variation);
		void CleanupRenderSurface(RenderSurface* surface);
		ConstantBuffer* GetOrCreateConstantBuffer(ShaderType type, unsigned index, unsigned size);
		void MarkFBODirty();
		void SetVBO(unsigned object);
		void SetUBO(unsigned object);

		static unsigned GetAlphaFormat();
		static unsigned GetLuminanceFormat();
		static unsigned GetLuminanceAlphaFormat();
		static unsigned GetRGBFormat();
		static unsigned GetRGBAFormat();
		static unsigned GetRGBA16Format();
		//todo

		static const Vector2& GetPixelUVOffset() { return pixelUVOffset; }
		static unsigned GetMaxBones();
		static bool GetGL3Support();

	private:
		bool OpenWindow(int width, int height, bool resizable, bool borderless);
		void CreateWindowIcon();
		void AdjustWindow(int& newWidth, int& newHeight, bool& newFullScreen, bool& newBorderless, int& monitor);
		bool CreateDevice(int width, int height, int multiSample);
		bool UpdateSwapChain(int width, int height);
		bool CreateInterface();
		bool CreateDevice(unsigned adapter, unsigned deviceType);
		void ResetDevice();
		// Notify all GPU resources so they can release themselves as needed. Used only on Direct3D9
		void OnDeviceLost();
		// Notify all GPU resources so they can recreate themselves as needed. Used only on Direct3D9
		void OnDeviceReset();
		// Set vertex buffer stream frequency. Used only on Direct3D9
		void SetStreamFrequency(unsigned index, unsigned frequency);
		void ResetStreamFrequencies();
		void CheckFeatureSupport();
		void ResetCachedState();
		void SetTextureUnitMappings();
		void PrepareDraw();
		void CreateResolveTexture();
		void CleanupFrameBuffers();
		unsigned CreateFrameBuffer();
		void DeleteFramebuffer(unsigned fbo);
		void BindFramebuffer(unsigned pos);
		//todo


		Mutex gpuObjectMutex_;
		GraphicsImpl* impl_;
		SDL_Window* window_;
		String windowTitle_;
		WeakPtr<Image> windowIcon_;
		void* externalWindow_;
		int width_;
		int height_;
		IntVector2 position_;
		int multiSample_;
		bool fullScreen_;
		bool borderless_;
		bool resizable_;
		bool highDPI_;
		bool vsync_;
		int refreshRate_;
		int monitor_;
		bool tripleBuffer_;
		bool flushGPU_;
		bool forceGL2_;
		bool sRGB_;
		bool lightPrepassSupport_;
		bool deferredSupport_;
		bool anisotronpySupport_;
		bool dxtTextureSupport_;
		bool etcTextureSupport_;
		bool pvrtcTextureSupport_;
		bool hardwareShadowSupport_;
		bool instancingSupport_;
		bool sRGBSupport_;
		bool sRGBWriteSupport_;
		// Number of primitives this frame
		unsigned numPrimitives_;
		unsigned numBatches_;
		unsigned maxScratchBufferRequest_;

		PODVector<GPUObject*> gpuObjects_;
		Vector<ScratchBuffer> scratchBuffers_;
		unsigned dummyColorFormat_;
		unsigned shadowMapFormat_;
		unsigned hiresShadowMapFormat_;
		VertexBuffer* vertexBuffers_[MAX_VERTEX_STREAMS];
		IndexBuffer* indexBuffer_;
		unsigned long long vertexDeclarationHash_;
		unsigned primitiveType_;
		ShaderVariation* vertexShader_;
		ShaderVariation* pixelShader_;
		Texture* textures_[MAX_TEXTURE_UNITS];
		HashMap<String, TextureUnit > textureUnits_;
		RenderSurface* renderTargets_[MAX_RENDERTARGETS];
		RenderSurface* depthStencil_;
		IntRect viewport_;
		TextureFilterMode defaultTextureFilterMode_;
		unsigned defaultTextureAnisotropy_;
		BlendMode blendMode_;
		//todo
		bool alphaToCoverage_;
		bool colorWrite_;
		CullMode cullMode_;
		float constantDepthBias_;
		float slopeScaleDepthBias_;
		CompareMode depthTestMode_;
		bool depthWrite_;
		bool lineAntiAlias_;
		FillMode fillMode_;
		bool scissorTest_;
		IntRect scissorRect_;
		CompareMode stencilTestMode_;
		StencilOp stencilPass_;
		StencilOp stencilFail_;
		StencilOp stencilZFail_;
		unsigned stencilRef_;
		unsigned stencilCompareMask_;
		unsigned stencilWriteMask_;
		Vector4 clipPlane_;
		bool stencilTest_;
		bool useClipPlane_;
		const void* shaderParameterSources_[MAX_SHADER_PARAMETER_GROUPS];
		String shaderPath_;
		String shaderCacheDir_;
		String shaderExtension_;
		mutable WeakPtr<Shader> lastShader_;
		mutable String lastShaderName_;
		SharedPtr<ShaderPrecache> shaderPrecache_;
		String orientations_;
		String apiName_;
		static const Vector2 pixelUVOffset;
		static bool gl3Support;
	};

	void RegisterGraphicsLibrary(Context* context);
}



#endif //URHO3DCOPY_GRAPHICS_H
