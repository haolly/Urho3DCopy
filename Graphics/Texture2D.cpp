//
// Created by liuhao1 on 2018/1/18.
//

#include "Texture2D.h"
#include "../Core/Context.h"

namespace Urho3D
{

	Texture2D::Texture2D(Context *context) :
			Texture(context)
	{
#ifdef URHO3D_OPENGL
		target_ = GL_TEXTURE_2D;
#endif
	}

	Texture2D::~Texture2D()
	{
		Release();
	}

	void Texture2D::RegisterObject(Context *context)
	{
		context->RegisterFactory<Texture2D>();
	}

	bool Texture2D::BeginLoad(Deserializer &source)
	{
		//todo
	}

	bool Texture2D::EndLoad()
	{
		return Resource::EndLoad();
	}

	void Texture2D::OnDeviceLost()
	{
		GPUObject::OnDeviceLost();
	}

	void Texture2D::OnDeviceReset()
	{
		GPUObject::OnDeviceReset();
	}

	bool Texture2D::SetSize(int width, int height, unsigned format, TextureUsage usage, int multiSample, bool autoResolve)
	{
		return false;
	}

	bool Texture2D::SetData(unsigned level, int x, int y, int width, int height, const void *data)
	{
		return false;
	}

	bool Texture2D::SetData(Image *image, bool useAlpha)
	{
		return false;
	}

	bool Texture2D::GetData(unsigned level, void *dest) const
	{
		return false;
	}

	bool Texture2D::GetData(Image &image) const
	{
		return false;
	}

	SharedPtr<Image> Texture2D::GetImage() const
	{
		return SharedPtr<Image>();
	}

	bool Texture2D::Create()
	{
		return Texture::Create();
	}

	void Texture2D::HandleRenderSurfaceUpdate(StringHash eventType, VariantMap &eventData)
	{

	}
}
