//
// Created by liuhao on 2017/12/31.
//

#include "ObjectAnimation.h"
#include "../Core/Context.h"
#include "../Resource/XMLFile.h"
#include "SceneEvents.h"

namespace Urho3D
{
	const char *wrapModeNames[] =
			{
					"Loop",
					"Once",
					"Clap",
					nullptr
			};

	ObjectAnimation::ObjectAnimation(Context *context) :
		Resource(context)
	{
	}

	ObjectAnimation::~ObjectAnimation()
	{
	}

	void ObjectAnimation::RegisterObject(Context *context)
	{
		context->RegisterFactory<ObjectAnimation>();
	}

	bool ObjectAnimation::BeginLoad(Deserializer &source)
	{
		XMLFile xmlFile(context_);
		if(!xmlFile.Load(source))
			return false;
		return LoadXML(xmlFile.GetRoot());
	}

	bool ObjectAnimation::Save(Serializer &dest) const
	{
		XMLFile xmlFile(context_);
		XMLElement rootElem = xmlFile.CreateRoot("objectanimation");
		if(!SaveXML(rootElem))
			return false;
		return xmlFile.Save(dest);
	}

	bool ObjectAnimation::LoadXML(const XMLElement &source)
	{
		attributeAnimationInfos_.Clear();

		XMLElement animElem;
		animElem = source.GetChild("attributeanimation");
		while (animElem)
		{
			String name = animElem.GetAttribute("name");

			SharedPtr<ValueAnimation> animation(new ValueAnimation(context_));
			if(!animation->LoadXML(animElem))
				return false;

			String wrapModeString = animElem.GetAttribute("wrapmode");
			WrapMode wrapMode = WM_LOOP;
			for(int i=0; i<= WM_CLAMP; ++i)
			{
				if(wrapModeString == wrapModeNames[i])
				{
					wrapMode = (WrapMode)i;
					break;
				}
			}
			float speed = animElem.GetFloat("speed");
			AddAttributeAnimation(name, animation, wrapMode, speed);

			animElem = animElem.GetNext("attributeanimation");
		}
		return true;
	}

	bool ObjectAnimation::SaveXML(XMLElement &dest) const
	{
		for(auto iter = attributeAnimationInfos_.Begin(); iter != attributeAnimationInfos_.End(); ++iter)
		{
			XMLElement animElem = dest.CreateChild("attributeanimation");
			animElem.SetAttribute("name", iter->first_);

			const ValueAnimationInfo* info = iter->second_;
			if(!info->GetAnimation()->SaveXML(animElem))
				return false;
			animElem.SetAttribute("wrapmode", wrapModeNames[info->GetWrapMode()]);
			animElem.SetFloat("speed", info->GetSpeed());
		}
		return true;
	}

	void
	ObjectAnimation::AddAttributeAnimation(const String &name, ValueAnimation *attributeAnimation, WrapMode wrapMode,
	                                       float speed)
	{
		if(!attributeAnimation)
			return;

		attributeAnimation->SetOwner(this);
		attributeAnimationInfos_[name] = new ValueAnimationInfo(attributeAnimation, wrapMode, speed);
		SendAttributeAnimationAddedEvent(name);
	}

	void ObjectAnimation::RemoveAttributeAnimation(const String &name)
	{
		auto iter = attributeAnimationInfos_.Find(name);
		if(iter != attributeAnimationInfos_.End())
		{
			SendAttributeAnimationRemovedEvent(name);

			iter->second_->GetAnimation()->SetOwner(nullptr);
			attributeAnimationInfos_.Erase(iter);
		}
	}

	void ObjectAnimation::RemoveAttributeAnimation(ValueAnimation *attributeAnimation)
	{
		if(!attributeAnimation)
			return;

		for(auto iter = attributeAnimationInfos_.Begin(); iter != attributeAnimationInfos_.End(); ++iter)
		{
			if(iter->second_->GetAnimation() == attributeAnimation)
			{
				SendAttributeAnimationRemovedEvent(iter->first_);
				attributeAnimation->SetOwner(nullptr);
				attributeAnimationInfos_.Erase(iter);
				return;
			}
		}
	}

	ValueAnimation *ObjectAnimation::GetAttributeAnimation(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetAnimation() : nullptr;
	}

	WrapMode ObjectAnimation::GetAttributeAnimationWrapMode(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetWrapMode() : WM_LOOP;
	}

	float ObjectAnimation::GetAttributeAnimationSpeed(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetSpeed() : 1.0f;
	}

	ValueAnimationInfo *ObjectAnimation::GetAttributeAnimationInfo(const String &name) const
	{
		auto iter = attributeAnimationInfos_.Find(name);
		return iter != attributeAnimationInfos_.End() ? iter->second_ : nullptr;
	}

	void ObjectAnimation::SendAttributeAnimationAddedEvent(const String &name)
	{
		using namespace AttributeAnimationAdded;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_OBJECTANIMATION] = this;
		eventData[P_ATTRIBUTEANIMATIONNAME] = name;
		SendEvent(E_ATTRIBUTEANIMATIONADDED, eventData);
	}

	void ObjectAnimation::SendAttributeAnimationRemovedEvent(const String &name)
	{
		using namespace AttributeAnimationRemoved;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_OBJECTANIMATION] = this;
		eventData[P_ATTRIBUTEANIMATIONNAME] = name;
		SendEvent(E_ATTRIBUTEANIMATIONREMOVED, eventData);
	}
}