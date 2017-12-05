//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_RESOURCECACHE_H
#define URHO3DCOPY_RESOURCECACHE_H

#include "../Container/HashMap.h"
#include "../Math/StringHash.h"
#include "../Container/Ptr.h"
#include "../Core/Object.h"

namespace Urho3D
{
	class BackgroundLoader;
	class FileWatcher;
	class PackageFile;

	static const unsigned PRIORITY_LAST = 0xffffffff;

	struct ResourceGroup
	{
		ResourceGroup() :
			memoryBudget_(0),
			memoryUse_(0)
		{
		}
		unsigned long long memoryBudget_;
		unsigned long long memoryUse_;
		HashMap<StringHash, SharedPtr<Resource> > resources_;
	};

	enum ResourceRequest
	{
		RESOURCE_CHECKEXISTS = 0,
		RESOURCE_GETFILE = 1
	};

	class ResourceLoader : public Object
	{
	public:
		ResourceLoader(Context *context) :
				Object(context)
		{
		}

		virtual void Route(String& name, ResourceRequest requestType) = 0;
	};

	class ResourceCache : public Object
	{
	public:
		ResourceCache(Context* context);
		virtual ~ResourceCache() override ;

	private:
		const SharedPtr<Resource>& FindResource(StringHash type, StringHash nameHash);
		//todo
	};

}



#endif //URHO3DCOPY_RESOURCECACHE_H
