//
// Created by liuhao1 on 2017/12/29.
//

#include "Material.h"
#include "../Core/Thread.h"
#include "../Core/Context.h"
#include "../IO/FileSystem.h"
#include "../Math/Vector4.h"
#include "Graphics.h"
#include "../IO/Deserializer.h"
#include "../IO/VectorBuffer.h"
#include "../Resource/ResourceCache.h"
#include "../Container/Sort.h"
#include "../Core/CoreEvent.h"

namespace Urho3D
{
	extern const char* wrapModeNames[];

	static const char* textureUnitNames[] =
			{
				"diffuse",
				"normal",
				"specular",
				"emmisive",
				"environment",
#ifdef DESKTOP_GRAPHICS
				"volume",
				"custom1",
				"custom2",
				"lightramp",
				"lightshape",
				"shadowmap",
				"faceselect",
				"indirection",
				"depth",
				"light",
				"zone",
				nullptr
#else
				"lightramp",
				"lightshape",
				"shadowmap",
				nullptr
#endif
			};

	const char* cullModeNames[] =
			{
				"none",
				"ccw",
				"cw",
				nullptr
			};

	static const char* fillModeNames[] =
			{
				"solid",
				"wireframe",
				"point",
				nullptr
			};

	static TechniqueEntry noEntry;

	bool CompareTechniqueEntries(const TechniqueEntry& lhs, const TechniqueEntry& rhs)
	{
		if(lhs.lodDistance_ != rhs.lodDistance_)
			return lhs.lodDistance_ > rhs.lodDistance_;
		else
			return lhs.qualityLevel_ > rhs.qualityLevel_;
	}

	TechniqueEntry::TechniqueEntry() :
		qualityLevel_(0),
		lodDistance_(0.0f)
	{
	}

	TechniqueEntry::TechniqueEntry(Technique *tech, unsigned qualityLevel, float lodDistance) :
		technique_(tech),
		original_(tech),
		qualityLevel_(qualityLevel),
		lodDistance_(lodDistance)
	{
	}

	TechniqueEntry::~TechniqueEntry()
	{
	}

	ShaderParameterAnimationInfo::ShaderParameterAnimationInfo(Material *material, const String &name,
	                                                           ValueAnimation *attributeAnimation, WrapMode wrapMode,
	                                                           float speed) :
		ValueAnimationInfo(material, attributeAnimation, wrapMode, speed),
		name_(name)
	{
	}

	ShaderParameterAnimationInfo::ShaderParameterAnimationInfo(const ShaderParameterAnimationInfo &other) :
		ValueAnimationInfo(other),
		name_(other.name_)
	{
	}

	ShaderParameterAnimationInfo::~ShaderParameterAnimationInfo()
	{

	}

	void ShaderParameterAnimationInfo::ApplyValue(const Variant &newValue)
	{
		static_cast<Material*>(target_.Get())->SetShaderParameter(name_, newValue);
	}


	Material::Material(Context *context) :
			Resource(context),
			auxViewFrameNumber_(0),
			shaderParameterHash_(0),
			alphaToConverage_(false),
			lineAntiAlias_(false),
			occlusion_(false),
			specular_(false),
			subscribed_(false),
			batchedParameterUpdate_(false)
	{
		ResetToDefaults();
	}

	Material::~Material()
	{

	}

	void Material::RegisterObject(Context *context)
	{
		context->RegisterFactory<Material>();
	}

	bool Material::BeginLoad(Deserializer &source)
	{
		Graphics* graphics = GetSubsystem<Graphics>();
		if(!graphics)
			return true;

		String extension = GetExtension(source.GetName());

		bool success = false;
		if(extension == ".xml")
		{
			success = BeginLoadXML(source);
			if(success)
				return true;
		}
		else //Load JSON file
		{
		}
		// Loading failed
		ResetToDefaults();
		return false;
	}

