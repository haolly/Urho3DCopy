//
// Created by liuhao on 2017/12/19.
//

#include "Animatable.h"
#include "../Resource/XMLElement.h"
#include "../Core/Context.h"
#include "SceneEvents.h"
#include "../Resource/ResourceCache.h"

namespace Urho3D
{

	extern const char* wrapModeNames[];

	AttributeAnimationInfo::AttributeAnimationInfo(Animatable *target, const AttributeInfo &attributeInfo,
	                                               ValueAnimation *attributeAnimation, WrapMode wrapMode, float speed):
		ValueAnimationInfo(target, attributeAnimation, wrapMode, speed),
		attributeInfo_(attributeInfo)
	{
	}

	AttributeAnimationInfo::AttributeAnimationInfo(const AttributeAnimationInfo &other):
		ValueAnimationInfo(other),
		attributeInfo_(other.attributeInfo_)
	{
	}

	AttributeAnimationInfo::~AttributeAnimationInfo()
	{
	}

	void AttributeAnimationInfo::ApplyValue(const Variant &newValue)
	{
		Animatable* animatable = static_cast<Animatable*>(target_.Get());
		if(animatable)
		{
			animatable->OnSetAttribute(attributeInfo_, newValue);
			animatable->ApplyAttributes();
		}
	}

	Animatable::Animatable(Context *context) :
			Serializable(context),
			animationEnabled_(true)
	{
	}

	Animatable::~Animatable()
	{
	}

