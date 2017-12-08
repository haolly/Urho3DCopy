//
// Created by LIUHAO on 2017/12/5.
//

#include "Resource.h"
#include "../Core/Thread.h"

namespace Urho3D
{

	Resource::Resource(Context *context) :
			Object(context),
			memoryUse_(0),
			asyncLoadState_(ASYNC_DONE)
	{
	}

	bool Resource::Load(Deserializer &source)
	{
		{
			//todo profile
		}
		SetAsyncLoadState(Thread::IsMainThread() ? ASYNC_DONE : ASYNC_LOADING);
		bool success = BeginLoad(source);
		if(success)
			success &= EndLoad();
		SetAsyncLoadState(ASYNC_DONE);
		return success;
	}

	bool Resource::BeginLoad(Deserializer &source)
	{
		return false;
	}

	bool Resource::EndLoad()
	{
		return false;
	}

	bool Resource::Save(Serializer &dest) const
	{
		//todo add Error Log
		return false;
	}

	bool Resource::LoadFile(const String &fileName)
	{
		File file(context_);
		return file.Open(fileName, FILE_READ) && Load(file);
	}

	bool Resource::SaveFile(const String &fileName) const
	{
		return false;
	}

	void Resource::SetName(const String &name)
	{

	}

	void Resource::SetMemoryUse(unsigned size)
	{

	}

	void Resource::ResetUseTimer()
	{

	}

	void Resource::SetAsyncLoadState(AsyncLoadState newState)
	{

	}
}


