//
// Created by liuhao1 on 2018/1/4.
//

#include "Graphics.h"
#include "Technique.h"
#include "RenderSurface.h"

namespace Urho3D
{

	Graphics::~Graphics()
	{

	}

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

	void Graphics::Close()
	{

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

	void Graphics::Clear(unsigned flags, const Color &color, float depth, unsigned int stencil)
	{

	}

	void Graphics::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
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

	void Graphics::SetVertexBuffer(VertexBuffer *buffer)
	{

	}

	bool Graphics::SetVertexBuffers(const PODVector<VertexBuffer *> &buffers, unsigned int instanceOffset)
	{
		return false;
	}

	bool Graphics::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>> &buffers, unsigned int instanceOffset)
	{
		return false;
	}

	void Graphics::SetIndexBuffer(IndexBuffer *buffer)
	{

	}

	void Graphics::SetShaders(ShaderVariation *vs, ShaderVariation *ps)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const float *data, unsigned count)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, float value)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, int value)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, bool value)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Color &color)
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

	void Graphics::SetShaderParameter(StringHash param, const Matrix3x4 &matrix)
	{

	}

	void Graphics::SetShaderParameter(StringHash param, const Variant &value)
	{

	}

	bool Graphics::NeedParameterUpdate(ShaderParameterGroup group, const void *source)
	{
		return false;
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
		return false;
	}

	IntVector2 Graphics::GetWindowPosition() const
	{
		return IntVector2();
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
		return nullptr;
	}

	ShaderProgram *Graphics::GetShaderProgram() const
	{
		return nullptr;
	}

	TextureUnit Graphics::GetTextureUnit(const String &name)
	{
		return TU_ENVIRONMENT;
	}

	const String &Graphics::GetTextureUnitName(TextureUnit unit)
	{
		return <#initializer#>;
	}

	IntVector2 Graphics::GetRenderTargetDimensions() const
	{
		return IntVector2();
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

	}

	void Graphics::RemoveGPUObject(GPUObject *object)
	{

	}

	void *Graphics::ReserveScratchBuffer(unsigned size)
	{
		return nullptr;
	}

	void Graphics::FreeScratchBuffer(void *buffer)
	{

	}

	void Graphics::CleanupShaderPrograms(ShaderVariation *variation)
	{

	}

	void Graphics::CleanupRenderSurface(RenderSurface *surface)
	{

	}

	ConstantBuffer *Graphics::GetOrCreateConstantBuffer(ShaderType type, unsigned index, unsigned size)
	{
		return nullptr;
	}

	void Graphics::MarkFBODirty()
	{

	}

	void Graphics::SetVBO(unsigned object)
	{

	}

	void Graphics::SetUBO(unsigned object)
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

	void Graphics::PrepareDraw()
	{

	}

	void Graphics::CreateResolveTexture()
	{

	}

	void Graphics::CleanupFrameBuffers()
	{

	}

	unsigned Graphics::CreateFrameBuffer()
	{
		return 0;
	}

	void Graphics::DeleteFramebuffer(unsigned fbo)
	{

	}

	void Graphics::BindFramebuffer(unsigned pos)
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
		Technique::RegisterObject(context);
	}
}