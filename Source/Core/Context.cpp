//
// Created by liuhao on 2017/11/27.
//

#include <SDL/include/SDL.h>
#include "Context.h"
#include "Thread.h"
#include "../IO/Log.h"

namespace Urho3D
{
#ifndef MINI_URHO
	static int sdlInitCounter = 0;
	static int ikInitCounter = 0;

	static void HandleIKLog(const char* msg)
	{
		URHO3D_LOGINFOF("[IK]%s", msg);
	}
#endif

	void EventReceiverGroup::BeginSendEvent()
	{
		++inSend_;
	}

	void EventReceiverGroup::EndSendEvent()
	{
		assert(inSend_ > 0);
		--inSend_;

		if(inSend_ == 0 && dirty_)
		{
			//Note, the second condition could NOT be i >=0, because unsigned i always >= 0
			for (unsigned i = receivers_.Size() -1; i < receivers_.Size(); --i) {
				if(!receivers_[i])
					receivers_.Erase(i);
			}
			dirty_ = false;
		}
	}

	void EventReceiverGroup::Add(Object *object)
	{
		if(object)
			receivers_.Push(object);
	}

	void EventReceiverGroup::Remove(Object *object)
	{
		if(inSend_ > 0)
		{
			auto it = receivers_.Find(object);
			if(it != receivers_.End())
			{
				(*it) = nullptr;
				dirty_ = true;
			}
		}
		else
		{
			receivers_.Remove(object);
		}
	}

	void RemoveNamedAttribute(HashMap<StringHash, Vector<AttributeInfo> >& attributes, StringHash objectType, const char* name)
	{
		auto iter = attributes.Find(objectType);
		if(iter == attributes.End())
			return;

		auto infos = iter->second_;
		for(auto iter_info = infos.Begin(); iter_info != infos.End(); )
		{
			if(!iter_info->name_.Compare(name, true))
			{
				iter_info = infos.Erase(iter_info);
				break;
			}
			else
				++iter_info;
		}

		if(infos.Empty())
			attributes.Erase(iter);
	}

	void Context::AddEventReceiver(Object *receiver, StringHash eventType)
	{
		//if there is no group found, the new group will be added to map
		SharedPtr<EventReceiverGroup>& group = eventReceivers_[eventType];
		if(!group)
		{
			group = new EventReceiverGroup();
		}
		group->Add(receiver);
	}

	void Context::AddEventReceiver(Object *receiver, Object *sender, StringHash eventType)
	{
		SharedPtr<EventReceiverGroup>& group = specificEventReceivers_[sender][eventType];
		if(!group)
			group = new EventReceiverGroup();
		group->Add(receiver);
	}

	void Context::RemoveEventSender(Object *sender)
	{
		auto iter = specificEventReceivers_.Find(sender);
		if(iter != specificEventReceivers_.End())
		{
			for(auto iter2 = iter->second_.Begin(); iter2 != iter->second_.End(); ++iter2)
			{
				for(auto iter3 = iter2->second_->receivers_.Begin(); iter3 != iter2->second_->receivers_.End(); ++iter3)
				{
					Object* receiver = *iter3;
					if(receiver)
						receiver->RemoveEventSender(sender);
				}
			}
			specificEventReceivers_.Erase(iter);
		}
	}

	void Context::RemoveEventReceiver(Object *receiver, Object *sender, StringHash eventType)
	{
		EventReceiverGroup* group = GetEventReceivers(sender, eventType);
		if(group)
			group->Remove(receiver);
	}

	void Context::RemoveEventReceiver(Object *receiver, StringHash eventType)
	{
		EventReceiverGroup* group = GetEventReceivers(eventType);
		if(group)
			group->Remove(receiver);
	}

	void Context::BeginSendEvent(Object *sender, StringHash eventType)
	{
		//todo profile
		eventSenders_.Push(sender);
	}

	void Context::EndSendEvent()
	{
		//todo usage
		eventSenders_.Pop();
	}

	Object *Context::GetEventSender() const
	{
		if(!eventSenders_.Empty())
			return eventSenders_.Back();
		return nullptr;
	}

	const Variant &Context::GetGlobalVar(StringHash key) const
	{
		auto it = globalVars_.Find(key);
		return it != globalVars_.End() ? it->second_ : Variant::EMPTY;
	}

	void Context::SetGlobalVar(StringHash key, const Variant &value)
	{
		globalVars_[key] = value;
	}

	VariantMap &Context::GetEventDataMap()
	{
		unsigned nestingLevel = eventSenders_.Size();
		while (eventDataMaps_.Size() < nestingLevel + 1)
		{
			eventDataMaps_.Push(new VariantMap());
		}
		VariantMap& ret = *eventDataMaps_[nestingLevel];
		ret.Clear();
		return ret;
	}

	Context::Context() :
		eventHandler_(nullptr)
	{
		Thread::SetMainThread();
	}

	Context::~Context()
	{
		RemoveSubsystem(StringHash("Audio"));
		RemoveSubsystem(StringHash("UI"));
		RemoveSubsystem(StringHash("Input"));
		RemoveSubsystem(StringHash("Renderer"));
		RemoveSubsystem(StringHash("Graphics"));

		subSystems_.Clear();
		factories_.Clear();

		for(auto iter = eventDataMaps_.Begin(); iter != eventDataMaps_.End(); ++iter)
		{
			delete *iter;
		}
		eventDataMaps_.Clear();
	}

