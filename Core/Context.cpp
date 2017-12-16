//
// Created by liuhao on 2017/11/27.
//

#include "Context.h"
#include "Thread.h"

namespace Urho3D
{

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

	void Context::RemoveEventSender(Object *sender) {

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
		//todo
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

	void Context::RemoveAttribute(StringHash objectType, const char *name)
	{

	}

	void Context::RemoveAllAtributes(StringHash objectType)
	{

	}

	void Context::UpdateAttributeDefaultValue(StringHash objectType, const char *name, const Variant &defaultValue)
	{

	}

	bool Context::RequireSDL(unsigned int sdlFlags)
	{
		return false;
	}

	void Context::ReleaseSDL()
	{

	}

	void Context::CopyBaseAttributes(StringHash baseType, StringHash derivedType)
	{

	}

	template<class T>
	void Context::RegisterFactory()
	{

	}

	template<class T>
	void Context::RegisterFactory(const char *category)
	{

	}

	template<class T>
	T *Context::RegisterSubsystem()
	{
		return nullptr;
	}

	template<class T>
	void Context::RemoveSubsystem()
	{

	}

	template<class T>
	AttributeHandle Context::RegisterAttribute(const AttributeInfo &attr)
	{
		return nullptr;
	}

	Object *Context::GetSubsystem(StringHash key) const
	{
		return nullptr;
	}

	const String &Context::GetTypeName(StringHash objectType) const
	{
		return <#initializer#>;
	}
}
