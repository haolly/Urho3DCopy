//
// Created by liuhao1 on 2017/12/29.
//

#ifndef URHO3DCOPY_MATERIAL_H
#define URHO3DCOPY_MATERIAL_H

#include "../Resource/Resource.h"
#include "GraphicsDefs.h"
#include "../Resource/XMLFile.h"

namespace Urho3D
{
	struct MaterialShaderParameter
	{
		String name_;
		Variant value_;
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
		void SetPixelShaderDefines(const String& defines);
		void SetShaderParameter(const String& name, const Variant& value);

		void SetTexture(TextureUnit unit, Texture* texture);
		void SetUVTransform(const Vector2& offset, float rotation, const Vector2& repeat);
		void SetUVTransform(const Vector2& offset, float rotation, float repeat);
		void SetCullMode(CullMode mode);
		void SetShadowCullMode(CullMode mode);
		void SetFillMode(FillMode mode);
		//todo

	private:
		bool BeginLoadXML(Deserializer& source);
		void ResetToDefaults();
		void RefreshShaderParameterHash();
		void RefreshMemroyUse();
		void ApplyShaderDefines(unsigned index = M_MAX_UNSIGNED);
		void UpdateEventSubscription();
		void HandleAttributeAnimationUpdate(StringHash eventType, VariantMap& eventData);

		//todo
		String vertexShaderDefines_;
		String pixelShaderDefines_;
		CullMode cullMode_;
		CullMode shadowCullMode_;
		//todo

		unsigned char renderOrder_;
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