	bool Material::EndLoad()
	{
		Graphics* graphics = GetSubsystem<Graphics>();
		if(!graphics)
			return true;

		bool success = false;
		if(loadXMLFile_)
		{
			// If async loading, get the techniques / textures which should be ready now
			// Todo, so the material MUST be loaded in async ??
			XMLElement rootElem = loadXMLFile_->GetRoot();
			success = Load(rootElem);
		}
		loadXMLFile_.Reset();
		return success;
	}

	bool Material::Save(Serializer &dest) const
	{
		SharedPtr<XMLFile> xml(new XMLFile(context_));
		XMLElement materialElem = xml->CreateRoot("material");

		Save(materialElem);
		return xml->Save(dest);
	}

	bool Material::Load(const XMLElement &source)
	{
		ResetToDefaults();

		if(source.IsNull())
		{
			URHO3D_LOGERROR("Can not load material from null XML element");
			return false;
		}

		ResourceCache* cache = GetSubsystem<ResourceCache>();

		XMLElement shaderElem = source.GetChild("shader");
		if(shaderElem)
		{
			vertexShaderDefines_ = shaderElem.GetAttribute("vsdefines");
			pixelShaderDefines_ = shaderElem.GetAttribute("psdefines");
		}

		XMLElement techniqueElem = source.GetChild("technique");
		techniques_.Clear();

		while(techniqueElem)
		{
			//Note, at this time, the Technique resource should be loaded and cached in ResourceCache
			Technique* tech = cache->GetResource<Technique>(techniqueElem.GetAttribute("name"));
			if(tech)
			{
				TechniqueEntry newTechnique;
				newTechnique.technique_ = newTechnique.original_ = tech;
				if(techniqueElem.HasAttribute("quality"))
					newTechnique.qualityLevel_ = techniqueElem.GetInt("quality");
				if(techniqueElem.HasAttribute("loddistance"))
					newTechnique.lodDistance_ = techniqueElem.GetFloat("loddistance");

				techniques_.Push(newTechnique);
			}

			techniqueElem = techniqueElem.GetNext("technique");
		}

		SortTechniques();
		ApplyShaderDefines();

		XMLElement textureElem = source.GetChild("texture");
		while (textureElem)
		{
			//todo
		}
		//todo

		XMLElement cullElem = source.GetChild("cull");
		if(cullElem)
			SetCullMode((CullMode)GetStringListIndex(cullElem.GetAttribute("value").CString(), cullModeNames, CULL_CCW));

		XMLElement shadowCullElem = source.GetChild("shadowcull");
		if(shadowCullElem)
			SetShadowCullMode((CullMode)GetStringListIndex(shadowCullElem.GetAttribute("value").CString(), cullModeNames, CULL_CCW));

		XMLElement fillElem = source.GetChild("fill");
		if(fillElem)
			SetFillMode((FillMode)GetStringListIndex(fillElem.GetAttribute("value").CString(), fillModeNames, FILL_SOLID));

		//todo
		RefreshShaderParameterHash();
		RefreshMemoryUse();
		return true;
	}

	bool Material::Save(XMLElement &dest) const
	{
		if(dest.IsNull())
		{
			URHO3D_LOGERROR("Can not save material to null XML element");
			return false;
		}
		// Write technique
		for(unsigned i=0; i< techniques_.Size(); ++i)
		{
			const TechniqueEntry& entry = techniques_[i];
			if(!entry.technique_)
				continue;
			XMLElement techniqueElem = dest.CreateChild("technique");
			techniqueElem.SetString("name", entry.technique_->GetName());
			techniqueElem.SetInt("quality", entry.qualityLevel_);
			techniqueElem.SetFloat("loddistance", entry.lodDistance_);
		}

		// Write texture units
		for(unsigned i = 0; i< MAX_TEXTURE_UNITS; ++i)
		{
			//todo
		}

		// Write shader compile defines
		if(!vertexShaderDefines_.Empty() || !pixelShaderDefines_.Empty())
		{
			XMLElement shaderElem = dest.CreateChild("shader");
			if(!vertexShaderDefines_.Empty())
				shaderElem.SetString("vsdefines", vertexShaderDefines_);
			if(!pixelShaderDefines_.Empty())
				shaderElem.SetString("psdefines", pixelShaderDefines_);
		}

		// Write shader parameters
		for(auto iter = shaderParameters_.Begin(); iter != shaderParameters_.End(); ++iter)
		{
			XMLElement parameterElem = dest.CreateChild("parameter");
			parameterElem.SetString("name", iter->second_.name_);
			if(iter->second_.value_.GetType() != VAR_BUFFER && iter->second_.value_.GetType() != VAR_INT &&
				iter->second_.value_.GetType() != VAR_BOOL)
			{
				parameterElem.SetVectorVariant("value", iter->second_.value_);
			}
			else
			{
				parameterElem.SetAttribute("type", iter->second_.value_.GetTypeName());
				parameterElem.SetAttribute("value", iter->second_.value_.ToString());
			}
		}

		// Write shader parameter animations
		//todo
	}

