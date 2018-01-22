//
// Created by liuhao on 2017/12/19.
//

#ifndef URHO3DCOPY_ANIMATABLE_H
#define URHO3DCOPY_ANIMATABLE_H

#include "Serializable.h"
#include "ValueAnimationInfo.h"
#include "ObjectAnimation.h"
#include "../Container/HashSet.h"

namespace Urho3D
{
	class Animatable;

	class AttributeAnimationInfo : public ValueAnimationInfo
	{
	public:
		AttributeAnimationInfo(Animatable* aimatable, const AttributeInfo& attributeInfo, ValueAnimation* attributeAnimation,
								WrapMode wrapMode, float speed);
		AttributeAnimationInfo(const AttributeAnimationInfo& other);
		virtual ~AttributeAnimationInfo() override ;
		const AttributeInfo& GetAttributeInfo() const
		{
			return attributeInfo_;
		}
	protected:
		virtual void ApplyValue(const Variant& newValue) override ;
	private:
		const AttributeInfo& attributeInfo_;
	};

	class Animatable : public Serializable
	{
		URHO3D_OBJECT(Animatable, Serializable);
	public:
		Animatable(Context* context);
		virtual ~Animatable() override ;
		static void RegisterObject(Context* context);

		virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false) override ;
		virtual bool SaveXML(XMLElement& dest) const override ;

		void SetAnimationEnabled(bool enable);
		void SetAnimationTime(float time);

		void SetObjectAnimation(ObjectAnimation* objectAnimation);
		void SetAttributeAnimation(const String& name, ValueAnimation* attributeAnimation, WrapMode wrapMode = WM_LOOP,
		                           float speed = 1.0f);
		void SetAttributeAnimationWrapMode(const String& name, WrapMode wrapMode);
		void SetAttributeAnimationSpeed(const String& name, float speed);
		void SetAttributeAnimationTime(const String& name, float time);
		void RemoveObjectAnimation();
		void RemoveAttributeAnimation(const String& name);

		bool GetAnimationEnabled() const
		{
			return animationEnabled_;
		}

		ObjectAnimation* GetObjectAnimation() const;
		ValueAnimation* GetAttributeAnimation(const String& name) const;
		WrapMode GetAttributeAnimationWrapMode(const String& name) const;
		float GetAttributeAnimationSpeed(const String& name) const;
		float GetAttributeAnimationTime(const String& name) const;

		void SetObjectAnimationAttr(const ResourceRef& value);
		ResourceRef GetObjectAnimationAttr() const;

	protected:
		virtual void OnAttributeAnimationAdded() = 0;
		virtual void OnAttributeAnimationRemoved() = 0;
		virtual Animatable* FindAttributeAnimationTarget(const String& name, String& outName);
		void SetObjectAttributeAnimation(const String& name, ValueAnimation* attributeAnimation,
										WrapMode wrapMode, float speed);
		void OnObjectAnimationAdded(ObjectAnimation* objectAnimation);
		void OnObjectAnimationRemoved(ObjectAnimation* objectAnimation);
		void UpdateAttributeAnimations(float timeStep);
		bool IsAnimatedNetworkAttribute(const AttributeInfo& attrInfo) const;
		AttributeAnimationInfo* GetAttributeAnimationInfo(const String& name) const;
		void HandleAttributeAnimationAdded(StringHash eventType, VariantMap& eventData);
		void HandleAttributeAnimationRemoved(StringHash eventType, VariantMap& eventData);

		bool animationEnabled_;
		SharedPtr<ObjectAnimation> objectAnimation_;
		HashSet<const AttributeInfo*> animatedNetworkAttributes_;
		//todo, what is the responsibility of this and objectAnimation_ ?
		HashMap<String, SharedPtr<AttributeAnimationInfo>> attributeAnimationInfos_;
	};
}



#endif //URHO3DCOPY_ANIMATABLE_H
