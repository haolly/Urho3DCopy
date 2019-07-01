//
// Created by liuhao1 on 2017/12/21.
//

#include "BackgroundLoader.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "../Core/Context.h"
#include "../IO/Log.h"
#include "ResourceEvents.h"

namespace Urho3D
{

	BackgroundLoader::BackgroundLoader(ResourceCache *owner) :
		owner_(owner)
	{
	}

	BackgroundLoader::~BackgroundLoader()
	{
		MutexLock lock(backgroundLoadMutex_);
		backgroundLoadQueue_.Clear();
	}

	//Note, 工作线程不会从队列中移除任务，只会标记任务的状态，主线程根据任务状态来修改任务队列
	void BackgroundLoader::ThreadFunction()
	{
		while (shouldRun_)
		{
			backgroundLoadMutex_.Acquire();

			auto iter = backgroundLoadQueue_.Begin();
			while(iter != backgroundLoadQueue_.End())
			{
				if(iter->second_.resource_->GetAsyncLoadState() == ASYNC_QUEUED)
					break;
				else
					++iter;
			}

			if(iter == backgroundLoadQueue_.End())
			{
				backgroundLoadMutex_.Release();
				Time::Sleep(5);
			}
			else
			{
				BackgroundLoadItem& item = iter->second_;
				Resource* resource = item.resource_;

				backgroundLoadMutex_.Release();

				bool success = false;
				SharedPtr<File> file = owner_->GetFile(resource->GetName(), item.sendEventOnFailure_);
				if(file)
				{
					resource->SetAsyncLoadState(ASYNC_LOADING);
					success = resource->BeginLoad(*file);
				}

				//Clear dependencies which dependents on this resource
				auto thisKey = MakePair(resource->GetType(), resource->GetNameHash());
				backgroundLoadMutex_.Acquire();
				if(item.dependents_.Size())
				{
					for(auto iter_dep = item.dependents_.Begin(); iter_dep != item.dependents_.End(); ++iter_dep)
					{
						auto it = backgroundLoadQueue_.Find(*iter_dep);
						if(it != backgroundLoadQueue_.End())
							it->second_.dependencies_.Erase(thisKey);
					}
					item.dependents_.Clear();
				}

				resource->SetAsyncLoadState(success ? ASYNC_SUCCESS : ASYNC_FAIL);
				backgroundLoadMutex_.Release();
			}
		}
	}

	bool BackgroundLoader::QueueResource(StringHash type, const String &name, bool sendEventOnFailure, Resource *caller)
	{
		StringHash nameHash(name);
		Pair<StringHash, StringHash> key = MakePair(type, nameHash);

		MutexLock lock(backgroundLoadMutex_);

		if(backgroundLoadQueue_.Find(key) != backgroundLoadQueue_.End())
			return false;

		BackgroundLoadItem& item = backgroundLoadQueue_[key];
		item.sendEventOnFailure_ = sendEventOnFailure;

		item.resource_ = DynamicCast<Resource>(owner_->GetContext()->CreateObject(type));
		if(!item.resource_)
		{
			URHO3D_LOGERROR("Could not load unknow resource type " + String(type));
			if(sendEventOnFailure && Thread::IsMainThread())
			{
				using namespace UnknowResourceType;
				VariantMap& eventData = owner_->GetEventDataMap();
				eventData[P_RESOURCETYPE] = type;
				owner_->SendEvent(E_UNKNOWRESOURCETYPE, eventData);
			}

			backgroundLoadQueue_.Erase(key);
			return false;
		}

		URHO3D_LOGDEBUG("Background Loading resource: " + name);

		item.resource_->SetName(name);
		item.resource_->SetAsyncLoadState(ASYNC_QUEUED);

		// If this is a resource calling fro the background load of more resources, mark the dependency as necessary
		if(caller)
		{
			Pair<StringHash, StringHash> callerKey = MakePair(caller->GetType(), caller->GetNameHash());
			auto iter = backgroundLoadQueue_.Find(callerKey);
			if(iter != backgroundLoadQueue_.End())
			{
				BackgroundLoadItem& callerItem = iter->second_;
				callerItem.dependencies_.Insert(key);
				item.dependents_.Insert(callerKey);
			}
			else
				URHO3D_LOGWARNING("Resource " + caller->GetName() + " requested for a background loaded resource, "
						         "but itself was not in the background load queue");
		}

		if(!IsStarted())
			Run();

		return true;
	}

