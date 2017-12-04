//
// Created by liuhao on 2017/11/27.
//

#include "Context.h"

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
			for (unsigned i = receivers_.Size() -1; i >= 0; --i) {
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
		SharedPtr<EventReceiverGroup>& group = eventReceivers_[eventType];
		//TODO, if there is no group found, the new group will be added to map???
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
		eventSenders_.Pop();
	}
}
