//
// Created by liuhao on 2017/11/27.
//

#include "Object.h"
#include "Context.h"
#include "Thread.h"
#include "../Container/HashSet.h"

namespace Urho3D
{
    TypeInfo::TypeInfo(const char *typeName, const TypeInfo *baseTypeInfo) :
        type_(typeName),
        typeName_(typeName),
        baseTypeInfo_(baseTypeInfo)
    {
    }

    TypeInfo::~TypeInfo()
    {

    }

    bool TypeInfo::IsTypeOf(StringHash type) const
    {
        const TypeInfo* current = this;
        while (current)
        {
            if(current->GetType() == type)
            {
                return true;
            }
            current = current->GetBaseTypeInfo();
        }
        return false;
    }

    bool TypeInfo::IsTypeOf(const TypeInfo *typeInfo) const
    {
        const TypeInfo* current = this;
        while (current)
        {
            if(current == typeInfo)
            {
                return true;
            }
            current = current->GetBaseTypeInfo();
        }
        return false;
    }

    Object::Object(Context *context) :
		context_(context),
		blockEvents_(false)
    {
    }

	Object::~Object()
	{
		UnsubscribeFromAllEvents();
		context_->RemoveEventSender(this);
	}

	void Object::OnEvent(Object *sender, StringHash eventType, VariantMap &eventData)
	{
		if(blockEvents_)
			return;

		Context* context = context_;
		EventHandler* specific = nullptr;
		EventHandler* nonSpecific = nullptr;

		EventHandler* handler = eventHandlers_.First();
		while(handler)
		{
			if(handler->GetEventType() == eventType)
			{
				if(!handler->GetSender())
					nonSpecific = handler;
				else if(handler->GetSender() == sender)
				{
					specific = handler;
					break;
				}
			}
			handler = eventHandlers_.Next(handler);
		}

		if(specific)
		{
			context->SetEventHandler(specific);
			specific->Invoke(eventData);
			context->SetEventHandler(nullptr);
			return;
		}
		if(nonSpecific)
		{
			context->SetEventHandler(nonSpecific);
			nonSpecific->Invoke(eventData);
			context->SetEventHandler(nullptr);
		}
	}

	bool Object::IsInstanceOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	bool Object::IsInstanceOf(const TypeInfo *typeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(typeInfo);
	}

	/// Specific sender and Nonspecific sender are allowed at the same time
	/// \param eventType
	/// \param handler
	void Object::SubscribeToEvent(StringHash eventType, EventHandler *handler)
	{
		if(!handler)
			return;

		handler->SetSenderAndEventType(nullptr, eventType);
		EventHandler* previous;
		EventHandler* oldHandler = FindSpecificEventHandler(nullptr, eventType, &previous);
		if(oldHandler)
		{
			eventHandlers_.Erase(oldHandler, previous);
			eventHandlers_.InsertFront(handler);
		}
		else
		{
			context_->AddEventReceiver(this, eventType);
			eventHandlers_.InsertFront(handler);
		}
	}

	void Object::SubscribeToEvent(StringHash eventType, const std::function<void(StringHash, VariantMap &)> &function,
	                              void *userData)
	{
		SubscribeToEvent(eventType, new EventHandler11Impl(function, userData));
	}


	void Object::SubscribeToEvent(Object *sender, StringHash eventType,
	                              const std::function<void(StringHash, VariantMap&)> &function, void *userData)
	{
		SubscribeToEvent(sender, eventType, new EventHandler11Impl(function, userData));
	}

	/// Specific sender and Nonspecific sender are allowed at the same time
	/// \param eventType
	/// \param handler
	void Object::SubscribeToEvent(Object *sender, StringHash eventType, EventHandler *handler)
	{
		if(!sender || !handler)
		{
			delete handler;
			return;
		}
		handler->SetSenderAndEventType(sender, eventType);
		EventHandler* previous;
		EventHandler* oldHandler = FindSpecificEventHandler(sender, eventType, &previous);
		if(oldHandler)
		{
			eventHandlers_.Erase(oldHandler, previous);
			eventHandlers_.InsertFront(handler);
		}
		else
		{
			eventHandlers_.InsertFront(handler);
			context_->AddEventReceiver(this, sender, eventType);
		}
	}

