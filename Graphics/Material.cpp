//
// Created by liuhao1 on 2017/12/29.
//

#include "Material.h"
#include "../Core/Thread.h"
#include "../Core/Context.h"
#include "../IO/FileSystem.h"

namespace Urho3D
{

	Material::Material(Context *context) :
			Resource(context),
			//todo
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
		//todo

		String extension = GetExtension(source.GetName());

		bool success = false;
		if(extension == ".xml")
		{
			success = BeginLoadXML(source);
			//todo
			if(success)
				return true;
		}
		else
		{
		}
		ResetToDefaults();
		return false;
	}

	bool Material::EndLoad()
	{
		//todo
		bool success = false;
		if(loadXMLFile_)
		{
			XMLElement rootElem = loadXMLFile_->GetRoot();
			success = Load(rootElem);
		}
		loadXMLFile_.Reset();
		return success;
	}

	bool Material::Save(Serializer &dest) const
	{
		return Resource::Save(dest);
	}

	bool Material::Load(const XMLElement &source)
	{
		return false;
	}

	bool Material::Save(XMLElement &dest) const
	{
		return false;
	}

	void Material::SetNumTechniques(unsigned num)
	{

	}

	void Material::SetTechnique(unsigned index, Technique *tech, unsigned int qualityLevel, float lodDistance)
	{

	}

	void Material::SetPixelShaderDefines(const String &defines)
	{

	}

	void Material::SetShaderParameter(const String &name, const Variant &value)
	{

	}

	void Material::SetTexture(TextureUnit unit, Texture *texture)
	{

	}

	void Material::SetUVTransform(const Vector2 &offset, float rotation, const Vector2 &repeat)
	{

	}

	void Material::SetUVTransform(const Vector2 &offset, float rotation, float repeat)
	{

	}

	void Material::SetCullMode(CullMode mode)
	{

	}

	void Material::SetShadowCullMode(CullMode mode)
	{

	}

	void Material::SetFillMode(FillMode mode)
	{

	}

	bool Material::BeginLoadXML(Deserializer &source)
	{
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

		SetShaderParameter("UOffset", Vector4(1.0f, 0.0f, 0.0f, 0.0f));
		SetShaderParameter("VOffset", Vector4(0.0f, 1.0f, 0.0f, 0.0f));
		SetShaderParameter("MatDiffColor", Vector4::ONE);
		SetShaderParameter("MatEmissiveColor", Vector3::ZERO);
		SetShaderParameter("MatEnvMapColor", Vector3::ONE);
		SetShaderParameter("MatSpecColor", Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		SetShaderParameter("Roughness", 0.5f);
		SetShaderParameter("Metallic", 0.0f);
		//todo
	}

	void Material::RefreshShaderParameterHash()
	{

	}

	void Material::RefreshMemroyUse()
	{

	}

	void Material::ApplyShaderDefines(unsigned int index)
	{

	}

	void Material::UpdateEventSubscription()
	{

	}

	void Material::HandleAttributeAnimationUpdate(StringHash eventType, VariantMap &eventData)
	{

	}
}