	SharedPtr<Object> Context::CreateObject(StringHash objectType)
	{
		auto it = factories_.Find(objectType);
		if(it != factories_.End())
			return it->second_->CreateObject();
		else
			return SharedPtr<Object>();
	}

	void Context::RegisterFactory(ObjectFactory *factory)
	{
		if(!factory)
			return;
		factories_[factory->GetType()] = factory;
	}

	void Context::RegisterFactory(ObjectFactory *factory, const char *category)
	{
		if(!factory)
			return;
		RegisterFactory(factory);
		if(String::CStringLength(category))
			objectCategories_[category].Push(factory->GetType());
	}

	void Context::RegisterSubsystem(Object *subsystem)
	{
		if(!subsystem)
			return;

		subSystems_[subsystem->GetType()] = subsystem;
	}

	void Context::RemoveSubsystem(StringHash objectType)
	{
		auto it = subSystems_.Find(objectType);
		if(it != subSystems_.End())
			subSystems_.Erase(it);
	}

	//todo, usage
	AttributeHandle Context::RegisterAttribute(StringHash objectType, const AttributeInfo &attr)
	{
		if(attr.type_ == VAR_NONE || attr.type_ == VAR_VOIDPTR || attr.type_ == VAR_PTR ||
				attr.type_ == VAR_CUSTOM_HEAP || attr.type_ == VAR_CUSTOM_STACK)
		{
			URHO3D_LOGWARNING("Attemp to register unsupported attribute type " +
				Variant::GetTypeName(attr.type_) + " to class " + GetTypeName(objectType));
			return AttributeHandle();
		}

		AttributeHandle handle;
		Vector<AttributeInfo>& objectAttributes = attributes_[objectType];
		objectAttributes.Push(attr);
		handle.attributeInfo_ = &objectAttributes.Back();

		if(attr.mode_ & AM_NET)
		{
			Vector<AttributeInfo>& objectNetworkAttributes = networkAttributes_[objectType];
			objectNetworkAttributes.Push(attr);
			handle.networkAttributeInfo_ = &objectNetworkAttributes.Back();
		}
		//todo, the copy assignment operator is deleted, so the copy-elision do not happen ??
		return handle;
	}

	void Context::RemoveAttribute(StringHash objectType, const char *name)
	{
		RemoveNamedAttribute(attributes_, objectType, name);
		RemoveNamedAttribute(networkAttributes_, objectType, name);
	}

	void Context::RemoveAllAtributes(StringHash objectType)
	{
		attributes_.Erase(objectType);
		networkAttributes_.Erase(objectType);
	}

	void Context::UpdateAttributeDefaultValue(StringHash objectType, const char *name, const Variant &defaultValue)
	{
		AttributeInfo* info = GetAttribute(objectType, name);
		if(info)
			info->defaultValue_ = defaultValue;
	}

	bool Context::RequireSDL(unsigned int sdlFlags)
	{
		++sdlInitCounter;

		if(sdlInitCounter == 1)
		{
			URHO3D_LOGDEBUG("Initialising SDL");
			if(SDL_Init(0) != 0)
			{
				URHO3D_LOGERRORF("Failed to initialise SDL: %s", SDL_GetError());
				return false;
			}
		}

		Uint32 remainingFlags = sdlFlags & ~SDL_WasInit(0);
		if(remainingFlags != 0)
		{
			if(SDL_InitSubSystem(remainingFlags) != 0)
			{
				URHO3D_LOGERRORF("Failed to initialise SDL subsystem: %s", SDL_GetError());
				return false;
			}
		}
		return true;
	}

	void Context::ReleaseSDL()
	{
		--sdlInitCounter;
		if(sdlInitCounter == 0)
		{
			URHO3D_LOGDEBUG("Quitting SDL");
			SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
			SDL_Quit();
		}
		if(sdlInitCounter < 0)
			URHO3D_LOGERROR("Too many calls to Context::ReleaseSDL()!");
	}

	void Context::CopyBaseAttributes(StringHash baseType, StringHash derivedType)
	{
		if(baseType == derivedType)
		{
			URHO3D_LOGWARNING("Attempt to copy base attributes to itself for class" +
								GetTypeName(baseType));
			return;
		}

		const Vector<AttributeInfo>* baseAttributes = GetAttributes(baseType);
		if(baseAttributes)
		{
			for(unsigned i=0; i< baseAttributes->Size(); ++i)
			{
				const AttributeInfo& attr = baseAttributes->At(i);
				attributes_[derivedType].Push(attr);
				if(attr.mode_ & AM_NET)
					networkAttributes_[derivedType].Push(attr);
			}
		}
	}


	Object *Context::GetSubsystem(StringHash type) const
	{
		auto iter = subSystems_.Find(type);
		if(iter != subSystems_.End())
		{
			return iter->second_;
		}
		else
			return nullptr;
	}

	const String &Context::GetTypeName(StringHash objectType) const
	{
		auto iter = factories_.Find(objectType);
		return iter != factories_.End() ? iter->second_->GetTypeName() : String::EMPTY;
	}

	AttributeInfo *Context::GetAttribute(StringHash objectType, const char *name)
	{
		auto iter = attributes_.Find(objectType);
		if(iter == attributes_.End())
			return nullptr;

		Vector<AttributeInfo>& infos = iter->second_;

		for(auto iterInfo = infos.Begin(); iterInfo != infos.End(); ++iterInfo)
		{
			if(!iterInfo->name_.Compare(name, true))
			{
				return &(*iterInfo);
			}
		}
		return nullptr;
	}

}
