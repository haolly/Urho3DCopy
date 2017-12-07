//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_CONTEXT_H
#define URHO3DCOPY_CONTEXT_H

#include "../Container/RefCounted.h"
#include "Object.h"
#include "../Math/StringHash.h"

namespace Urho3D
{

    class EventReceiverGroup : public RefCounted
    {
    public:
	    EventReceiverGroup() :
			inSend_(0),
			dirty_(false)
	    {
	    }

	    void BeginSendEvent();
	    void EndSendEvent();
	    void Add(Object* object);
	    void Remove(Object* object);

	    PODVector<Object*> receivers_;

    private:
	    unsigned inSend_;
	    bool dirty_;
    };

    class Context : public RefCounted
    {
		friend class Object;

    public:
	    Context();

	    virtual ~Context() override;

	    template <class T> inline
	    SharedPtr<T> CreateObject()
	    {
		    return StaticCast<T>(CreateObject(T::GetTypeStatic()));
	    }

	    SharedPtr<Object> CreateObject(StringHash objectType);

	    void RegisterFactory(ObjectFactory* factory);
	    void RegisterFactory(ObjectFactory* factory, const char* category);

	    void RegisterSubsystem(Object* subsystem);
	    void RemoveSubsystem(StringHash objectType);

//	    AttributeHandle RegisterAttribute(StringHash objectType, const AttributeInfo& attr);
	    void RemoveAttribute(StringHash objectType, const char* name);
	    void RemoveAllAtributes(StringHash objectType);
	    void UpdateAttributeDefaultValue(StringHash objectType, const char* name, const Variant& defaultValue);

	    VariantMap& GetEventDataMap();
	    bool RequireSDL(unsigned int sdlFlags);
	    void ReleaseSDL();

#ifdef URHO3D_IK
		void RequireIK();
		void ReleaseIK();
#endif

		void CopyBaseAttributes(StringHash baseType, StringHash derivedType);
		template <class T> void RegisterFactory();
		template <class T> void RegisterFactory(const char* category);
		template <class T> T* RegisterSubsystem();
		template <class T> void RemoveSubsystem();
		template <class T> AttributeHandle RegisterAttribute(const AttributeInfo& attr);
		//todo

		Object* GetSubsystem(StringHash key) const;

		const Variant& GetGlobalVar(StringHash key) const;

		const VariantMap& GetGlobalVars() const
		{
			return globalVars_;
		}

		void SetGlobalVar(StringHash key, const Variant& value);

		const HashMap<StringHash, SharedPtr<Object> >& GetSubsystems() const
		{
			return subSystems_;
		};

		const HashMap<String, Vector<StringHash> >& GetObjectCategories() const
		{
			return objectCategories_;
		};

		Object* GetEventSender() const;

		EventHandler* GetEventHandler() const { return eventHandler_; }
		const String& GetTypeName(StringHash objectType) const;





		EventReceiverGroup* GetEventReceivers(Object* sender, StringHash eventType)
		{
			//todo
			auto it = specificEventReceivers_.Find(sender);
			if(it != specificEventReceivers_.End())
			{

			}
			return nullptr;
		}

		EventReceiverGroup* GetEventReceivers(StringHash eventType)
		{
			//todo
			return nullptr;
		}


    private:
	    void AddEventReceiver(Object* receiver, StringHash eventType);
	    void AddEventReceiver(Object* receiver, Object* sender, StringHash eventType);

		void RemoveEventSender(Object* sender);
	    void RemoveEventReceiver(Object* receiver, Object* sender, StringHash eventType);
	    void RemoveEventReceiver(Object* receiver, StringHash eventType);

		void BeginSendEvent(Object* sender, StringHash eventType);
		void EndSendEvent();
	    void SetEventHandler(EventHandler* handler) { eventHandler_ = handler; }

	    HashMap<StringHash, SharedPtr<ObjectFactory> > factories_;
	    HashMap<StringHash, SharedPtr<Object> > subSystems_;
//	    HashMap<StringHash, Vector<AttributeInfo> > attributes_;
//	    HashMap<StringHash, Vector<AttributeInfo> > networkAttributes_;

	    HashMap<StringHash, SharedPtr<EventReceiverGroup> > eventReceivers_;
		HashMap<Object*, HashMap<StringHash, SharedPtr<EventReceiverGroup> > > specificEventReceivers_;

		PODVector<Object*> eventSenders_;
		// cache eventDataMap
		// ref https://urho3d.github.io/documentation/1.7/_events.html
		PODVector<VariantMap*> eventDataMaps_;
		EventHandler* eventHandler_;
		// todo, what is this usage
		HashMap<String, Vector<StringHash> > objectCategories_;
		VariantMap globalVars_;
    };
}


#endif //URHO3DCOPY_CONTEXT_H
