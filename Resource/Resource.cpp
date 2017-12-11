//
// Created by LIUHAO on 2017/12/5.
//

#include "Resource.h"
#include "../Core/Thread.h"
#include "../IO/File.h"

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
		File file(context_);
		return file.Open(fileName, FILE_WRITE) && Save(file);
	}

	void Resource::SetName(const String &name)
	{
		name_ = name;
		nameHash_ = StringHash(name);
	}

	void Resource::SetMemoryUse(unsigned size)
	{
		memoryUse_ = size;
	}

	void Resource::ResetUseTimer()
	{
		useTimer_.Reset();
	}

	void Resource::SetAsyncLoadState(AsyncLoadState newState)
	{
		asyncLoadState_ = newState;
	}

	unsigned Resource::GetUseTimer()
	{
		if(Refs() > 1)
		{
			useTimer_.Reset();
			return 0;
		}
		else
			return useTimer_.GetMSec(false);
	}

	void ResourceWithMetadata::AddMetadata(const String &name, const VariantMap &value)
	{

	}

	void ResourceWithMetadata::RemoveMetadata(const String &name)
	{

	}

	void ResourceWithMetadata::RemoveAllMetadata()
	{

	}

	const VariantMap &ResourceWithMetadata::GetMetadata(const String &name) const
	{
		return <#initializer#>;
	}

	bool ResourceWithMetadata::HasMetadata() const
	{
		return false;
	}

	void ResourceWithMetadata::LoadMetadataFromXML(const XMLElement &source)
	{

	}

	void ResourceWithMetadata::LoadMetadataFromJSON(const JSONArray &array)
	{

	}

	void ResourceWithMetadata::SaveMetadataToXML(XMLElement &destination) const
	{

	}

	void ResourceWithMetadata::CopyMetadata(const ResourceWithMetadata &source)
	{

	}
}


