//
// Created by liuhao1 on 2018/1/18.
//

#include "Texture2D.h"
#include "../Core/Context.h"
#include "Graphics.h"
#include "../Resource/Image.h"
#include "../Resource/ResourceCache.h"
#include "GraphicsEvents.h"

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
		if(!graphics_)
			return true;
		// If device is lost, retry later
		if(graphics_->IsDeviceLost())
		{
			URHO3D_LOGWARNING("Texture load while device is lost");
			dataPending_ = true;
			return true;
		}

		loadImage_ = new Image(context_);
		if(!loadImage_->Load(source))
		{
			loadImage_.Reset();
			return false;
		}

		// Precalculate mip levels if async loading
		if(GetAsyncLoadState() == ASYNC_LOADING)
			loadImage_->PrecalculateLevels();

		// Load the optional parameters file
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		String xmlName = ReplaceExtension(GetName(), ".xml");
		loadParameters_ = cache->GetTempResource<XMLFile>(xmlName, false);
		return true;
	}

	bool Texture2D::EndLoad()
	{
		if(!graphics_ || graphics_->IsDeviceLost())
			return true;

		CheckTextureBudget(GetTypeStatic());
		SetParameters(loadParameters_);
		bool success = SetData(loadImage_);

		loadImage_.Reset();
		loadParameters_.Reset();
		return success;
	}

	bool Texture2D::SetSize(int width, int height, unsigned format, TextureUsage usage, int multiSample, bool autoResolve)
	{
		if(width <= 0 || height <= 0)
		{
			URHO3D_LOGERROR("Zero or negative texture dimensions");
			return false;
		}

		multiSample = Clamp(multiSample, 1, 16);
		if(multiSample == 1)
			autoResolve = false;
		else if(multiSample > 1 && usage < TEXTURE_RENDERTARGET)
		{
			URHO3D_LOGERROR("Multisampling is only supported for rendertarget or depth-stencil textures");
			return false;
		}

		// Disable mipmaps if multisample & custom resolve
		if(multiSample > 1 && autoResolve == false)
			requestedLevels_ = 1;

		// Delete the old surface if any
		renderSurface_.Reset();
		usage_ = usage;
		if(usage >= TEXTURE_RENDERTARGET)
		{
			renderSurface_ = new RenderSurface(this);

			// Clamp mode addresssing by default and nearest filtering
			addressMode_[COORD_U] = ADDRESS_CLAMP;
			addressMode_[COORD_V] = ADDRESS_CLAMP;
			filterMode_ = FILTER_NEAREST;
		}

		if(usage == TEXTURE_RENDERTARGET)
			SubscribeToEvent(E_RENDERSURFACEUPDATE, URHO3D_HANDLER(Texture2D, HandleRenderSurfaceUpdate));
		else
			UnsubscribeFromEvent(E_RENDERSURFACEUPDATE);

		width_ = width;
		height_ = height;
		format_ = format;
		depth_ = 1;
		multiSample_ = multiSample;
		autoResolve_ = autoResolve;
		return Create();
	}

	bool Texture2D::GetImage(Image &image) const
	{
		if(format_ != Graphics::GetRGBAFormat() && format_ != Graphics::GetRGBFormat())
		{
			URHO3D_LOGERROR("Unsupported texture format, can not convert to Image");
			return false;
		}
		image.SetSize(width_, height_, GetComponents());
		GetData(0, image.GetData());
		return true;
	}

	SharedPtr<Image> Texture2D::GetImage() const
	{
		auto rawImage = MakeShared<Image>(context_);
		if(!GetImage(*rawImage))
			return nullptr;
		return rawImage;
	}

	void Texture2D::HandleRenderSurfaceUpdate(StringHash eventType, VariantMap &eventData)
	{
		//todo
	}
}
