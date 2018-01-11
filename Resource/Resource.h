//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_RESOURCE_H
#define URHO3DCOPY_RESOURCE_H

#include "../Core/Object.h"
#include "../Core/Timer.h"
#include "../IO/Serializer.h"

namespace Urho3D
{
	class Deserializer;
	class XMLElement;

	enum AsyncLoadState
	{
		ASYNC_DONE = 0,
		ASYNC_QUEUED = 1,
		ASYNC_LOADING = 2,
		ASYNC_SUCCESS = 3,
		ASYNC_FAIL = 4,
	};


	class Resource : public Object
	{
		URHO3D_OBJECT(Resource, Object);

	public:
		Resource(Context* context);

		bool Load(Deserializer& source);
		virtual bool BeginLoad(Deserializer& source);
		virtual bool EndLoad();
		virtual bool Save(Serializer& dest) const;

		bool LoadFile(const String& fileName);
		virtual bool SaveFile(const String& fileName) const;

		void SetName(const String& name);
		void SetMemoryUse(unsigned size);
		void ResetUseTimer();
		void SetAsyncLoadState(AsyncLoadState newState);

		const String& GetName() const { return name_; }
		StringHash GetNameHash() const { return nameHash_; }
		unsigned GetMemoryUse() const { return memoryUse_; }
		unsigned GetUseTimer();
		AsyncLoadState GetAsyncLoadState() const { return asyncLoadState_; }

	private:
		String name_;
		StringHash nameHash_;
		Timer useTimer_;
		unsigned memoryUse_;
		AsyncLoadState asyncLoadState_;
	};

	class ResourceWithMetadata : public Resource
	{
		URHO3D_OBJECT(ResourceWithMetadata, Resource);
	public:
		ResourceWithMetadata(Context* context) :
				Resource(context)
		{
		}

		void AddMetadata(const String& name, const Variant& value);
		void RemoveMetadata(const String& name);
		void RemoveAllMetadata();
		const Variant& GetMetadata(const String& name) const;
		bool HasMetadata() const;

	protected:
		void LoadMetadataFromXML(const XMLElement& source);
//		void LoadMetadataFromJSON(const JSONArray& array);
		void SaveMetadataToXML(XMLElement& destination) const;
		void CopyMetadata(const ResourceWithMetadata& source);

	private:
		VariantMap metadata_;
		StringVector metadataKeys_;
	};

	inline const String& GetResourceName(Resource* resource)
	{
		return resource ? resource->GetName() : String::EMPTY;
	}

	inline StringHash GetResourceType(Resource* resource, StringHash defaultType)
	{
		return resource ? resource->GetType() : defaultType;
	}

	inline ResourceRef GetResourceRef(Resource* resource, StringHash defaultType)
	{
		return ResourceRef(GetResourceType(resource, defaultType), GetResourceName(resource));
	}

	template <class T>
	Vector<String> GetResourceNames(const Vector<SharedPtr<T> >& resources)
	{
		Vector<String> ret(resources.Size());
		for(unsigned i = 0; i< resources.Size(); ++i)
		{
			ret[i] = GetResourceName(resources[i]);
		}
		return ret;
	}

	template <class T>
	ResourceRefList GetResourceRefList(const Vector<SharedPtr<T> >& resources)
	{
		return ResourceRefList(T::GetTypeStatic(), GetResourceNames(resources));
	}
}



#endif //URHO3DCOPY_RESOURCE_H
