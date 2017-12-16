//
// Created by LIUHAO on 2017/12/5.
//

#include "ResourceCache.h"
#include "../Core/CoreEvent.h"
#include "Image.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Core/StringUtils.h"
#include "../Core/Context.h"
#include "ResourceEvents.h"

namespace Urho3D
{

	static const char* checkDirs[] = {
		"Fonts",
		"Materials",
		"Models",
		"Music",
		"Objects",
		"Particle",
		"PostProcess",
		"RenderPaths",
		"Scenes",
		"Scripts",
		"Sounds",
		"Shaders",
		"Techniques",
		"Textures",
		"UI",
		nullptr
	};

	static const SharedPtr<Resource> noResource;

	ResourceCache::ResourceCache(Context *context) :
			Object(context),
			autoReloadResources_(false),
			returnFailedResources_(false),
			searchPackagesFirst_(true),
			isRouting_(false),
			finishBackgroundResourcesMs_(5)
	{
		RegisterResourceLibrary(context_);
#ifdef URHO3D_THREADING
		//todo
		backgroundLoader_ = new BackgroundLoader(this);
#endif

		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(ResourceCache, HandleBeginFrame));
	}

	ResourceCache::~ResourceCache()
	{
#ifdef URHO3D_THREADING
		//todo
		backgroundLoader_.Reset();
#endif
	}

	bool ResourceCache::AddResourceDir(const String &pathName, unsigned int priority)
	{
		MutexLock lock(resourceMutex_);
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if(!fileSystem || !fileSystem->DirExists(pathName))
		{
			URHO3D_LOGERROR("Could not open directory " + pathName);
			return false;
		}

		String fixedPath = SanitateResourceDirName(pathName);
		for(unsigned i=0; i< resourceDirs_.Size(); ++i)
		{
			if(!resourceDirs_[i].Compare(fixedPath, false))
				return true;
		}

		if(priority < resourceDirs_.Size())
			resourceDirs_.Insert(priority, fixedPath);
		else
			resourceDirs_.Push(fixedPath);

		if(autoReloadResources_)
		{
			SharedPtr<FileWatcher> watcher(new FileWatcher(context_));
			watcher->StartWatching(fixedPath, true);
			fileWatchers_.Push(watcher);
		}

		URHO3D_LOGINFO("Added resource path " + fixedPath);
		return true;
	}

	bool ResourceCache::AddPackageFile(PackageFile *package, unsigned int priority)
	{
		MutexLock lock(resourceMutex_);

		if(!package || !package->GetNumFiles())
		{
			URHO3D_LOGERRORF("Could not add package file %s due to load failure", package->GetName().CString());
			return false;
		}

		if(priority < packages_.Size())
			packages_.Insert(priority, SharedPtr<PackageFile>(package));
		else
			packages_.Push(SharedPtr<PackageFile>(package));
		URHO3D_LOGINFO("Added resource package " + package->GetName());
		return true;
	}

	bool ResourceCache::AddPackageFile(const String &fileName, unsigned int priority)
	{
		//todo
	}

	bool ResourceCache::AddManualResource(Resource *resource)
	{
		if(!resource)
		{
			URHO3D_LOGERROR("Null manual resource");
			return false;
		}

		const String& name = resource->GetName();
		if(name.Empty())
		{
			URHO3D_LOGERROR("Manaual resource with empty name, can not add");
			return false;
		}
		resource->ResetUseTimer();
		resourceGroups_[resource->GetType()].resources_[resource->GetNameHash()] = resource;
		UpdateResourceGroup(resource->GetType());
		return true;
	}

	void ResourceCache::RemoveResourceDir(const String &pathName)
	{
		MutexLock lock(resourceMutex_);
		String fixedPath = SanitateResourceDirName(pathName);
		for(unsigned i = 0; i< resourceDirs_.Size(); ++i)
		{
			if(!resourceDirs_[i].Compare(fixedPath, false))
			{
				resourceDirs_.Erase(i);
				for(unsigned k= 0; k<fileWatchers_.Size(); ++k)
				{
					if(!fileWatchers_[k]->GetPath().Compare(fixedPath, false))
					{
						fileWatchers_.Erase(k);
						break;
					}
				}
				URHO3D_LOGINFO("Removed resource path " + fixedPath);
				return;
			}
		}
	}

	void ResourceCache::RemovePackageFile(PackageFile *package, bool releaseResources, bool forceRelease)
	{
		MutexLock lock(resourceMutex_);

		for(auto it = packages_.Begin(); it != packages_.End(); ++it)
		{
			if(*it == package)
			{
				if(releaseResources)
					ReleasePackageResources(package, forceRelease);
				URHO3D_LOGINFO("Removed resource package " + package->GetName());
				packages_.Erase(it);
				return;
			}
		}
	}

	void ResourceCache::RemovePackageFile(const String &fileName, bool releaseResources, bool forceRelease)
	{
		MutexLock lock(resourceMutex_);
		String fileNameNoPath = GetFileNameAndExtension(fileName);
		for(auto it = packages_.Begin(); it != packages_.End(); ++it)
		{
			if(!GetResourceFileName((*it)->GetName()).Compare(fileNameNoPath, false))
			{
				if(releaseResources)
					ReleasePackageResources(*it, forceRelease);
				URHO3D_LOGINFO("Removed resource package " + fileNameNoPath);
				return;
			}
		}
	}

	void ResourceCache::ReleaseResource(StringHash type, const String &name, bool force)
	{
		StringHash nameHash(name);
		const SharedPtr<Resource>& existingRes = FindResource(type, nameHash);
		if(!existingRes)
			return;

		if((existingRes.Refs() == 1 && existingRes.WeakRefs() == 0) || force)
		{
			resourceGroups_[type].resources_.Erase(nameHash);
			UpdateResourceGroup(type);
		}
	}

	void ResourceCache::ReleaseResources(StringHash type, bool force)
	{

	}

	void ResourceCache::ReleaseResources(StringHash type, const String &partialName, bool force)
	{

	}

	void ResourceCache::ReloadResource(Resource *resource)
	{

	}

	void ResourceCache::ReloadResourceWithDependencies(const String &fileName)
	{

	}

	void ResourceCache::SetMemoryBudget(StringHash type, unsigned long long budget)
	{

	}

	void ResourceCache::AddResourceRouter(ResourceRouter *router, bool addAsFirst)
	{

	}

	void ResourceCache::RemoveResourceRouter(ResourceRouter *router)
	{

	}

	SharedPtr<File> ResourceCache::GetFile(const String &name, bool sendEventOnFailure)
	{
		return SharedPtr<File>();
	}

	Resource *ResourceCache::GetResource(StringHash type, const String &nameIn, bool sendEventOnFialure)
	{
		String name = SanitateResourceName(nameIn);
		//Todo, why need check this ??
		//https://discourse.urho3d.io/t/why-does-urho-load-resource-only-in-the-main-thread/3852/1
		if(!Thread::IsMainThread())
		{
			URHO3D_LOGERROR("Attempted to get resource " + name + " from outside the main thread");
			return nullptr;
		}

		if(name.Empty())
			return nullptr;

		StringHash nameHash(name);
#ifdef URHO3D_THREADING
		backgroundLoader_->WaitForResource(type, nameHash);
#endif
		const SharedPtr<Resource>& existing = FindResource(type, nameHash);
		//todo , 这里有问题，隐式转换为bool类型吗？？？
		if(existing)
			return existing;
		SharedPtr<Resource> resource;
		resource = DynamicCast<Resource>(context_->CreateObject(type));
		//todo, the same issue as above
		if(!resource)
		{
			URHO3D_LOGERROR("Could not load unknow resource type " + String(type));
			if(sendEventOnFialure)
			{
				using namespace UnknowResourceType;
				VariantMap& eventMap = GetEventDataMap();
				eventMap[P_RESOURCETYPE] = type;
				SendEvent(E_UNKNOWRESOURCETYPE, eventMap);
			}
			return nullptr;
		}

		SharedPtr<File> file = GetFile(name, sendEventOnFialure);
		if(!file)
			return nullptr;

		URHO3D_LOGDEBUG("Loading resource " + name);
		resource->SetName(name);

		if(resource->Load(*file.Get()))
		{
			if(sendEventOnFialure)
			{
				using namespace LoadFailed;
				VariantMap& eventMap = GetEventDataMap();
				eventMap[P_RESOURCENAME] = name;
				SendEvent(E_LOADFAILED, eventMap);
			}

			if(!returnFailedResources_)
				return nullptr;
		}

		resource->ResetUseTimer();
		resourceGroups_[type].resources_[nameHash] = resource;
		UpdateResourceGroup(type);
		return resource;
	}



	SharedPtr<Resource> ResourceCache::GetTempResource(StringHash type, const String &name, bool sendEventOnFailure)
	{
		return SharedPtr<Resource>();
	}

	bool ResourceCache::BackgroundLoadResource(StringHash type, const String &name, bool sendEventOnFailure,
	                                           Resource *caller)
	{
		return false;
	}

	const SharedPtr<Resource> &ResourceCache::FindResource(StringHash type, StringHash nameHash)
	{
		MutexLock lock(resourceMutex_);

		auto it = resourceGroups_.Find(type);
		if(it == resourceGroups_.End())
			return noResource;
		auto i = it->second_.resources_.Find(nameHash);
		if(i == (*it).second_.resources_.End())
			return noResource;
		return i->second_;
	}

	const SharedPtr<Resource> &ResourceCache::FindResource(StringHash nameHash)
	{
		return <#initializer#>;
	}

	void ResourceCache::ReleasePackageResources(PackageFile *package, bool force)
	{

	}

	void ResourceCache::UpdateResourceGroup(StringHash type)
	{
		auto it = resourceGroups_.Find(type);
		//todo
	}

	void ResourceCache::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{

	}

	File *ResourceCache::SearchResourceDirs(const String &nameIn)
	{
		return nullptr;
	}

	File *ResourceCache::SearchPackages(const String &nameIn)
	{
		return nullptr;
	}

	void ResourceCache::GetResources(PODVector<Resource *> &result, StringHash type) const
	{
		result.Clear();
		auto it = resourceGroups_.Find(type);
		if(it != resourceGroups_.End())
		{
			for(auto i = it->second_.resources_.Begin(); i != it->second_.resources_.End(); ++i)
			{
				result.Push((*i).second_.Get());
			}
		}
	}

	Resource *ResourceCache::GetExistingResource(StringHash type, const String &name)
	{
		return nullptr;
	}

	template<class T>
	T *ResourceCache::GetResource(const String &name, bool sendEventOnFailure)
	{
		return nullptr;
	}

	template<class T>
	T *ResourceCache::GetExistingResource(const String &name)
	{
		return nullptr;
	}

	template<class T>
	SharedPtr<T> ResourceCache::GetTempResource(const String &name, bool sendEventOnFailure)
	{
		return SharedPtr<T>();
	}

	template<class T>
	void ResourceCache::ReleaseResource(const String &name, bool force)
	{

	}

	template<class T>
	bool ResourceCache::BackgroundLoadResource(const String &name, bool sendEventOnFailure, Resource *caller)
	{
		return 0;
	}

	template<class T>
	void ResourceCache::GetResources(PODVector<T *> &result) const
	{

	}

	bool ResourceCache::Exists(const String &name) const
	{
		return false;
	}

	unsigned long long ResourceCache::GetMemoryBudget(StringHash type) const
	{
		return 0;
	}

	unsigned long long ResourceCache::GetMemroyUse(StringHash type) const
	{
		return 0;
	}

	unsigned long long ResourceCache::GetTotalMemoryUse() const
	{
		return 0;
	}

	String ResourceCache::GetResourceFileName(const String &name) const
	{
		return String();
	}

	ResourceRouter *ResourceCache::GetResourceRouter(unsigned index) const
	{
		return nullptr;
	}

	String ResourceCache::GetPreferredResourceDir(const String &path) const
	{
		return String();
	}

	String ResourceCache::SanitateResourceName(const String &name) const
	{
		return String();
	}

	String ResourceCache::SanitateResourceDirName(const String &name) const
	{
		String fixedPath = AddTrailingSlash(name);
		if(!IsAbsolutePath(fixedPath))
			fixedPath = GetSubsystem<FileSystem>()->GetCurrentDir() + fixedPath;

		fixedPath.Replace("/./", "/");
		return fixedPath.Trimmed();
	}

	void ResourceCache::StoreResourceDependency(Resource *resource, const String &dependency)
	{

	}

	void ResourceCache::ResetDependencies(Resource *resource)
	{

	}

	String ResourceCache::PrintMemoryUsage() const
	{
		return String();
	}

	//todo

	void RegisterResourceLibrary(Context* context)
	{
		Image::RegisterObject(context);
		JSONFile::RegisterObject(context);
		//todo
		XMLFile::RegisterObject(context);
	}
}

