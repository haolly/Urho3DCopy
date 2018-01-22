//
// Created by liuhao on 2017/12/31.
//

#ifndef URHO3DCOPY_OBJECTANIMATION_H
#define URHO3DCOPY_OBJECTANIMATION_H

#include "../Resource/Resource.h"
#include "AnimationDefs.h"
#include "ValueAnimation.h"
#include "ValueAnimationInfo.h"

namespace Urho3D
{
	class ObjectAnimation : public Resource
	{
		URHO3D_OBJECT(ObjectAnimation, Resource);
	public:
		ObjectAnimation(Context* context);
		virtual ~ObjectAnimation() override ;
		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool Save(Serializer& dest) const override ;
		bool LoadXML(const XMLElement& source);
		bool SaveXML(XMLElement& dest) const;

		void AddAttributeAnimation(const String& name, ValueAnimation* attributeAnimation, WrapMode wrapMode= WM_LOOP, float speed = 1.0f);
		void RemoveAttributeAnimation(const String& name);
		void RemoveAttributeAnimation(ValueAnimation* attributeAnimation);

		ValueAnimation* GetAttributeAnimation(const String& name) const;
		WrapMode GetAttributeAnimationWrapMode(const String& name) const;
		float GetAttributeAnimationSpeed(const String& name) const;

		const HashMap<String, SharedPtr<ValueAnimationInfo>>& GetAttributeAnimationInfos() const
		{
			return attributeAnimationInfos_;
		};

		ValueAnimationInfo* GetAttributeAnimationInfo(const String& name) const;

	private:
		void SendAttributeAnimationAddedEvent(const String& name);
		void SendAttributeAnimationRemovedEvent(const String& name);

		HashMap<String, SharedPtr<ValueAnimationInfo>> attributeAnimationInfos_;
	};
}



#endif //URHO3DCOPY_OBJECTANIMATION_H