	EventHandler *
	Object::FindSpecificEventHandler(Object *sender, StringHash eventType, EventHandler **previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if(previous)
		{
			//Note, need this when return value is the First()
			*previous = nullptr;
		}
		while (handler)
		{
			if(handler->GetSender() == sender && handler->GetEventType() == eventType)
			{
				return handler;
			}
			if(previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}
		return nullptr;
	}


	/// Find **THE FIRST** eventHandler with eventType
	/// \param eventType
	/// \param previous
	/// \return
	EventHandler *Object::FindEventHandler(StringHash eventType, EventHandler **previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if(previous)
			*previous = nullptr;
		while (handler)
		{
			if(handler->GetEventType() == eventType)
				return handler;
			if(previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}
		return nullptr;
	}

	/// Find eventHandler with sender
	/// \param sender
	/// \param previous
	/// \return
	EventHandler *Object::FindSpecificEventHandler(Object *sender, EventHandler **previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if(previous)
			*previous = nullptr;

		while(handler)
		{
			if(handler->GetSender() == sender)
				return handler;
			if(previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}
		return nullptr;
	}

	void Object::RemoveEventSender(Object *sender)
	{

	}


	/// Unsubscribe **ALL** eventType event
	/// \param eventType
	void Object::UnsubscribeFromEvent(StringHash eventType)
	{
		while(true)
		{
			EventHandler* previous;
			EventHandler* handler = FindEventHandler(eventType, &previous);
			if(handler)
			{
				if(handler->GetSender())
					context_->RemoveEventReceiver(this, handler->GetSender(), eventType);
				else
					context_->RemoveEventReceiver(this, eventType);
				eventHandlers_.Erase(handler, previous);
			}
			else
				break;
		}
	}

	void Object::UnsubscribeFromEvent(Object *sender, StringHash eventType)
	{
		if(!sender)
			return;

		EventHandler* previous;
		EventHandler* handler = FindSpecificEventHandler(sender, eventType, &previous);
		if(handler)
		{
			context_->RemoveEventReceiver(this, sender, eventType);
			eventHandlers_.Erase(handler, previous);
		}
	}

	void Object::UnsubscribeFromEvent(Object *sender)
	{
		if(!sender)
			return;
		while(true)
		{
			EventHandler* previous;
			EventHandler* handler = FindSpecificEventHandler(sender, &previous);
			if(handler)
			{
				context_->RemoveEventReceiver(this, sender, handler->GetEventType());
				eventHandlers_.Erase(handler, previous);
			}
			else
				break;
		}
	}

	void Object::UnsubscribeFromAllEvents()
	{
		while (true)
		{
			EventHandler* handler = eventHandlers_.First();
			if(handler)
			{
				if(handler->GetSender())
					context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
				else
					context_->RemoveEventReceiver(this, handler->GetEventType());
				eventHandlers_.Erase(handler);
			}
			else
				break;
		}
	}


	void Object::UnsubscribeFromEventsExcept(const PODVector<StringHash> &exceptions, bool onlyUserData)
	{
		EventHandler* handler = eventHandlers_.First();
		EventHandler* previous = nullptr;
		while (handler)
		{
			EventHandler* next = eventHandlers_.Next(handler);
			if(!exceptions.Contains(handler->GetEventType()))
			{
				if((onlyUserData && handler->GetUserData()) || (!onlyUserData))
				{
					if(handler->GetSender())
						context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
					else
						context_->RemoveEventReceiver(this, handler->GetEventType());
					eventHandlers_.Erase(handler, previous);
				}
			}
			else
				previous = handler;
			handler = next;
		}
	}

	void Object::SendEvent(StringHash eventType)
	{
		VariantMap noEventData;
		SendEvent(eventType, noEventData);
	}

	void Object::SendEvent(StringHash eventType, VariantMap &eventData)
	{
		if (!Thread::IsMainThread())
		{
			//todo add log
			return;
		}
		if(blockEvents_)
			return;

		WeakPtr<Object> self(this);
		Context* context = context_;
		HashSet<Object*> processed;

		context->BeginSendEvent(this, eventType);

		// First check specific event receiver
		SharedPtr<EventReceiverGroup> group(context->GetEventReceivers(this, eventType));
		if(group)
		{
			group->BeginSendEvent();
			const unsigned numReceivers = group->receivers_.Size();
			for (unsigned i = 0; i < numReceivers; ++i) {
				Object* receiver = group->receivers_[i];
				//Note, Holes my exist if receivers removed during send
				if(!receiver)
					continue;

				receiver->OnEvent(this, eventType, eventData);
				//todo, under what conditions/circumstances will happen?
				if(self.Expired())
				{
					group->EndSendEvent();
					context->EndSendEvent();
					return;
				}

				processed.Insert(receiver);
			}
			group->EndSendEvent();
		}

		// Second check no specific receiver
		group = context->GetEventReceivers(eventType);
		if(group)
		{
			group->BeginSendEvent();
			const unsigned numReceivers = group->receivers_.Size();
			for(unsigned i=0; i<numReceivers; ++i)
			{
				Object* receiver = group->receivers_[i];
				if(!receiver || processed.Contains(receiver))
					continue;
				receiver->OnEvent(this, eventType, eventData);

				if(self.Expired())
				{
					group->EndSendEvent();
					context->EndSendEvent();
					return;
				}
			}
			group->EndSendEvent();
		}
		context->EndSendEvent();
	}

	VariantMap &Object::GetEventDataMap() const
	{
		return context_->GetEventDataMap();
	}

	const Variant &Object::GetGlobalVar(StringHash key) const
	{
		return context_->GetGlobalVar(key);
	}

	const VariantMap &Object::GetGlobalVars() const
	{
		return <#initializer#>;
	}

	void Object::SetGlobalVar(StringHash key, const Variant &value) {

	}

	Object *Object::GetSubsystem(StringHash type) const {
		return nullptr;
	}

	Object *Object::GetEventSender() const {
		return nullptr;
	}

	EventHandler *Object::GetEventHandler() const {
		return nullptr;
	}

	bool Object::HasSubscribedToEvent(StringHash eventType) const {
		return false;
	}

	bool Object::HasSubscribedToEvent(Object *sender, StringHash eventType) {
		return false;
	}

	template<class T>
	T *Object::GetSubsystem() const {
		return nullptr;
	}

	const String &Object::GetCategory() const {
		return <#initializer#>;
	}

	StringHash EventNameRegistrar::RegisterEventName(const char *eventName)
	{
		StringHash id(eventName);
		GetEventNameMap()[id] = eventName;
		return id;
	}

	const String &EventNameRegistrar::GetEventName(StringHash eventID)
	{
		auto iter = GetEventNameMap().Find(eventID);
		return iter != GetEventNameMap().End() ? iter->second_ : String::EMPTY;
	}

	HashMap<StringHash, String> &EventNameRegistrar::GetEventNameMap()
	{
		static HashMap<StringHash, String> eventNames_;
		return eventNames_;
	}
}

