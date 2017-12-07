//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_RESOURCE_H
#define URHO3DCOPY_RESOURCE_H

#include "../Core/Object.h"
#include "../Core/Timer.h"

namespace Urho3D
{
	class Deserializer;
	class Serializer;
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

	private:
		String name_;
		StringHash nameHash_;
		Timer useTimer_;
		unsigned memoryUse_;
		AsyncLoadState asyncLoadState_;
	};

}



#endif //URHO3DCOPY_RESOURCE_H
