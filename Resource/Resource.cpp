//
// Created by LIUHAO on 2017/12/5.
//

#include "Resource.h"
#include "../Core/Thread.h"
#include "../IO/File.h"
#include "../IO/Log.h"

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
		// This always needs to be overriden by subclasses
		return false;
	}

	bool Resource::EndLoad()
	{
		// If no GPU upload step is necessary, no override is necessary
		return true;
	}

	bool Resource::Save(Serializer &dest) const
	{
		URHO3D_LOGERROR("Save not supported for " + GetTypeName());
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
		// Todo, in use ??
		if(Refs() > 1)
		{
			useTimer_.Reset();
			return 0;
		}
		else
			return useTimer_.GetMSec(false);
	}

	void ResourceWithMetadata::AddMetadata(const String &name, const Variant &value)
	{
		bool exists;
		metadata_.Insert(MakePair(StringHash(name), value), exists);
		if(!exists)
			metadataKeys_.Push(name);
	}

	void ResourceWithMetadata::RemoveMetadata(const String &name)
	{
		metadata_.Erase(StringHash(name));
		metadataKeys_.Remove(name);
	}

	void ResourceWithMetadata::RemoveAllMetadata()
	{
		metadata_.Clear();
		metadataKeys_.Clear();
	}

	const Variant &ResourceWithMetadata::GetMetadata(const String &name) const
	{
		//todo error ??
		const Variant* value = metadata_[StringHash(name)];
		return value ? *value : Variant::EMPTY;
	}

	bool ResourceWithMetadata::HasMetadata() const
	{
		return !metadata_.Empty();
	}

	void ResourceWithMetadata::LoadMetadataFromXML(const XMLElement &source)
	{

	}

	void ResourceWithMetadata::SaveMetadataToXML(XMLElement &destination) const
	{

	}

	void ResourceWithMetadata::CopyMetadata(const ResourceWithMetadata &source)
	{
		metadata_ = source.metadata_;
		metadataKeys_ = source.metadataKeys_;
	}
}


