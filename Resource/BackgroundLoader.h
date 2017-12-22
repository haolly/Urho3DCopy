//
// Created by liuhao1 on 2017/12/21.
//

#ifndef URHO3DCOPY_BACKGROUNDLOADER_H
#define URHO3DCOPY_BACKGROUNDLOADER_H

#include "../Container/RefCounted.h"
#include "../Core/Thread.h"
#include "../Container/Ptr.h"
#include "../Math/StringHash.h"
#include "../Container/Pair.h"
#include "../Container/HashSet.h"
#include "../Core/Mutex.h"
#include "../Container/HashMap.h"

namespace Urho3D
{
	class Resource;
	class ResourceCache;

	struct BackgroundLoadItem
	{
		SharedPtr<Resource> resource_;
		// I need you
		HashSet<Pair<StringHash, StringHash> > dependencies_;
		// You need I
		HashSet<Pair<StringHash, StringHash> > dependents_;
		bool sendEventOnFailure_;
	};

	class BackgroundLoader : public RefCounted, public Thread
	{
	public:
		BackgroundLoader(ResourceCache* owner);
		virtual ~BackgroundLoader() override ;
		virtual void ThreadFunction() override ;
		bool QueueResource(StringHash type, const String& name, bool sendEventOnFailure, Resource* caller);
		void WaitForResource(StringHash type, StringHash nameHash);
		void FinishResources(int maxMs);
		unsigned GetNumQueuedResources() const;

	private:
		void FinishBackgroundLoading(BackgroundLoadItem& item);

		ResourceCache* owner_;
		mutable Mutex backgroundLoadMutex_;
		// manipulated by main thread and this thread
		HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem> backgroundLoadQueue_;
	};
}

#endif //URHO3DCOPY_BACKGROUNDLOADER_H