	void Material::SetNumTechniques(unsigned num)
	{
		if(!num)
			return;
		techniques_.Resize(num);
		RefreshMemoryUse();
	}

	void Material::SetTechnique(unsigned index, Technique *tech, unsigned int qualityLevel, float lodDistance)
	{
		if(index > techniques_.Size())
			return;
		techniques_[index] = TechniqueEntry(tech, qualityLevel, lodDistance);
		ApplyShaderDefines(index);
	}

	void Material::SetVertexShaderDefines(const String &defines)
	{
		if(defines != vertexShaderDefines_)
		{
			vertexShaderDefines_ = defines;
			ApplyShaderDefines();
		}
	}

	void Material::SetPixelShaderDefines(const String &defines)
	{
		if(defines != pixelShaderDefines_)
		{
			pixelShaderDefines_ = defines;
			ApplyShaderDefines();
		}
	}

	void Material::SetShaderParameter(const String &name, const Variant &value)
	{
		MaterialShaderParameter newParam;
		newParam.name_ = name;
		newParam.value_ = value;

		StringHash nameHash(name);
		shaderParameters_[nameHash] = newParam;

		if(nameHash == PSP_MATSPECCOLOR)
		{
			VariantType type = value.GetType();
			if(type == VAR_VECTOR3)
			{
				const Vector3& vec = value.GetVector3();
				specular_ = vec.x_ > 0.0f || vec.y_ > 0.0f || vec.z_ > 0.0f;
			}
			else if(type == VAR_VECTOR4)
			{
				const Vector4& vec = value.GetVector4();
				specular_ = vec.x_ > 0.0f || vec.y_ > 0.0f || vec.z_ > 0.0f;
			}
		}
		if(!batchedParameterUpdate_)
		{
			RefreshShaderParameterHash();
			RefreshMemoryUse();
		}
	}

	void Material::SetTexture(TextureUnit unit, Texture *texture)
	{
		if(unit < MAX_TEXTURE_UNITS)
		{
			if(texture)
				textures_[unit] = texture;
			else
				textures_.Erase(unit);
		}
	}

	//todo, usage
	void Material::SetUVTransform(const Vector2 &offset, float rotation, const Vector2 &repeat)
	{
		Matrix3x4 transform(Matrix3x4::IDENTITY);
		transform.m00_ = repeat.x_;
		transform.m11_ = repeat.y_;
		//todo
	}

	void Material::SetUVTransform(const Vector2 &offset, float rotation, float repeat)
	{
		SetUVTransform(offset, rotation, Vector2(repeat, repeat));
	}

	void Material::SetCullMode(CullMode mode)
	{
		cullMode_ = mode;
	}

	void Material::SetShadowCullMode(CullMode mode)
	{
		shadowCullMode_ = mode;
	}

	void Material::SetFillMode(FillMode mode)
	{
		fillMode_ = mode;
	}