	void Animatable::RegisterObject(Context *context)
	{
		URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Object Animation", GetObjectAnimationAttr, SetObjectAnimationAttr,
				ResourceRef, ResourceRef(ObjectAnimation::GetTypeStatic()), AM_DEFAULT);
	}

	bool Animatable::LoadXML(const XMLElement &source, bool setInstanceDefault)
	{
		if(!Serializable::LoadXML(source, setInstanceDefault))
			return false;

		SetObjectAnimation(nullptr);
		attributeAnimationInfos_.Clear();

		XMLElement elem = source.GetChild("objectanimation");
		if(elem)
		{
			SharedPtr<ObjectAnimation> objectAnimation(new ObjectAnimation(context_));
			if(!objectAnimation->LoadXML(elem))
				return false;
			SetObjectAnimation(objectAnimation);
		}

		elem = source.GetChild("attributeanimation");
		while (elem)
		{
			String name = elem.GetAttribute("name");
			SharedPtr<ValueAnimation> attributeAnimation(new ValueAnimation(context_));
			if(!attributeAnimation->LoadXML(elem))
				return false;
			String wrapModeString = source.GetAttribute("wrapmode");
			WrapMode wrapMode = static_cast<WrapMode>(GetStringListIndex(wrapModeString.CString(), wrapModeNames, WM_LOOP));

			float speed = elem.GetFloat("speed");
			SetAttributeAnimation(name, attributeAnimation, wrapMode, speed);

			elem = elem.GetNext("attributeanimation");
		}
		return true;
	}

	bool Animatable::SaveXML(XMLElement &dest) const
	{
		if(!Serializable::SaveXML(dest))
			return false;

		if(objectAnimation_ && objectAnimation_->GetName().Empty())
		{
			XMLElement elem = dest.CreateChild("objectanimation");
			if(!objectAnimation_->SaveXML(elem))
				return false;
		}

		for(auto iter = attributeAnimationInfos_.Begin(); iter != attributeAnimationInfos_.End(); ++iter)
		{
			ValueAnimation* attributeAnimation = iter->second_->GetAnimation();
			//todo, why continue ??
			if(attributeAnimation->GetOwner())
				continue;

			const AttributeInfo& attr = iter->second_->GetAttributeInfo();
			XMLElement elem = dest.CreateChild("attributeanimation");
			elem.SetAttribute("name", attr.name_);
			if(!attributeAnimation->SaveXML(elem))
				return false;
			elem.SetAttribute("wrapmode", wrapModeNames[iter->second_->GetWrapMode()]);
			elem.SetFloat("speed", iter->second_->GetSpeed());
		}
		return true;
	}

	void Animatable::SetAnimationEnabled(bool enable)
	{
		if(objectAnimation_)
		{
			// In object animation there may be targets in hierarchy. Set same enable/disable state in all
			HashSet<Animatable*> targets;
			auto& infos = objectAnimation_->GetAttributeAnimationInfos();
			for(auto iter = infos.Begin(); iter != infos.End(); ++iter)
			{
				String outName;
				Animatable* target = FindAttributeAnimationTarget(iter->first_, outName);
				if(target && target != this)
					targets.Insert(target);
			}
			for(auto iter = targets.Begin(); iter != targets.End(); ++iter)
			{
				(*iter)->animationEnabled_ = enable;
			}
		}
		animationEnabled_ = enable;
	}

	void Animatable::SetAnimationTime(float time)
	{
		if(objectAnimation_)
		{
			auto& infos = objectAnimation_->GetAttributeAnimationInfos();
			for(auto iter = infos.Begin(); iter != infos.End(); ++iter)
			{
				String outName;
				Animatable* target = FindAttributeAnimationTarget(iter->first_, outName);
				if(target)
					target->SetAttributeAnimationTime(outName, time);
			}
		}
		else
		{
			for(auto iter = attributeAnimationInfos_.Begin(); iter != attributeAnimationInfos_.End(); ++iter)
			{
				iter->second_->SetTime(time);
			}
		}
	}

	void Animatable::SetObjectAnimation(ObjectAnimation *objectAnimation)
	{
		if(objectAnimation == objectAnimation_)
			return;

		if(objectAnimation_)
		{
			OnObjectAnimationRemoved(objectAnimation_);
			UnsubscribeFromEvent(objectAnimation_, E_ATTRIBUTEANIMATIONADDED);
			UnsubscribeFromEvent(objectAnimation_, E_ATTRIBUTEANIMATIONREMOVED);
		}
		objectAnimation_ = objectAnimation;
		if(objectAnimation_)
		{
			OnObjectAnimationAdded(objectAnimation_);
			SubscribeToEvent(objectAnimation_, E_ATTRIBUTEANIMATIONADDED, URHO3D_HANDLER(Animatable, HandleAttributeAnimationAdded));
			SubscribeToEvent(objectAnimation_, E_ATTRIBUTEANIMATIONREMOVED, URHO3D_HANDLER(Animatable, HandleAttributeAnimationRemoved));
		}
	}

	void Animatable::SetAttributeAnimation(const String &name, ValueAnimation *attributeAnimation, WrapMode wrapMode,
	                                       float speed)
	{
		AttributeAnimationInfo* info = GetAttributeAnimationInfo(name);
		if(attributeAnimation)
		{
			if(info && attributeAnimation == info->GetAnimation())
			{
				info->SetWrapMode(wrapMode);
				info->SetSpeed(speed);
				return;
			}

			const AttributeInfo* attributeInfo = nullptr;
			if(info)
				attributeInfo = &info->GetAttributeInfo();
			else
			{
				const Vector<AttributeInfo>* attributes = GetAttributes();
				if(!attributes)
				{
					URHO3D_LOGERROR(GetTypeName() + " has not attributes");
					return;
				}

				for(auto iter = attributes->Begin(); iter != attributes->End(); ++iter)
				{
					if(name == (*iter).name_)
					{
						attributeInfo = &(*iter);
						break;
					}
				}
			}

			if(!attributeInfo)
			{
				URHO3D_LOGERROR("Invalid name: " + name);
				return;
			}

			if(attributeAnimation->GetValueType() != attributeInfo->type_)
			{
				URHO3D_LOGERROR("Invalid value type");
				return;
			}

			if(attributeInfo->mode_ & AM_NET)
			{
				animatedNetworkAttributes_.Insert(attributeInfo);
			}

			attributeAnimationInfos_[name] = new AttributeAnimationInfo(this, *attributeInfo, attributeAnimation, wrapMode, speed);

			if(!info)
				OnAttributeAnimationAdded();
		}
		else
		{
			if(!info)
				return;

			if(info->GetAttributeInfo().mode_ & AM_NET)
				animatedNetworkAttributes_.Erase(&info->GetAttributeInfo());

			attributeAnimationInfos_.Erase(name);
			OnAttributeAnimationRemoved();
		}
	}

	void Animatable::SetAttributeAnimationWrapMode(const String &name, WrapMode wrapMode)
	{
		AttributeAnimationInfo* info = GetAttributeAnimationInfo(name);
		if(info)
			info->SetWrapMode(wrapMode);
	}

	void Animatable::SetAttributeAnimationSpeed(const String &name, float speed)
	{
		auto info = GetAttributeAnimationInfo(name);
		if(info)
			info->SetSpeed(speed);
	}

	void Animatable::SetAttributeAnimationTime(const String &name, float time)
	{
		AttributeAnimationInfo* info = GetAttributeAnimationInfo(name);
		if(info)
			info->SetTime(time);
	}

	void Animatable::RemoveObjectAnimation()
	{
		SetObjectAnimation(nullptr);
	}

	void Animatable::RemoveAttributeAnimation(const String &name)
	{
		SetAttributeAnimation(name, nullptr);
	}

	ObjectAnimation *Animatable::GetObjectAnimation() const
	{
		return objectAnimation_;
	}

	ValueAnimation *Animatable::GetAttributeAnimation(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetAnimation() : nullptr;
	}

	WrapMode Animatable::GetAttributeAnimationWrapMode(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetWrapMode() : WM_LOOP;
	}

	float Animatable::GetAttributeAnimationSpeed(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetSpeed() : 1.0f;
	}

	float Animatable::GetAttributeAnimationTime(const String &name) const
	{
		auto info = GetAttributeAnimationInfo(name);
		return info ? info->GetTime() : 0.0f;
	}

	void Animatable::SetObjectAnimationAttr(const ResourceRef &value)
	{
		if(!value.name_.Empty())
		{
			ResourceCache* cache = GetSubsystem<ResourceCache>();
			SetObjectAnimation(cache->GetResource<ObjectAnimation>(value.name_));
		}
	}

	ResourceRef Animatable::GetObjectAnimationAttr() const
	{
		return GetResourceRef(objectAnimation_, ObjectAnimation::GetTypeStatic());
	}

	Animatable *Animatable::FindAttributeAnimationTarget(const String &name, String &outName)
	{
		// Base implementation only handles self
		outName = name;
		return this;
	}

	void
	Animatable::SetObjectAttributeAnimation(const String &name, ValueAnimation *attributeAnimation, WrapMode wrapMode,
	                                        float speed)
	{
		String outName;
		Animatable* target = FindAttributeAnimationTarget(name, outName);
		if(target)
		{
			target->SetAttributeAnimation(outName, attributeAnimation, wrapMode, speed);
		}
	}

	void Animatable::UpdateAttributeAnimations(float timeStep)
	{
		if(!animationEnabled_)
			return;
		WeakPtr<Animatable> self(this);

		Vector<String> finishedNames;
		for(auto iter = attributeAnimationInfos_.Begin(); iter != attributeAnimationInfos_.End(); ++iter)
		{
			bool finished = iter->second_->Update(timeStep);
			if(self.Expired())
				return;
			if(finished)
				finishedNames.Push(iter->second_->GetAttributeInfo().name_);
		}

		for(unsigned i=0; i< finishedNames.Size(); ++i)
		{
			SetAttributeAnimation(finishedNames[i], nullptr);
		}
	}

	bool Animatable::IsAnimatedNetworkAttribute(const AttributeInfo &attrInfo) const
	{
		return animatedNetworkAttributes_.Find(&attrInfo) != animatedNetworkAttributes_.End();
	}

	AttributeAnimationInfo *Animatable::GetAttributeAnimationInfo(const String &name) const
	{
		auto iter = attributeAnimationInfos_.Find(name);
		if(iter != attributeAnimationInfos_.End())
			return iter->second_;
		return nullptr;
	}

	void Animatable::HandleAttributeAnimationAdded(StringHash eventType, VariantMap &eventData)
	{
		//todo
	}

	void Animatable::HandleAttributeAnimationRemoved(StringHash eventType, VariantMap &eventData)
	{
		//todo
	}

	void Animatable::OnObjectAnimationAdded(ObjectAnimation *objectAnimation)
	{
		//todo
	}

	void Animatable::OnObjectAnimationRemoved(ObjectAnimation *objectAnimation)
	{
		//todo
	}

}