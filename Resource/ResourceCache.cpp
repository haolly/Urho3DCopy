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
#include "XMLFile.h"
#include "../IO/PackageFile.h"
#include "BackgroundLoader.h"

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
		SharedPtr<PackageFile> package(new PackageFile(context_));
		return package->Open(fileName) && AddPackageFile(package);
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
		bool released = false;

		auto iter = resourceGroups_.Find(type);
		if(iter != resourceGroups_.End())
		{
			for(auto iter_res = iter->second_.resources_.Begin(); iter_res != iter->second_.resources_.End(); )
			{
				if(iter_res->second_.Refs() == 1 && iter_res->second_.WeakRefs() == 0 || force)
				{
					iter_res = iter->second_.resources_.Erase(iter_res);
					released = true;
				}
				else
					++iter_res;
			}
		}
		if(released)
			UpdateResourceGroup(type);
	}

	void ResourceCache::ReleaseResources(StringHash type, const String &partialName, bool force)
	{
		bool released = false;
		auto it = resourceGroups_.Find(type);
		if(it != resourceGroups_.End())
		{
			for(auto iter_res = it->second_.resources_.Begin(); iter_res != it->second_.resources_.End();)
			{
				if(iter_res->second_->GetName().Contains(partialName))
				{
					if((iter_res->second_.Refs() == 1 && iter_res->second_.WeakRefs() == 0) || force)
					{
						iter_res = it->second_.resources_.Erase(iter_res);
						released = true;
					}
					else
						++iter_res;
				}
				else
					++iter_res;
			}
		}
		if(released)
			UpdateResourceGroup(type);
	}

	void ResourceCache::ReleaseAllResources(bool force)
	{
		unsigned repeat = force ? 1 : 2;

		while (repeat--)
		{
			for(auto it = resourceGroups_.Begin(); it != resourceGroups_.End(); ++it)
			{
				bool released = false;
				for(auto iter_res = it->second_.resources_.Begin(); iter_res != it->second_.resources_.End(); )
				{
					if(iter_res->second_.Refs() == 1 && iter_res->second_.WeakRefs() == 0 || force)
					{
						iter_res = it->second_.resources_.Erase(iter_res);
						released = true;
					}
					else
						++iter_res;
				}
				if(released)
					UpdateResourceGroup(it->first_);
			}
		}
	}

	bool ResourceCache::ReloadResource(Resource *resource)
	{
		if(!resource)
			return false;

		resource->SendEvent(E_RELOADSTARTED);
		bool success = false;
		SharedPtr<File> file = GetFile(resource->GetName());
		if(file)
			success = resource->Load(*file.Get());

		if(success)
		{
			resource->ResetUseTimer();
			UpdateResourceGroup(resource->GetType());
			return true;
		}

		resource->SendEvent(E_RELOADFAILED);
	}

	void ResourceCache::ReloadResourceWithDependencies(const String &fileName)
	{
		StringHash fileNameHash(fileName);
		const SharedPtr<Resource>& resource = FindResource(fileNameHash);

		if(resource)
		{
			URHO3D_LOGDEBUG("Reloading changed resource " + fileName);
			ReloadResource(resource);
		}

		if(!resource || GetExtension(resource->GetName()) == ".xml")
		{
			auto iter = dependentResouces_.Find(fileNameHash);
			if(iter != dependentResouces_.End())
			{
				Vector<SharedPtr<Resource> > dependents;
				dependents.Reserve(iter->second_.Size());

				for(auto iter_dep = iter->second_.Begin(); iter_dep != iter->second_.End(); ++iter_dep)
				{
					const SharedPtr<Resource>& dependent = FindResource(*iter_dep);
					if(dependent)
						dependents.Push(dependent);
				}

				for(unsigned k = 0; k< dependents.Size(); ++k)
				{
					URHO3D_LOGDEBUG("Reloading resource " + dependents[k]->GetName() + " depending on " + fileName);
					ReloadResource(dependents[k]);
				}
			}
		}
	}

	void ResourceCache::SetMemoryBudget(StringHash type, unsigned long long budget)
	{
		resourceGroups_[type].memoryBudget_ = budget;
	}

	void ResourceCache::SetAutoReloadResources(bool enable)
	{
		if(enable != autoReloadResources_)
		{
			if(enable)
			{
				for(unsigned i=0; i < resourceDirs_.Size(); ++i)
				{
					SharedPtr<FileWatcher> watcher(new FileWatcher(context_));
					watcher->StartWatching(resourceDirs_[i], true);
					fileWatchers_.Push(watcher);
				}
			}
			else
				fileWatchers_.Clear();

			autoReloadResources_ = enable;
		}
	}

	void ResourceCache::AddResourceRouter(ResourceRouter *router, bool addAsFirst)
	{
		SharedPtr<ResourceRouter> routerPtr(router);
		if(resourceRouters_.Find(routerPtr) != resourceRouters_.End())
			return;

		if(addAsFirst)
			resourceRouters_.Insert(0, SharedPtr<ResourceRouter>(router));
		else
			resourceRouters_.Push(SharedPtr<ResourceRouter>(router));
	}

	void ResourceCache::RemoveResourceRouter(ResourceRouter *router)
	{
		auto iter = resourceRouters_.Find(router);
		if(iter != resourceRouters_.End())
		{
			resourceRouters_.Erase(iter);
			return;
		}
	}

	SharedPtr<File> ResourceCache::GetFile(const String &nameIn, bool sendEventOnFailure)
	{
		MutexLock lock(resourceMutex_);

		String name = SanitateResourceName(nameIn);
		if(!isRouting_)
		{
			isRouting_ = true;
			for(unsigned i =0; i< resourceRouters_.Size(); ++i)
			{
				resourceRouters_[i]->Route(name, RESOURCE_GETFILE);
			}
			isRouting_ = false;
		}

		//Todo, finished loading here ??
		//好像是没有load，直接去目录里面去找了，所以，上面的 Route 是干啥用的？
		if(name.Length())
		{
			File* file = nullptr;
			if(searchPackagesFirst_)
			{
				file = SearchPackages(name);
				if(!file)
					file = SearchResourceDirs(name);
			}
			else
			{
				file = SearchResourceDirs(name);
				if(!file)
					file = SearchPackages(name);
			}

			if(file)
				return SharedPtr<File>(file);
		}

		if(sendEventOnFailure)
		{
			if(resourceRouters_.Size() && name.Empty() && !nameIn.Empty())
				URHO3D_LOGERROR("Resource request " + nameIn + " was blocked");
			else
				URHO3D_LOGERROR("Could not find resource " + name);

			if(Thread::IsMainThread())
			{
				using namespace ResourceNotFound;
				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCENAME] = name.Length() ? name : nameIn;
				SendEvent(E_RESOURCENOTFOUND, eventData);
			}
		}
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
		//Note, if the resource is queued, wait for it loaded
		backgroundLoader_->WaitForResource(type, nameHash);