	bool Material::BeginLoadXML(Deserializer &source)
	{
		ResetToDefaults();
		loadXMLFile_ = new XMLFile(context_);

		if(loadXMLFile_->Load(source))
		{
			// If async loading, scan the XML content beforehand for technique & texture resources
			// and request them to also be loaded. Can not do anything else at this point
			// Todo,
			if(GetAsyncLoadState() == ASYNC_LOADING)
			{
				ResourceCache* cache = GetSubsystem<ResourceCache>();
				XMLElement rootElem = loadXMLFile_->GetRoot();
				XMLElement techniqueElem = rootElem.GetChild("technique");
				while (techniqueElem)
				{
					//Note, do really load
					cache->BackgroundLoadResource<Technique>(techniqueElem.GetAttribute("name"), true, this);
					techniqueElem = techniqueElem.GetNext("technique");
				}

				XMLElement textureElem = rootElem.GetChild("texture");
				while (textureElem)
				{
					String name = textureElem.GetAttribute("name");
					// Detect cube maps and arrays of by file extension
					if(GetExtension(name) == ".xml")
					{
#ifdef DESKTOP_GRAPHICS
						StringHash type = ParseTextureTypeXml(cache, name);
						if(!type && textureElem.HasAttribute("unit"))
						{
							TextureUnit unit = ParseTextureUnitName(textureElem.GetAttribute("unit"));
							if(unit == TU_VOLUMEMAP)
								type = Texture3D::GetTypeStatic();
						}

						//todo
#endif
					}
				}

			}
			return true;
		}
		return false;
	}

