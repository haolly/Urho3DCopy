//
// Created by liuhao1 on 2018/1/5.
//

#include "Texture.h"
#include "../Core/StringUtils.h"
#include "../Resource/ResourceCache.h"
#include "Material.h"

namespace Urho3D
{

	static const char* addressModeNames[] =
			{
				"wrap",
				"mirror",
				"clamp",
				"border",
				nullptr
			};

	static const char* filterModeNames[] =
			{
				"nearest",
				"bilinear",
				"trilinear",
				"anisotropic",
				"nearestanisotropic",
				"default",
				nullptr
			};


	Texture::Texture(Context *context) :
			ResourceWithMetadata(context),
			GPUObject(GetSubsystem<Graphics>()),
			shaderResourceView_(nullptr),
			sampler_(nullptr),
			resolveTexture_(nullptr),
			format_(0),
			usage_(TEXTURE_STATIC),
			levels_(0),
			requestedLevels_(0),
			width_(0),
			height_(0),
			depth_(0),
			shadowCompare_(false),
			filterMode_(FILTER_DEFAULT),
			anisotropy_(0),
			multiSample_(1),
			sRGB_(false),
			parametersDirty_(true),
			autoResolve_(false),
			levelsDirty_(false)
	{
		for(int i=0; i<MAX_COORDS; ++i)
		{
			addressMode_[i] = ADDRESS_WRAP;
		}
		for(int i=0; i<MAX_TEXTURE_QUALITY_LEVELS; ++i)
		{
			mipsToSkip_[i] = (unsigned)(MAX_TEXTURE_QUALITY_LEVELS - 1 - i);
		}
	}

	Texture::~Texture()
	{
	}

	void Texture::SetNumLevels(unsigned levels)
	{
		//todo, why check this?
		if(usage_ > TEXTURE_RENDERTARGET)
			requestedLevels_ = 1;
		else
			requestedLevels_ = levels;
	}

	void Texture::SetFilterMode(TextureFilterMode mode)
	{
		filterMode_ = mode;
		parametersDirty_ = true;
	}

	// todo, what's the point?
	void Texture::SetAddressMode(TextureCoordinate coord, TextureAddressMode address)
	{
		addressMode_[coord] = address;
		parametersDirty_ = true;
	}

	void Texture::SetAnisotropy(unsigned level)
	{
		anisotropy_ = level;
		parametersDirty_ = true;
	}

	void Texture::SetShadowCompare(bool enable)
	{
		shadowCompare_ = enable;
		parametersDirty_ = true;
	}

	void Texture::SetBorderColor(const Color &color)
	{
		borderColor_ = color;
		parametersDirty_ = true;
	}


	void Texture::SetBackupTexture(Texture *texture)
	{
		backupTexture_ = texture;
	}

	void Texture::SetMipsToSkip(int quality, int toSkip)
	{
		if(quality >= QUALITY_LOW && quality < MAX_TEXTURE_QUALITY_LEVELS)
		{
			mipsToSkip_[quality] = (unsigned)toSkip;

			//Note, make sure a higher quality levels does not actually skip more mips
			for(int i=1; i< MAX_TEXTURE_QUALITY_LEVELS; ++i)
			{
				if(mipsToSkip_[i] > mipsToSkip_[i -1])
					mipsToSkip_[i] = mipsToSkip_[i -1];
			}
		}
	}


	int Texture::GetMipsToSkip(int quality) const
	{
		return quality >= QUALITY_LOW && quality < MAX_TEXTURE_QUALITY_LEVELS ? mipsToSkip_[quality] : 0;
	}

	int Texture::GetLevelWidth(unsigned level) const
	{
		if(level > levels_)
			return 0;
		//Note, each mipmap is a square root in width and height
		return Max(width_ >> level, 1);
	}

	int Texture::GetLevelHeight(unsigned level) const
	{
		if(level > levels_)
			return 0;
		//Note, each mipmap is a square root in width and height
		return Max(height_ >> level, 1);
	}

	int Texture::GetLevelDepth(unsigned level) const
	{
		if(level > levels_)
			return 0;
		return Max(depth_ >> level, 1);
	}

	unsigned Texture::GetDataSize(int width, int height) const
	{
		if(IsCompressed())
			return GetRowDataSize(width) * ((height + 3) >> 2);
		else
			return GetRowDataSize(width) * height;
	}

