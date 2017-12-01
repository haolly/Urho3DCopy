//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_OBJECT_H
#define URHO3DCOPY_OBJECT_H

#include <functional>
#include "../Container/RefCounted.h"
#include "../Math/StringHash.h"
#include "../Container/Str.h"
#include "Variant.h"
#include "../Container/LinkedList.h"
#include "../Container/Ptr.h"

namespace Urho3D
{
    class Context;
    class EventHandler;

    class TypeInfo
    {
    public:
        TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
        ~TypeInfo();

        bool IsTypeOf(StringHash type) const;
        bool IsTypeOf(const TypeInfo* typeInfo) const;

        template <typename T> bool IsTypeOf() const
        {
            IsTypeOf(T::GetTypeInfoStatic());
        }

        StringHash GetType() const
        {
            return type_;
        }

        const String& GetTypeName() const
        {
            return typeName_;
        }

        const TypeInfo* GetBaseTypeInfo() const
        {
            return baseTypeInfo_;
        }

    private:
        StringHash type_;
        String typeName_;
        const TypeInfo* baseTypeInfo_;
    };

#define URHO3D_OBJECT(typeName, baseTypeName) \
	public: \
		using ClassName = typeName; \
		using BaseClassName = baseTypeName; \
		virtual Urho3D::StringHash GetType() const override { return GetTypeInfoStatic() ->GetType();} \
		virtual const Urho3D::String& GetTypeInfo() const override { return GetTypeInfoStatic() ->GetTypeName(); } \
		virtual const Urho3D::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic();} \
		static Urho3D::StringHash GetTypeStatic() { return GetTypeInfoStatic() ->GetType();} \
		static const Urho3D::String& GetTypeNameStatic() { return GetTypeInfoStatic() ->GetTypeName();} \
		static const Urho3D::TypeInfo* GetTypeInfoStatic() { static const Urho3D::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic;} \



    class Object : public RefCounted
    {
        friend class Context;
    public:
	    Object(Context* context);
	    virtual ~Object() override;

	    virtual StringHash GetType() const = 0;
	    virtual const String& GetTypeName() const = 0;
	    virtual const TypeInfo* GetTypeInfo() const = 0;

	    virtual void OnEvent(Object* sender, StringHash eventType, VariantMap& eventData);

	    static const TypeInfo* GetTypeInfoStatic() { return nullptr;}

	    bool IsInstanceOf(StringHash type) const;
	    bool IsInstanceOf(const TypeInfo* typeInfo) const;
	    template<typename  T> bool IsInstanceOf() const
	    {
		    return IsInstanceOf(T::GetTypeInfoStatic());
	    }
	    template<typename T> T* Cast()
	    {
		    return IsInstanceOf<T>() ? static_cast<T*>(this) : nullptr;
	    }
	    template <typename T> const T* Cast() const
	    {
			return IsInstanceOf<T>() ? static_cast<const T*>(this) : nullptr;
	    }


	    //Subscribe to an event that can be sent by any sender
	    void SubscribeToEvent(StringHash eventType, EventHandler* handler);
	    void SubscribeToEvent(StringHash eventType, const std::function<void(StringHash, VariantMap&)>& function, void* userData = nullptr);
	    //Subscribe to an event that can be sent by a specific sender
	    void SubscribeToEvent(Object* sender, StringHash eventType, EventHandler* handler);
	    void SubscribeToEvent(Object* sender, StringHash eventType, const std::function<void(StringHash, VariantMap)>& function, void* userData = nullptr);


	    void UnsubscribeFromEvent(StringHash eventType);
	    void UnsubscribeFromEvent(Object* sender, StringHash eventType);
	    void UnsubscribeFromEvent(Object* sender);

	    void UnsubscribeFromAllEvents();
		void UnsubscribeFromEventsExcept(const PODVector<StringHash>& exceptions, bool onlyUserData);

	    void SendEvent(StringHash eventType);
	    void SendEvent(StringHash eventType, VariantMap& eventData);

	    VariantMap& GetEventDataMap() const;

	    template <typename... Args> void SendEvent(StringHash eventType, Args... args)
	    {
		    SendEvent(eventType, GetEventDataMap().Populate(args...));
	    }

	    Context* GetContext() const { return context_;}

	    const Variant& GetGlobalVar(StringHash key) const;
	    const VariantMap& GetGlobalVars() const;
	    void SetGlobalVar(StringHash key, const Variant& value);

	    Object* GetSubsystem(StringHash type) const;
	    Object* GetEventSender() const;
	    EventHandler* GetEventHandler() const;

	    bool HasSubscribedToEvent(StringHash eventType) const;
	    bool HasSubscribedToEvent(Object* sender, StringHash eventType);

	    bool HasEventHandlers() const
	    {
		    return !eventHandlers_.Empty();
	    }

	    template <class T>
		T* GetSubsystem() const;

	    const String& GetCategory() const;

	    void SetBlockEvents(bool block) { blockEvents_ = block; }
	    bool GetBlockEvents() const { return blockEvents_; }



    protected:
	    Context* context_;
    private:
	    EventHandler* FindEventHandler(StringHash eventType, EventHandler** previous = nullptr) const;
	    EventHandler* FindSpecificEventHandler(Object* sender, EventHandler** previous = nullptr) const;
	    EventHandler* FindSepcificEventHandler(Object* sender, StringHash eventType, EventHandler** previous = nullptr) const;
	    void RemoveEventSender(Object* sender);
	    LinkedList<EventHandler> eventHandlers_;
	    bool blockEvents_;
    };

	class ObjectFactory : public RefCount
	{
	public:
		ObjectFactory(Context* context) :
			context_(context)
		{
			assert(context_);
		}

		virtual SharedPtr<Object> CreateObject() = 0;

		Context* GetContext() const { return context_; }
		const TypeInfo* GetTypeInfo() const { return typeInfo_;}
		StringHash GetType() const { return typeInfo_->GetType();}
		const String& GeTypeName() { return typeInfo_->GetTypeName();}

	protected:
		Context* context_;
		const TypeInfo* typeInfo_;
	};

	template <class T>
	class ObjectFactoryImpl : public ObjectFactory
	{
	public:
		ObjectFactoryImpl(Context* context) :
				ObjectFactory(context)
		{
			typeInfo_ = T::GetTypeInfoStatic();
		}

		virtual SharedPtr<Object> CreateObject() override
		{
			return SharedPtr<Object>(new T(context_));
		}
	};


	class EventHandler : public LinkedListNode
	{
	public:
		EventHandler(Object* receiver, void* userData = nullptr) :
				receiver_(receiver),
				sender_(nullptr),
				userData_(userData)
		{
		}

		virtual ~EventHandler()
		{
		}

		void SetSenderAndEventType(Object* sender, StringHash eventType)
		{
			sender_ = sender;
			eventType_ = eventType;
		}

		virtual void Invoke(VariantMap& eventData) = 0;
		virtual EventHandler* Clone() const = 0;

		Object* GetReceiver() const { return receiver_;}
		Object* GetSender() const { return sender_;}
		const StringHash& GetEventType() const { return eventType_;}
		void* GetUserData() const { return userData_;}

	protected:
		Object* receiver_;
		Object* sender_;
		StringHash eventType_;
		void* userData_;
	};

	template <class T>
	class EventHandlerImpl : public EventHandler
	{
	public:
		using HandlerFunctionPtr = void (T::*)(StringHash, VariantMap&);
		EventHandlerImpl(T* receiver, HandlerFunctionPtr functionPtr, void* userData = nullptr) :
				EventHandler(receiver, userData),
				function_(functionPtr)
		{
			assert(receiver);
			assert(function_);
		}

		virtual void Invoke(VariantMap& eventData) override
		{
			T* receiver = static_cast<T*>(receiver);
			(receiver->*function_)(eventType_, eventData);
		}

		virtual EventHandler* Clone() const override
		{
			return new() EventHandlerImpl(static_cast<T*>(receiver_), function_, userData_);
		}

	private:
		HandlerFunctionPtr function_;
	};

	class EventHandler11Impl : public EventHandler
	{
	public:
		EventHandler11Impl(std::function<void(StringHash, VariantMap&)> function, void* userData = nullptr) :
				EventHandler(nullptr, userData),
				function_(function)
		{
			assert(function_);
		}

		virtual void Invoke(VariantMap& eventData) override
		{
			function_(eventType_, eventData);
		}

		virtual EventHandler* Clone() const override
		{
			return new EventHandler11Impl(function_, userData_);
		}

	private:
		std::function<void (StringHash, VariantMap&)> function_;

	};

}


#endif //URHO3DCOPY_OBJECT_H