#endif
		const SharedPtr<Resource>& existing = FindResource(type, nameHash);
		//Note , 这里有问题，隐式转换为bool类型吗？？？
		//Converted to T* implicitly, so can check is is null
		//Ref "type conversion operator"
		if(existing)
			return existing;
		SharedPtr<Resource> resource;
		resource = DynamicCast<Resource>(context_->CreateObject(type));
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

	bool ResourceCache::BackgroundLoadResource(StringHash type, const String &nameIn, bool sendEventOnFailure,
	                                           Resource *caller)
	{
#ifdef URHO3D_THREADING
		String name = SanitateResourceName(nameIn);
		if(name.Empty())
			return false;

		StringHash nameHash(name);
		if(FindResource(nameHash))
			return false;
		return backgroundLoader_->QueueResource(type, name, sendEventOnFailure, caller);
#else
		return GetResource(type, nameIn, sendEventONFailure);
#endif
	}

	SharedPtr<Resource> ResourceCache::GetTempResource(StringHash type, const String &nameIn, bool sendEventOnFailure)
	{
		String name = SanitateResourceName(nameIn);
		if(name.Empty())
			return SharedPtr<Resource>();

		SharedPtr<Resource> resource;
		resource = DynamicCast<Resource>(context_->CreateObject(type));
		if(!resource)
		{
			URHO3D_LOGERROR("Could not load unknow resource type " + type);

			if(sendEventOnFailure)
			{
				using namespace UnknowResourceType;
				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCETYPE] = type;
				SendEvent(E_UNKNOWRESOURCETYPE, eventData);
			}

			return SharedPtr<Resource>();
		}

		SharedPtr<File> file = GetFile(name, sendEventOnFailure);

		if(!file)
			return SharedPtr<Resource>();

		URHO3D_LOGDEBUG("Loading temporary resource " + name);
		resource->SetName(file->GetName());
		if(!resource->Load(*(file.Get())))
		{
			if(sendEventOnFailure)
			{
				using namespace LoadFailed;
				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCENAME] = name;
				SendEvent(E_LOADFAILED, eventData);
			}
		}
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
		MutexLock lock(resourceMutex_);

		for(auto it = resourceGroups_.Begin(); it != resourceGroups_.End(); ++it)
		{
			auto ret = it->second_.resources_.Find(nameHash);
			if(ret != it->second_.resources_.End())
				return ret->second_;
		}
		return noResource;
	}

	void ResourceCache::ReleasePackageResources(PackageFile *package, bool force)
	{
		HashSet<StringHash> affectedGroups;
		const auto& enties = package->GetEntries();
		for(auto iter = enties.Begin(); iter != enties.End(); ++iter)
		{
			StringHash nameHash(iter->first_);
			for(auto iter_res = resourceGroups_.Begin(); iter_res != resourceGroups_.End(); ++iter_res)
			{
				auto existing = iter_res->second_.resources_.Find(nameHash);
				if(existing != iter_res->second_.resources_.End())
				{
					if((existing->second_.Refs() == 1 && existing->second_.WeakRefs() == 0) || force)
					{
						iter_res->second_.resources_.Erase(existing);
						affectedGroups.Insert(iter_res->first_);
					}
					break;
				}
			}
		}

		for(auto iter = affectedGroups.Begin(); iter != affectedGroups.End(); ++iter)
		{
			UpdateResourceGroup(*iter);
		}
	}

	void ResourceCache::UpdateResourceGroup(StringHash type)
	{
		auto it = resourceGroups_.Find(type);
		if(it == resourceGroups_.End())
			return;

		for(;;)
		{
			unsigned totalSize = 0;
			unsigned oldestTimer = 0;
			auto oldestResource = it->second_.resources_.End();

			for(auto iter = it->second_.resources_.Begin(); iter != it->second_.resources_.End();
					++iter)
			{
				totalSize += iter->second_->GetMemoryUse();
				unsigned useTimer = iter->second_->GetUseTimer();
				if(useTimer > oldestTimer)
				{
					oldestTimer = useTimer;
					oldestResource = iter;
				}
			}

			it->second_.memoryUse_ = totalSize;
			// If memory budget defined, and is exceeded, remove the oldest resource and loop agagin
			// Note, resources in use always return a zero timer and can not be removed
			if(it->second_.memoryBudget_ && it->second_.memoryUse_ > it->second_.memoryBudget_ &&
					oldestResource != it->second_.resources_.End())
			{
				URHO3D_LOGDEBUG("Resource group " + oldestResource->second_->GetTypeName() +
								" over memory budget, releasing resource " + oldestResource->second_->GetName());
				it->second_.resources_.Erase(oldestResource);
			}
			else
				break;
		}
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

	Resource *ResourceCache::GetExistingResource(StringHash type, const String &nameIn)
	{
		String name = SanitateResourceName(nameIn);

		if(!Thread::IsMainThread())
		{
			URHO3D_LOGERROR("Attemped to get resource " + name + " from outside the main thread");
			return nullptr;
		}
		if(name.Empty())
			return nullptr;

		StringHash nameHash(name);
		auto existing = FindResource(type, nameHash);
		return existing;
	}


	void ResourceCache::StoreResourceDependency(Resource *resource, const String &dependency)
	{
		if(!resource)
			return;

		MutexLock lock(resourceMutex_);

		StringHash nameHash(resource->GetName());
		HashSet<StringHash>& dependents = dependentResouces_[StringHash(dependency)];
		dependents.Insert(nameHash);
	}

	//Reset dependency for the resource
	void ResourceCache::ResetDependencies(Resource *resource)
	{
		if(!resource)
			return;

		MutexLock lock(resourceMutex_);

		StringHash nameHash(resource->GetName());
		for(auto iter = dependentResouces_.Begin(); iter != dependentResouces_.End(); )
		{
			HashSet<StringHash>& dependents = iter->second_;
			dependents.Erase(nameHash);
			if(dependents.Empty())
				iter = dependentResouces_.Erase(iter);
			else
				++iter;
		}
	}

	String ResourceCache::PrintMemoryUsage() const
	{
		//todo
	}


	bool ResourceCache::Exists(const String &nameIn) const
	{
		MutexLock lock(resourceMutex_);

		String name = SanitateResourceName(nameIn);
		if(!isRouting_)
		{
			isRouting_ = true;
			for(unsigned i = 0; i< resourceRouters_.Size(); ++i)
			{
				resourceRouters_[i]->Route(name, RESOURCE_CHECKEXISTS);
			}
			isRouting_ = false;
		}

		if(name.Empty())
			return false;

		for(unsigned i=0; i<packages_.Size(); ++i)
		{
			if(packages_[i]->Exists(name))
				return true;
		}

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for(unsigned i=0; i< resourceDirs_.Size(); ++i)
		{
			if(fileSystem->FileExists(resourceDirs_[i] + name))
				return true;
		}

		return fileSystem->FileExists(name);
	}

	unsigned long long ResourceCache::GetMemoryBudget(StringHash type) const
	{
		auto iter = resourceGroups_.Find(type);
		return iter != resourceGroups_.End() ? iter->second_.memoryBudget_ : 0;
	}

	unsigned long long ResourceCache::GetMemoryUse(StringHash type) const
	{
		auto iter = resourceGroups_.Find(type);
		return iter != resourceGroups_.End() ? iter->second_.memoryUse_ : 0;
	}

	unsigned long long ResourceCache::GetTotalMemoryUse() const
	{
		unsigned long long res = 0;
		for(auto it = resourceGroups_.Begin(); it != resourceGroups_.End(); ++it)
		{
			res += it->second_.memoryUse_;
		}
		return res;
	}

	String ResourceCache::GetResourceFileName(const String &name) const
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for(unsigned i=0; i< resourceDirs_.Size(); ++i)
		{
			if(fileSystem->FileExists(resourceDirs_[i] + name))
				return resourceDirs_[i] + name;
		}
		if(IsAbsolutePath(name) && fileSystem->FileExists(name))
			return name;
		else
			return String();
	}

	ResourceRouter *ResourceCache::GetResourceRouter(unsigned index) const
	{
		return index < resourceRouters_.Size() ? resourceRouters_[index] : nullptr;
	}

	String ResourceCache::GetPreferredResourceDir(const String &path) const
	{
		String fixedPath = AddTrailingSlash(path);
		bool pathHashKnowDirs = false;
		bool parentHasKnowDirs = false;

		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		for(unsigned i = 0; checkDirs[i] != nullptr; ++i)
		{
			if(fileSystem->DirExists(fixedPath + checkDirs[i]))
			{
				pathHashKnowDirs = true;
				break;
			}
		}
		if(!pathHashKnowDirs)
		{
			String parentPath = GetParentPath(fixedPath);
			for(unsigned i=0; checkDirs[i] != nullptr; ++i)
			{
				if(fileSystem->DirExists(parentPath + checkDirs[i]))
				{
					parentHasKnowDirs = true;
					break;
				}
			}
			if(parentHasKnowDirs)
				fixedPath = parentPath;
		}
		return fixedPath;
	}

	String ResourceCache::SanitateResourceName(const String &nameIn) const
	{
		String name = GetInternalPath(nameIn);
		name.Replace("../", "");
		name.Replace("./", "");

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if(resourceDirs_.Size())
		{
			String namePath = GetPath(name);
			String exePath = fileSystem->GetProgramDir().Replaced("/./", "/");
			for(unsigned i=0; i< resourceDirs_.Size(); ++i)
			{
				String relativeResourcePath = resourceDirs_[i];
				if(relativeResourcePath.StartWith(exePath))
					relativeResourcePath = relativeResourcePath.SubString(exePath.Length());
				if(namePath.StartWith(resourceDirs_[i], false))
					namePath = namePath.SubString(resourceDirs_[i].Length());
				else if(namePath.StartWith(relativeResourcePath, false))
					namePath = namePath.SubString(relativeResourcePath.Length());
			}
			name = namePath + GetFileNameAndExtension(name);
		}
		return name.Trimmed();
	}

	String ResourceCache::SanitateResourceDirName(const String &name) const
	{
		String fixedPath = AddTrailingSlash(name);
		if(!IsAbsolutePath(fixedPath))
			fixedPath = GetSubsystem<FileSystem>()->GetCurrentDir() + fixedPath;

		fixedPath.Replace("/./", "/");
		return fixedPath.Trimmed();
	}

	void ResourceCache::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{
		for(unsigned i=0; i< fileWatchers_.Size(); ++i)
		{
			String fileName;
			while (fileWatchers_[i]->GetNextChange(fileName))
			{
				ReloadResourceWithDependencies(fileName);

				using namespace FileChanged;
				VariantMap newEventData = GetEventDataMap();
				newEventData[P_FILENAME] = fileWatchers_[i]->GetPath() + fileName;
				newEventData[P_RESOURCENAME] = fileName;
				SendEvent(E_FILECHANGED, newEventData);
			}
		}
#ifdef URHO3D_THREADING
		{
			//profile
			backgroundLoader_->FinishResources(finishBackgroundResourcesMs_);
		};
#endif
	}

	File *ResourceCache::SearchResourceDirs(const String &nameIn)
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for(unsigned i=0; i<resourceDirs_.Size(); ++i)
		{
			if(fileSystem->FileExists(resourceDirs_[i] + nameIn))
			{
				File* file = new File(context_, resourceDirs_[i] + nameIn);
				file->SetName(nameIn);
				return file;
			}
		}
		if(fileSystem->FileExists(nameIn))
			return new File(context_, nameIn);
		return nullptr;
	}

	File *ResourceCache::SearchPackages(const String &nameIn)
	{
		for(unsigned i=0; i< packages_.Size(); ++i)
		{
			if(packages_[i]->Exists(nameIn))
				return new File(context_, packages_[i], nameIn);
		}
		return nullptr;
	}

	void RegisterResourceLibrary(Context* context)
	{
		Image::RegisterObject(context);
		//JSONFile::RegisterObject(context);
		//todo
		XMLFile::RegisterObject(context);
	}
}

