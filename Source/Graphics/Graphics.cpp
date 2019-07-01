//
// Created by liuhao1 on 2018/1/4.
//

#include "Graphics.h"
#include "Technique.h"
#include "RenderSurface.h"
#include "Material.h"
#include "Drawable.h"

namespace Urho3D
{

	void Graphics::SetExternalWindow(void *window)
	{

	}

	void Graphics::SetWindowTitle(const String &windowTitle)
	{

	}

	void Graphics::SetWindowIcon(Image *windowIcon)
	{

	}

	void Graphics::SetWindowPosition(const IntVector2 &position)
	{

	}

	void Graphics::SetWindowPosition(int x, int y)
	{

	}

	bool
	Graphics::SetMode(int width, int height, bool fullScreen, bool borderless, bool resizable, bool highDPI, bool vsync,
	                  bool tripleBuffer, int multiSample, int monitor, int refreshRate)
	{
		return false;
	}

	bool Graphics::SetMode(int width, int height)
	{
		return false;
	}

	void Graphics::SetSRGB(bool enable)
	{

	}

	void Graphics::SetDither(bool enable)
	{

	}

	void Graphics::SetFlushGPU(bool enable)
	{

	}

	void Graphics::SetForceGL2(bool enable)
	{

	}

	void Graphics::SetOrientations(const String &orientations)
	{

	}

	bool Graphics::ToggleFullscreen()
	{
		return false;
	}


	bool Graphics::TakeScreenShot(Image &destImage)
	{
		return false;
	}

	bool Graphics::BeginFrame()
	{
		return false;
	}

	void Graphics::EndFrame()
	{

	}

	void Graphics::Clear(ClearTargetFlags flags, const Color &color, float depth, unsigned int stencil)
	{

	}

