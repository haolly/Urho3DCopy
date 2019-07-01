//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_CONTEXT_H
#define URHO3DCOPY_CONTEXT_H

#include "../Container/RefCounted.h"
#include "Object.h"
#include "../Math/StringHash.h"
#include "Attribute.h"
#include "../IO/Log.h"

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

	    AttributeHandle RegisterAttribute(StringHash objectType, const AttributeInfo& attr);
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
		template <class T> void RemoveAttribute(const char* name);
		template <class T> void RemoveAllAttributes();
		template <class T, class U> void CopyBaseAttributes();
		template <class T> void UpdateAttributeDefaultValue(const char* name, const Variant& defalutValue);


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

		AttributeInfo* GetAttribute(StringHash objectType, const char* name);

		template <class T> T* GetSubsystem() const
		{
			return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
		}

		template <class T> AttributeInfo* GetAttribute(const char* name)
		{
			return GetAttribute(T::GetTypeStatic(), name);
		}

		const Vector<AttributeInfo>* GetAttributes(StringHash type) const
		{
			auto it = attributes_.Find(type);
			return it != attributes_.End() ? &(it->second_) : nullptr;
		}

		const Vector<AttributeInfo>* GetNetworkAttributes(StringHash type) const
		{
			auto it = networkAttributes_.Find(type);
			return it != networkAttributes_.End() ? &it->second_ : nullptr;
		}

		const HashMap<StringHash, Vector<AttributeInfo> >& GetAllAttributes() const
		{
			return attributes_;
		};

		EventReceiverGroup* GetEventReceivers(Object* sender, StringHash eventType)
		{
			auto it = specificEventReceivers_.Find(sender);
			if(it != specificEventReceivers_.End())
			{
				auto eventGroupIter = it->second_.Find(eventType);
				if(eventGroupIter != it->second_.End())
					return eventGroupIter->second_;
				else
					return nullptr;
			}
			return nullptr;
		}

		EventReceiverGroup* GetEventReceivers(StringHash eventType)
		{
			auto it = eventReceivers_.Find(eventType);
			if(it != eventReceivers_.End())
			{
				return it->second_;
			}
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
		//todo, 这些 attribute 是什么时候添加的, CopyBaseAttributes ??
	    HashMap<StringHash, Vector<AttributeInfo> > attributes_;
	    HashMap<StringHash, Vector<AttributeInfo> > networkAttributes_;

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

	template <class T>
	void Context::RegisterFactory()
	{
		RegisterFactory(new ObjectFactoryImpl<T>(this));
	}

	template <class T>
	void Context::RegisterFactory(const char* category)
	{
		RegisterFactory(new ObjectFactoryImpl<T>(this), category);
	}

	template <class T>
	T* Context::RegisterSubsystem()
	{
		T* subsystem = new T(this);
		RegisterSubsystem(subsystem);
		return subsystem;
	}

	template<class T>
	void Context::RemoveSubsystem()
	{
		RemoveSubsystem(T::GetTypeStatic());
	}

	//todo, when to call this?
	template<class T>
	AttributeHandle Context::RegisterAttribute(const AttributeInfo &attr)
	{
		return RegisterAttribute(T::GetTypeStatic(), attr);
	}

	template <class T>
	void Context::RemoveAttribute(const char* name)
	{
		RemoveAttribute(T::GetTypeStatic(), name);
	}

	template <class T>
	void Context::RemoveAllAttributes()
	{
		RemoveAttribute(T::GetTypeStatic());
	}

	template <class T, class U>
	void Context::CopyBaseAttributes()
	{
		CopyBaseAttributes(T::GetTypeStatic(), U::GetTypeStatic());
	}

	template <class T>
	void Context::UpdateAttributeDefaultValue(const char* name, const Variant& defalutValue)
	{
		UpdateAttributeDefaultValue(T::GetTypeStatic(), name, defalutValue);
	}


}


#endif //URHO3DCOPY_CONTEXT_H
