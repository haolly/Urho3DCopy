//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_RESOURCECACHE_H
#define URHO3DCOPY_RESOURCECACHE_H

#include "../Container/HashMap.h"
#include "../Math/StringHash.h"
#include "../Container/Ptr.h"
#include "../Core/Object.h"
#include "Resource.h"
#include "../IO/File.h"
#include "../Core/Mutex.h"
#include "../Container/HashSet.h"
#include "../IO/FileWatcher.h"

namespace Urho3D
{
	class BackgroundLoader;
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
		//key is name hash
		HashMap<StringHash, SharedPtr<Resource> > resources_;
	};

	enum ResourceRequest
	{
		RESOURCE_CHECKEXISTS = 0,
		RESOURCE_GETFILE = 1
	};

	class ResourceRouter : public Object
	{
	public:
		ResourceRouter(Context* context) :
			Object(context)
		{
		}

		//todo, how to route
		virtual void Route(String& name, ResourceRequest requestType) = 0;
	};

	class ResourceCache : public Object
	{
		URHO3D_OBJECT(ResourceCache, Object);
	public:
		ResourceCache(Context* context);
		virtual ~ResourceCache() override ;

		bool AddResourceDir(const String& pathName, unsigned priority = PRIORITY_LAST);
		bool AddPackageFile(PackageFile* package, unsigned priority = PRIORITY_LAST);
		bool AddPackageFile(const String& fileName, unsigned priority = PRIORITY_LAST);

		bool AddManualResource(Resource* resource);
		void RemoveResourceDir(const String& pathName);
		void RemovePackageFile(PackageFile* package, bool releaseResources = true, bool forceRelease = false);
		void RemovePackageFile(const String& fileName, bool releaseResources = true, bool forceRelease = false);

		void ReleaseResource(StringHash type, const String& name, bool force = false);
		void ReleaseResources(StringHash type, bool force = false);
		void ReleaseResources(StringHash type, const String& partialName, bool force = false);
		void ReleaseResources(const String& partialName, bool force = false);
		void ReleaseAllResources(bool force = false);
		bool ReloadResource(Resource* resource);
		void ReloadResourceWithDependencies(const String& fileName);
		void SetMemoryBudget(StringHash type, unsigned long long budget);
		void SetAutoReloadResources(bool enable);
		void SetReturnFailedResources(bool enable)
		{
			returnFailedResources_ = enable;
		}

		void SetSearchPackagesFirst(bool value)
		{
			searchPackagesFirst_ = value;
		}

		void SetFinishBackgroundResourcesMs(int ms)
		{
			finishBackgroundResourcesMs_ = ms;
		}

		//Todo, usage
		void AddResourceRouter(ResourceRouter* router, bool addAsFirst = false);
		void RemoveResourceRouter(ResourceRouter* router);

		SharedPtr<File> GetFile(const String& name, bool sendEventOnFailure = true);
		Resource* GetResource(StringHash type, const String& name, bool sendEventOnFialure = true);
		SharedPtr<Resource> GetTempResource(StringHash type, const String& name, bool sendEventOnFailure = true);
		bool BackgroundLoadResource(StringHash type, const String& name, bool sendEventOnFailure = true, Resource* caller = nullptr);

		void GetResources(PODVector<Resource*>& result, StringHash type) const;
		Resource* GetExistingResource(StringHash type, const String& name);

		const HashMap<StringHash, ResourceGroup>& GetAllResources() const { return resourceGroups_; };
		const Vector<String>& GetResourceDirs() const { return resourceDirs_; }
		const Vector<SharedPtr<PackageFile> >& GetPackageFiles() const { return packages_; }

		//Note, those template function should be defined in .h file
		template <class T> T* GetResource(const String& name, bool sendEventOnFailure = true);
		template <class T> T* GetExistingResource(const String& name);
		template <class T> SharedPtr<T> GetTempResource(const String& name, bool sendEventOnFailure = true);
		template <class T> void ReleaseResource(const String& name, bool force = false);
		template <class T> bool BackgroundLoadResource(const String& name, bool sendEventOnFailure = true, Resource* caller = nullptr);
		template <class T> void GetResources(PODVector<T *> &result) const;

		bool Exists(const String& name) const;
		unsigned long long GetMemoryBudget(StringHash type) const;
		unsigned long long GetMemoryUse(StringHash type) const;
		unsigned long long GetTotalMemoryUse() const;

		String GetResourceFileName(const String& name) const;

		bool GetAutoReloadResources() const { return autoReloadResources_; }
		bool GetReturnFailedResources() const { return returnFailedResources_; }
		bool GetSearchPackagesFirst() const { return searchPackagesFirst_; }
		int GetFinishBackgroundResourcesMs() const { return finishBackgroundResourcesMs_; }

		ResourceRouter* GetResourceRouter(unsigned index) const;

		String GetPreferredResourceDir(const String& path) const;
		String SanitateResourceName(const String& name) const;
		String SanitateResourceDirName(const String &name) const;
		void StoreResourceDependency(Resource* resource, const String& dependency);
		void ResetDependencies(Resource* resource);

		String PrintMemoryUsage() const;

	private:
		const SharedPtr<Resource>& FindResource(StringHash type, StringHash nameHash);
		const SharedPtr<Resource>& FindResource(StringHash nameHash);

		void ReleasePackageResources(PackageFile* package, bool force = false);
		void UpdateResourceGroup(StringHash type);
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

		File* SearchResourceDirs(const String& nameIn);
		File* SearchPackages(const String& nameIn);

		//Note, mutable keyword allow const member function to modify it's value
		mutable Mutex resourceMutex_;
		HashMap<StringHash, ResourceGroup> resourceGroups_;
		Vector<String> resourceDirs_;
		Vector<SharedPtr<FileWatcher> > fileWatchers_;
		Vector<SharedPtr<PackageFile> > packages_;

		//The resource(key) those (value) resource all dependents on it
		HashMap<StringHash, HashSet<StringHash> > dependentResouces_;
		SharedPtr<BackgroundLoader> backgroundLoader_;
		Vector<SharedPtr<ResourceRouter> > resourceRouters_;

		bool autoReloadResources_;
		bool returnFailedResources_;
		bool searchPackagesFirst_;
		mutable bool isRouting_;
		int finishBackgroundResourcesMs_;
	};

	//Note, template function should be defined in .h file
	template <class T>
	T* ResourceCache::GetExistingResource(const String &name)
	{
		StringHash type = T::GetTypeStatic();
		return static_cast<T*>(GetExistingResource(type, name));
	}

	template <class T>
	T* ResourceCache::GetResource(const String &name, bool sendEventOnFailure)
	{
		StringHash type = T::GetTypeStatic();
		return static_cast<T*>(GetResource(type, name, sendEventOnFailure));
	}

	template <class T>
	void ResourceCache::ReleaseResource(const String &name, bool force)
	{
		StringHash type = T::GetTypeStatic();
		ReleaseResource(type, name, force);
	}

	template <class T>
	SharedPtr<T> ResourceCache::GetTempResource(const String &name, bool sendEventOnFailure)
	{
		StringHash type = T::GetTypeStatic();
		return StaticCast<T>(GetTempResource(type, name, sendEventOnFailure));
	}

	template <class T>
	bool ResourceCache::BackgroundLoadResource(const String &name, bool sendEventOnFailure, Resource *caller)
	{
		StringHash type = T::GetTypeStatic();
		return BackgroundLoadResource(type, name, sendEventOnFailure, caller);
	}

	template <class T>
	void ResourceCache::GetResources(PODVector<T *> &result) const
	{
		PODVector<Resource*>& resources = reinterpret_cast<PODVector<Resource*>&>(result);
		StringHash type = T::GetTypeStatic();
		GetResources(resources, type);

		for(unsigned i = 0; i< resources.Size(); ++i)
		{
			Resource* resource = resources[i];
			result[i] = static_cast<T*>(resource);
		}
	}

	void RegisterResourceLibrary(Context* context);
}

#endif //URHO3DCOPY_RESOURCECACHE_H
