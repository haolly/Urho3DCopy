//
// Created by liuhao1 on 2018/1/10.
//

#include "Texture3D.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../Core/Context.h"

namespace Urho3D
{

	Texture3D::Texture3D(Context *context) :
			Texture(context)
	{
#ifdef URHO3D_OPENGL
#ifndef GL_ES_VERSION_2_0
		target_ = GL_TEXTURE_3D;
#else
		target = 0;
#endif
#endif
	}

	Texture3D::~Texture3D()
	{
		Release();
	}

	void Texture3D::RegisterObject(Context *context)
	{
		context->RegisterFactory<Texture3D>();
	}

	bool Texture3D::BeginLoad(Deserializer &source)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		if(!graphics_)
			return true;

		if(graphics_->IsDeviceLost())
		{
			URHO3D_LOGWARNING("Texture load while device is lost");
			dataPending_ = true;
			return true;
		}

		String texPath, texName, texExt;
		SplitPath(GetName(), texPath, texName, texExt);

		cache->ResetDependencies(this);

		loadParameters_ = new XMLFile(context_);
		if(loadParameters_->Load(source))
		{
			loadParameters_.Reset();
			return false;
		}

		XMLElement textureElem = loadParameters_->GetRoot();
		XMLElement volumeElem = textureElem.GetChild("volume");
		XMLElement colorlutElem = textureElem.GetChild("colorlut");

		if(volumeElem)
		{
			String name = volumeElem.GetAttribute("name");

			String volumeTexPath, volumeTexName, volumeTexExt;
			SplitPath(name, volumeTexPath, volumeTexName, volumeTexExt);

			if(volumeTexPath.Empty())
				name = texPath + name;

			loadImage_ = cache->GetTempResource<Image>(name);
			// Precalculate mip levels if async loading
			if(loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
				loadImage_->PrecalculateLevels();
			cache->StoreResourceDependency(this, name);
			return true;
		}
		else if(colorlutElem)
		{
			String name = colorlutElem.GetAttribute("name");

			String colorlutTexPath, colorlutTexName, colorlutTexExt;
			SplitPath(name, colorlutTexPath, colorlutTexName, colorlutTexExt);
			if(colorlutTexPath.Empty())
				name = texPath + name;

			SharedPtr<File> file = GetSubsystem<ResourceCache>()->GetFile(name);
			loadImage_ = new Image(context_);
			if(!loadImage_->LoadColorLUT(*(file.Get())))
			{
				loadParameters_.Reset();
				loadImage_.Reset();
				return false;
			}

			if(loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
				loadImage_->PrecalculateLevels();
			cache->StoreResourceDependency(this, name);
			return true;
		}
		URHO3D_LOGERROR("Texture3D XML data for " + GetName() + " did not contain either volume or colorlut element");
		return false;
	}

	bool Texture3D::EndLoad()
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


	bool Texture3D::SetSize(int width, int height, int depth, unsigned format, TextureUsage usage)
	{
		if(width <= 0 || height <= 0 || depth <= 0)
		{
			URHO3D_LOGERROR("Zero or negative 3D texture dimensions");
			return false;
		}
		if(usage >= TEXTURE_RENDERTARGET)
		{
			URHO3D_LOGERROR("Rendertarget or depth-stencil usage not supported for 3D textures");
			return false;
		}

		usage_ = usage;
		width_ = width;
		height_ = height;
		depth_ = depth;
		format_ = format;
		return Create();
	}

}