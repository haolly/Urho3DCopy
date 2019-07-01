//
// Created by liuhao1 on 2017/12/29.
//

#ifndef URHO3DCOPY_MATERIAL_H
#define URHO3DCOPY_MATERIAL_H

#include "../Resource/Resource.h"
#include "GraphicsDefs.h"
#include "../Resource/XMLFile.h"
#include "Technique.h"
#include "Texture.h"
#include "../Scene/ValueAnimationInfo.h"

namespace Urho3D
{
	class Material;

	struct MaterialShaderParameter
	{
		String name_;
		Variant value_;
	};

	struct TechniqueEntry
	{
		TechniqueEntry();
		TechniqueEntry(Technique* tech, unsigned qualityLevel, float lodDistance);
		~TechniqueEntry();

		SharedPtr<Technique> technique_;
		// Original technique, in case the material adds shader compilation defines.
		SharedPtr<Technique> original_;

		int qualityLevel_;
		float lodDistance_;
	};

	class ShaderParameterAnimationInfo : public ValueAnimationInfo
	{
	public:
		ShaderParameterAnimationInfo(Material* material, const String& name, ValueAnimation* attributeAnimation, WrapMode wrapMode,
									float speed);

		ShaderParameterAnimationInfo(const ShaderParameterAnimationInfo& other);

		virtual ~ShaderParameterAnimationInfo() override ;

		const String& GetName() const
		{
			return name_;
		}

	protected:
		virtual void ApplyValue(const Variant& newValue) override ;

	private:
		String name_;
	};


	class Material : public Resource
	{
		URHO3D_OBJECT(Material, Resource);
	public:
		Material(Context* context);
		virtual ~Material() override ;
		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool EndLoad() override ;
		virtual bool Save(Serializer& dest) const override ;

		bool Load(const XMLElement& source);
		bool Save(XMLElement& dest) const;

		void SetNumTechniques(unsigned num);
		void SetTechnique(unsigned index, Technique* tech, unsigned qualityLevel = 0, float lodDistance = 0.0f);
		void SetVertexShaderDefines(const String& defines);
		void SetPixelShaderDefines(const String& defines);
		void SetShaderParameter(const String& name, const Variant& value);

		void SetShaderParameterAnimation(const String& name, ValueAnimation* animation, WrapMode wrapMode = WM_LOOP, float speed = 1.0f);
		//todo

		void SetTexture(TextureUnit unit, Texture* texture);
		void SetUVTransform(const Vector2& offset, float rotation, const Vector2& repeat);
		void SetUVTransform(const Vector2& offset, float rotation, float repeat);
		void SetCullMode(CullMode mode);
		void SetShadowCullMode(CullMode mode);
		void SetFillMode(FillMode mode);
		//todo

		void SetAlphaToCoverage(bool enable);
		void SetLineAntiAlias(bool enable);
		void SetRenderOrder(unsigned char order);
		void SetOcculusion(bool enable);
		//todo

		void RemoveShaderParameter(const String& name);
		void ReleaseShaders();
		SharedPtr<Material> Clone(const String& cloneName = String::EMPTY) const;
		void SortTechniques();
		void MarkForAuxView(unsigned frameNumber);

		unsigned GetNumTechniques() const
		{
			return techniques_.Size();
		}

		const Vector<TechniqueEntry>& GetTechniques() const { return techniques_; }

		const TechniqueEntry& GetTechniqueEntry(unsigned index) const;
		Technique* GetTechnique(unsigned index) const;
		Pass* GetPass(unsigned index, const String& passName) const;
		Texture* GetTexture(TextureUnit unit) const;

		const HashMap<TextureUnit, SharedPtr<Texture> >& GetTextures() const { return textures_; };

		const String& GetVertexShaderDefines() const { return vertexShaderDefines_; }
		const String& GetPixelShaderDefines() const { return pixelShaderDefines_; }

		const Variant& GetShaderParameter(const String& name) const;
		//todo
	private:
		bool BeginLoadXML(Deserializer& source);
		void ResetToDefaults();
		void RefreshShaderParameterHash();
		void RefreshMemoryUse();
		void ApplyShaderDefines(unsigned index = M_MAX_UNSIGNED);
		void UpdateEventSubscription();
		void HandleAttributeAnimationUpdate(StringHash eventType, VariantMap& eventData);

		Vector<TechniqueEntry> techniques_;
		HashMap<TextureUnit, SharedPtr<Texture>> textures_;
		HashMap<StringHash, MaterialShaderParameter> shaderParameters_;

		//todo
		String vertexShaderDefines_;
		String pixelShaderDefines_;
		CullMode cullMode_;
		CullMode shadowCullMode_;
		FillMode fillMode_;
		//todo what is this ?
		BiasParameters depthBias_;
		unsigned char renderOrder_;
		//todo, usage
		// Last auxiliary view rendered frame number
		unsigned auxViewFrameNumber_;
		unsigned shaderParameterHash_;
		bool alphaToConverage_;
		bool lineAntiAlias_;
		bool occlusion_;
		bool specular_;
		bool subscribed_;
		bool batchedParameterUpdate_;
		SharedPtr<XMLFile> loadXMLFile_;
		SharedPtr<Scene> scene_;
	};

}


#endif //URHO3DCOPY_MATERIAL_H