	void Material::ResetToDefaults()
	{
		if(!Thread::IsMainThread())
			return;
		vertexShaderDefines_.Clear();
		pixelShaderDefines_.Clear();

		SetNumTechniques(1);
		Renderer* renderer = GetSubsystem<Renderer>();
		//todo

		batchedParameterUpdate_ = true;
		shaderParameters_.Clear();
		SetShaderParameter("UOffset", Vector4(1.0f, 0.0f, 0.0f, 0.0f));
		SetShaderParameter("VOffset", Vector4(0.0f, 1.0f, 0.0f, 0.0f));
		SetShaderParameter("MatDiffColor", Vector4::ONE);
		SetShaderParameter("MatEmissiveColor", Vector3::ZERO);
		SetShaderParameter("MatEnvMapColor", Vector3::ONE);
		SetShaderParameter("MatSpecColor", Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		SetShaderParameter("Roughness", 0.5f);
		SetShaderParameter("Metallic", 0.0f);
		batchedParameterUpdate_ = false;

		cullMode_ = CULL_CCW;
		shadowCullMode_ = CULL_CCW;
		fillMode_ = FILL_SOLID;
		//todo
		renderOrder_ = DEFAULT_RENDER_ORDER;
		occlusion_ = true;

		RefreshShaderParameterHash();
		RefreshMemoryUse();
	}

	void Material::RefreshShaderParameterHash()
	{
		VectorBuffer temp;
		for(auto i = shaderParameters_.Begin(); i != shaderParameters_.End(); ++i)
		{
			temp.WriteStringHash(i->first_);
			temp.WriteVariant(i->second_.value_);
		}

		shaderParameterHash_ = 0;
		const unsigned char* data = temp.GetData();
		unsigned dataSize = temp.GetSize();
		for(unsigned i=0; i<dataSize; ++i)
			shaderParameterHash_ = SDBMHash(shaderParameterHash_, data[i]);
	}

	void Material::RefreshMemoryUse()
	{
		unsigned memoryUse = sizeof(Material);
		memoryUse += techniques_.Size() * sizeof(Technique);
		memoryUse += MAX_TEXTURE_UNITS * sizeof(TextureUnit);
		memoryUse += shaderParameters_.Size() * sizeof(MaterialShaderParameter);

		SetMemoryUse(memoryUse);
	}

	void Material::ApplyShaderDefines(unsigned int index)
	{
		if(index == M_MAX_UNSIGNED)
		{
			for(unsigned i=0; i<techniques_.Size(); ++i)
			{
				ApplyShaderDefines(i);
			}
			return;
		}

		if(index >= techniques_.Size() || !techniques_[index].original_)
			return;

		if(vertexShaderDefines_.Empty() && pixelShaderDefines_.Empty())
			techniques_[index].technique_ = techniques_[index].original_;
		else
			techniques_[index].technique_ = techniques_[index].original_->CloneWithDefines(vertexShaderDefines_, pixelShaderDefines_);
	}

	void Material::UpdateEventSubscription()
	{
		//todo
	}

	void Material::HandleAttributeAnimationUpdate(StringHash eventType, VariantMap &eventData)
	{
		using namespace Update;
		float timeStep = eventData[P_TIMESTEP].GetFloat();

		// Keep weak pointer to self to check for destruction cased by event handling
		WeakPtr<Object> self(this);

		Vector<String> finishedNames;
		//todo
	}

	void Material::SetAlphaToCoverage(bool enable)
	{
		alphaToConverage_ = enable;
	}

	void Material::SetLineAntiAlias(bool enable)
	{
		lineAntiAlias_ = enable;
	}

	void Material::SetRenderOrder(unsigned char order)
	{
		renderOrder_ = order;
	}

	void Material::SetOcculusion(bool enable)
	{
		occlusion_ = enable;
	}

	void Material::RemoveShaderParameter(const String &name)
	{
		StringHash nameHash(name);
		shaderParameters_.Erase(nameHash);

		if(nameHash == PSP_MATSPECCOLOR)
			specular_ = false;

		RefreshShaderParameterHash();
		RefreshMemoryUse();
	}

	void Material::ReleaseShaders()
	{
		for(unsigned i=0; i<techniques_.Size(); ++i)
		{
			Technique* tech = techniques_[i].technique_;
			if(tech)
				tech->ReleaseShaders();
		}
	}

	SharedPtr<Material> Material::Clone(const String &cloneName) const
	{
		SharedPtr<Material> ret(new Material(context_));

		ret->SetName(cloneName);
		ret->techniques_ = techniques_;
		ret->vertexShaderDefines_ = vertexShaderDefines_;
		ret->pixelShaderDefines_ = pixelShaderDefines_;
		ret->shaderParameters_ = shaderParameters_;
		ret->shaderParameterHash_ = shaderParameterHash_;
		ret->textures_ = textures_;
		ret->depthBias_ = depthBias_;
		ret->alphaToConverage_ = alphaToConverage_;
		ret->lineAntiAlias_ = lineAntiAlias_;
		ret->occlusion_ = occlusion_;
		ret->specular_ = specular_;
		ret->cullMode_ = cullMode_;
		ret->shadowCullMode_ = shadowCullMode_;
		ret->fillMode_ = fillMode_;
		ret->renderOrder_ = renderOrder_;
		ret->RefreshMemoryUse();

		return ret;
	}

	void Material::SortTechniques()
	{
		Sort(techniques_.Begin(), techniques_.End(), CompareTechniqueEntries);
	}

	void Material::MarkForAuxView(unsigned frameNumber)
	{
		auxViewFrameNumber_ = frameNumber;
	}

	const TechniqueEntry &Material::GetTechniqueEntry(unsigned index) const
	{
		return index < techniques_.Size() ? techniques_[index] : noEntry;
	}

	Technique *Material::GetTechnique(unsigned index) const
	{
		return index < techniques_.Size() ? techniques_[index].technique_ : nullptr;
	}

	Pass *Material::GetPass(unsigned index, const String &passName) const
	{
		Technique* tech = index < techniques_.Size() ? techniques_[index].technique_ : nullptr;
		return tech ? tech->GetPass(passName) : nullptr;
	}

	Texture *Material::GetTexture(TextureUnit unit) const
	{
		auto iter = textures_.Find(unit);
		return iter != textures_.End() ? iter->second_.Get() : nullptr;
	}

	const Variant &Material::GetShaderParameter(const String &name) const
	{
		auto iter = shaderParameters_.Find(name);
		return iter != shaderParameters_.End() ?  iter->second_.value_ : Variant::EMPTY;
	}


}