	void Graphics::Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex,
	                    unsigned vertexCount)
	{

	}

	void Graphics::Draw(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex,
	                    unsigned minVertex, unsigned vertexCount)
	{

	}

	void Graphics::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex,
	                             unsigned vertexCount, unsigned instanceCount)
	{

	}

	void Graphics::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned baseVertexIndex,
	                             unsigned minVertex, unsigned vertexCount, unsigned instanceCount)
	{

	}


	void Graphics::SetShaderParameter(StringHash param, int value)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, bool value)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Vector2 &vector)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Matrix3 &matrix)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Vector3 &vector)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Vector4 &vector)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Variant &value)
	{

	}


	bool Graphics::HasShaderParameter(StringHash param)
	{
		return false;
	}

	bool Graphics::HasTextureUnit(TextureUnit unit)
	{
		return false;
	}

	void Graphics::ClearParamterSource(ShaderParameterGroup group)
	{

	}

	void Graphics::ClearParameterSources()
	{

	}

	void Graphics::ClearTransformSources()
	{

	}

	void Graphics::SetTextureForUpdate(Texture *texture)
	{

	}

	void Graphics::SetTextureParamtersDirty()
	{

	}

	bool Graphics::IsInitialized() const
	{
		return window_ != nullptr ;
	}

	IntVector2 Graphics::GetWindowPosition() const
	{
		return IntVector2();
	}

	IntVector2 Graphics::GetRenderTargetDimensions() const
	{
		//todo
	}

	void Graphics::OnWindowResized()
	{

	}

	void Graphics::OnWindowMoved()
	{

	}

	void Graphics::Restore()
	{

	}

	void Graphics::Maxmize()
	{

	}

	void Graphics::Minimize()
	{

	}

	void Graphics::Raise() const
	{

	}

	void Graphics::AddGPUObject(GPUObject *object)
	{
		MutexLock lock(gpuObjectMutex_);
		gpuObjects_.Push(object);
	}

	void Graphics::RemoveGPUObject(GPUObject *object)
	{
		MutexLock lock(gpuObjectMutex_);
		gpuObjects_.Remove(object);
	}

	void *Graphics::ReserveScratchBuffer(unsigned size)
	{
		if(!size)
			return nullptr;
		if(size > maxScratchBufferRequest_)
			maxScratchBufferRequest_ = size;

		// First check for a free buffer that is large enough
		for(auto it = scratchBuffers_.Begin(); it != scratchBuffers_.End(); ++it)
		{
			if(!it->reserved_ && it->size_ >= size)
			{
				it->reserved_ = true;
				return it->data_.Get();
			}
		}

		// Then check if a free buffer can be resized
		for(auto it = scratchBuffers_.Begin(); it != scratchBuffers_.End(); ++it)
		{
			if(!it->reserved_)
			{
				//Note, will release origin data first
				it->data_ = new unsigned char[size];
				it->size_ = size;
				it->reserved_ = true;

				URHO3D_LOGDEBUG("Resized scratch buffer to size " + String(size));
				return it->data_.Get();
			}
		}

		// Finally allocate a new buffer
		ScratchBuffer newBuffer;
		newBuffer.data_ = new unsigned char[size];
		newBuffer.size_ = size;
		newBuffer.reserved_ = true;
		scratchBuffers_.Push(newBuffer);
		return newBuffer.data_.Get();
	}

	void Graphics::FreeScratchBuffer(void *buffer)
	{
		if(!buffer)
			return;
		for(auto it = scratchBuffers_.Begin(); it != scratchBuffers_.End(); ++it)
		{
			if(it->reserved_ && it->data_.Get() == buffer)
			{
				it->reserved_ = false;
				return;
			}
		}

		URHO3D_LOGWARNING("Reserved scratch buffer " + ToStringHex((unsigned)(size_t)buffer) + " not found");
	}

	void Graphics::CleanupShaderPrograms(ShaderVariation *variation)
	{

	}

	void Graphics::CleanupRenderSurface(RenderSurface *surface)
	{

	}

	void Graphics::MarkFBODirty()
	{

	}

	unsigned Graphics::GetAlphaFormat()
	{
		return 0;
	}

	unsigned Graphics::GetLuminanceFormat()
	{
		return 0;
	}

	unsigned Graphics::GetLuminanceAlphaFormat()
	{
		return 0;
	}

	unsigned Graphics::GetRGBFormat()
	{
		return 0;
	}

	unsigned Graphics::GetRGBAFormat()
	{
		return 0;
	}

	unsigned Graphics::GetRGBA16Format()
	{
		return 0;
	}

	bool Graphics::CreateInterface()
	{
		return false;
	}

	bool Graphics::CreateDevice(unsigned adapter, unsigned deviceType)
	{
		return false;
	}

	void Graphics::ResetDevice()
	{

	}

	void Graphics::OnDeviceLost()
	{

	}

	void Graphics::OnDeviceReset()
	{

	}

	void Graphics::SetStreamFrequency(unsigned index, unsigned frequency)
	{

	}

	void Graphics::ResetStreamFrequencies()
	{

	}

	void Graphics::CheckFeatureSupport()
	{

	}

	void Graphics::CreateResolveTexture()
	{

	}



	void Graphics::CreateWindowIcon()
	{
		if(windowIcon_)
		{
			SDL_Surface* surface = windowIcon_->GetSDLSurface();
			if(surface)
			{
				SDL_SetWindowIcon(window_, surface);
				SDL_FreeSurface(surface);
			}
		}
	}

	void Graphics::ResetRenderTargets()
	{
		for(unsigned i=0; i< MAX_RENDERTARGETS; ++i)
			SetRenderTarget(i, (RenderSurface*)nullptr);
		SetDepthStencil((RenderSurface*)nullptr);
		SetViewport(IntRect(0, 0, width_, height_));
	}

	void Graphics::SetDefaultTextureFilterMode(TextureFilterMode mode)
	{

	}

	void Graphics::SetDefaultTextureAnisotropy(unsigned level)
	{

	}

	void Graphics::SetRenderTarget(unsigned index, Texture2D *texture)
	{
		//todo
	}

	void Graphics::SetDepthStencil(Texture2D *texture)
	{

	}

	void Graphics::SetViewport(const IntRect &rect)
	{

	}

	PODVector<IntVector3> Graphics::GetResolutions(int monitor) const
	{
		return PODVector<IntVector3>();
	}

	PODVector<int> Graphics::GetMultiSampleLevels() const
	{
		return PODVector<int>();
	}

	IntVector2 Graphics::GetDesktopResolution(int monitor) const
	{
		return IntVector2();
	}

	int Graphics::GetMonitorCount() const
	{
		return 0;
	}

	int Graphics::GetCurrentMonitor() const
	{
		return 0;
	}

	bool Graphics::GetMaximized() const
	{
		return false;
	}

	Vector3 Graphics::GetDisplayDPI(int monitor) const
	{
		return Vector3();
	}

	unsigned Graphics::GetFormat(CompressedFormat format) const
	{
		return 0;
	}

	void RegisterGraphicsLibrary(Context* context)
	{
		//todo
		Material::RegisterObject(context);
		Shader::RegisterObject(context);
		Technique::RegisterObject(context);
		Texture2D::RegisterObject(context);
		Drawable::RegisterObject(context);
	}
}