	unsigned Texture::GetDataSize(int width, int height, int depth) const
	{
		return depth * GetDataSize(width, height);
	}

	unsigned Texture::GetComponents() const
	{
		if(!width_ || IsCompressed())
			return 0;
		else
			return GetRowDataSize(width_) / width_;
	}


	void Texture::SetParameters(XMLFile *xml)
	{
		if(!xml)
			return;
		XMLElement rootElem = xml->GetRoot();
		SetParameters(rootElem);
	}

	void Texture::SetParameters(const XMLElement &element)
	{
		LoadMetadataFromXML(element);
		for(XMLElement paramElem = element.GetChild(); paramElem; paramElem.GetNext())
		{
			String name = paramElem.GetName();
			if(name == "address")
			{
				String coord = paramElem.GetAttributeLower("coord");
				if(coord.Length() >= 1)
				{
					//todo, WTF ??
					TextureCoordinate  coordIndex = (TextureCoordinate)(coord[0] - 'u');
					String mode = paramElem.GetAttributeLower("mode");
					SetAddressMode(coordIndex, (TextureAddressMode)GetStringListIndex(mode.CString(), addressModeNames, ADDRESS_WRAP));
				}
			}

			if(name == "border")
				SetBorderColor(paramElem.GetColor("color"));

			if(name == "filter")
			{
				String mode = paramElem.GetAttributeLower("mode");
				SetFilterMode((TextureFilterMode)GetStringListIndex(mode.CString(), filterModeNames, FILTER_DEFAULT));
				if(paramElem.HasAttribute("anisotropy"))
					SetAnisotropy(paramElem.GetUInt("anisotropy"));
			}

			if(name == "mipmap")
				SetNumLevels(paramElem.GetBool("enable") ? 0 : 1);

			if(name == "quality")
			{
				if(paramElem.HasAttribute("low"))
					SetMipsToSkip(QUALITY_LOW, paramElem.GetInt("low"));
				if(paramElem.HasAttribute("med"))
					SetMipsToSkip(QUALITY_MEDIUM, paramElem.GetInt("med"));
				if(paramElem.HasAttribute("medius"))
					SetMipsToSkip(QUALITY_MEDIUM, paramElem.GetInt("medium"));
				if(paramElem.HasAttribute("high"))
					SetMipsToSkip(QUALITY_HIGH, paramElem.GetInt("high"));
			}

			if(name == "srgb")
				SetSRGB(paramElem.GetBool("enable"));
		}
	}

	void Texture::SetParametersDirty()
	{
		parametersDirty_ = true;
	}


	void Texture::SetLevelsDirty()
	{
		if(usage_ == TEXTURE_RENDERTARGET && levels_ > 1)
			levelsDirty_ = true;
	}

	unsigned Texture::CheckMaxLevels(int width, int height, unsigned requestLevels)
	{
		unsigned maxLevels = 1;
		while(width > 1 || height > 1)
		{
			++maxLevels;
			width = width > 1 ? (width >> 1) : 1;
			height = height > 1 ? (height >> 1) : 1;
		}

		if(!requestLevels || maxLevels < requestLevels)
			return maxLevels;
		else
			return requestLevels;
	}

	unsigned Texture::CheckMaxLevels(int width, int height, int depth, unsigned requestedLevels)
	{
		unsigned maxLevels = 1;
		while (width > 1 || height > 1 || depth > 1)
		{
			++maxLevels;
			width = width > 1 ? (width >> 1) : 1;
			height = height > 1 ? (height >> 1) : 1;
			depth = depth > 1 ? (depth >> 1) : 1;
		}

		if(!requestedLevels || maxLevels < requestedLevels)
			return maxLevels;
		else
			return requestedLevels;
	}


	void Texture::CheckTextureBudget(StringHash type)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		unsigned long long textureBudget = cache->GetMemoryBudget(type);
		unsigned long long textureUse = cache->GetMemoryUse(type);
		if(!textureBudget)
			return;

		// if textures are over budget, they likely can not be freed directly as materials still refer to them.
		// Therefore free unused material first
		if(textureUse > textureBudget)
			cache->ReleaseResources(Material::GetTypeStatic());
	}

	void Texture::UpdateParameters()
	{
		//todo

	}
}