	void BackgroundLoader::WaitForResource(StringHash type, StringHash nameHash)
	{
		backgroundLoadMutex_.Acquire();
		auto key = MakePair(type, nameHash);
		auto iter = backgroundLoadQueue_.Find(key);
		if(iter != backgroundLoadQueue_.End())
		{
			backgroundLoadMutex_.Release();
			{
				Resource *resource = iter->second_.resource_;
				HiresTimer waitTimer;
				bool didWait = false;
				for (;;) {
					unsigned numDeps = iter->second_.dependencies_.Size();
					AsyncLoadState loadState = resource->GetAsyncLoadState();
					if (numDeps > 0 || loadState == ASYNC_LOADING || loadState == ASYNC_QUEUED) {
						didWait = true;
						Time::Sleep(1);
					} else
						break;
				}
				if (didWait)
					URHO3D_LOGDEBUG(
							"Waited " + String(waitTimer.GetUSec(false) / 1000) + " ms for background loaded resource " +
							resource->GetName());
			}
			FinishBackgroundLoading(iter->second_);

			backgroundLoadMutex_.Acquire();
			backgroundLoadQueue_.Erase(iter);
			backgroundLoadMutex_.Release();
		}
		else
			backgroundLoadMutex_.Release();
	}

	//Note, need to be called in the main thread
	void BackgroundLoader::FinishResources(int maxMs)
	{
		if(IsStarted())
		{
			HiresTimer timer;
			backgroundLoadMutex_.Acquire();

			for(auto iter = backgroundLoadQueue_.Begin(); iter != backgroundLoadQueue_.End();)
			{
				Resource* resource = iter->second_.resource_;
				auto state = resource->GetAsyncLoadState();
				unsigned numDeps = iter->second_.dependencies_.Size();
				if(numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
					++iter;
				else
				{
					backgroundLoadMutex_.Release();
					FinishBackgroundLoading(iter->second_);
					backgroundLoadMutex_.Acquire();
					iter = backgroundLoadQueue_.Erase(iter);
				}

				if(timer.GetUSec(false) >= maxMs * 1000)
					break;
			}
		}

		backgroundLoadMutex_.Release();
	}

	unsigned BackgroundLoader::GetNumQueuedResources() const
	{
		MutexLock lock(backgroundLoadMutex_);
		return backgroundLoadQueue_.Size();
	}

	//Note, need to be called in the main thread
	void BackgroundLoader::FinishBackgroundLoading(BackgroundLoadItem &item)
	{
		Resource* resource = item.resource_;
		bool success = resource->GetAsyncLoadState() == ASYNC_SUCCESS;
		if(success)
		{
			URHO3D_LOGDEBUG("Finishing background loaded resource " + resource->GetName());
			success = resource->EndLoad();
		}
		resource->SetAsyncLoadState(ASYNC_DONE);

		if(!success && item.sendEventOnFailure_)
		{
			using namespace LoadFailed;
			VariantMap& eventData = owner_->GetEventDataMap();
			eventData[P_RESOURCENAME] = resource->GetName();
			owner_->SendEvent(E_LOADFAILED, eventData);
		}

		//Note, store to the cache just before sending to event; use the same mechnism as for normal reosurces
		if(success || owner_->GetReturnFailedResources())
		{
			owner_->AddManualResource(resource);
		}
		{
			using namespace ResourceBackgroundLoaded;
			VariantMap& eventData = owner_->GetEventDataMap();
			eventData[P_RESOURCENAME] = resource->GetName();
			eventData[P_SUCCESS] = success;
			eventData[P_RESOURCE] = resource;
			owner_->SendEvent(E_RESOURCEBACKGROUNDLOADED, eventData);
		}
	}
}