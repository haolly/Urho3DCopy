//
// Created by liuhao1 on 2018/1/24.
//

#include "Scene.h"
#include "../Core/CoreEvent.h"
#include "../Resource/ResourceEvents.h"
#include "../Scene/Component.h"
#include "../IO/File.h"
#include "../Resource/ResourceCache.h"
#include "../Container/HashMap.h"
#include "SceneEvents.h"

namespace Urho3D
{
	const char* LOGIC_CATEGORY = "Logic";
	const char* SUBSYSTEM_CATEGORY = "Subsystem";

	static const float DEFAULT_SMOOTHING_CONSTANT = 50.0f;
	static const float DEFAULT_SNAP_THRESHOLD = 5.0F;

	Scene::Scene(Context *context) :
			Node(context),
			replicatedNodeID_(FIRST_REPLICATED_ID),
			replicatedComponentID_(FIRST_REPLICATED_ID),
			localNodeID_(FIRST_LOCAL_ID),
			localComponentID_(FIRST_LOCAL_ID),
			checksum_(0),
			asyncLoadingMs_(5),
			timeScale_(1.0f),
			elapsedTime_(0),
			smoothingConstant_(DEFAULT_SMOOTHING_CONSTANT),
			snapThreshold_(DEFAULT_SNAP_THRESHOLD),
			updateEnabled_(true),
			asyncLoading_(false),
			threadedUpdate_(false)
	{
		SetID(GetFreeNodeID(REPLICATED));
		NodeAdded(this);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Scene, HandleUpdate));
		SubscribeToEvent(E_RESOURCEBACKGROUNDLOADED, URHO3D_HANDLER(Scene, HandleResourceBackgroundLoaded));
	}

	Scene::~Scene()
	{
		// Remove root-level components first , so that subsystems such as the octree destroy themselves.
		// This will speed up the removal of child nodes' components
		// todo, why Scene is a Node, what is the scene's components and children?
		RemoveAllComponents();
		RemoveAllChildren();

		for(auto it = replicatedNodes_.Begin(); it != replicatedNodes_.End(); ++it)
			it->second_->ResetScene();
		for(auto it = localNodes_.Begin(); it != localNodes_.End(); ++it)
			it->second_->ResetScene();
	}

	void Scene::RegisterObject(Context *context)
	{
		context->RegisterFactory<Scene>();

		URHO3D_ACCESSOR_ATTRIBUTE("Name", GetName, SetName, String, String::EMPTY, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Time Scale", GetTimeScale, SetTimeScale, float, 1.0f, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Smoothing Constant", GetSmoothingConstant, SetSmoothingConstant, float, DEFAULT_SMOOTHING_CONSTANT,
									AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Snap Threshold", GetSnapThreshold, SetSnapThreshold, float, DEFAULT_SNAP_THRESHOLD, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Elapsed Time", GetElapsedTime, SetElapsedTime, float, 0.0f, AM_FILE);

		URHO3D_ATTRIBUTE("Next Replicated Node ID", unsigned, replicatedNodeID_, FIRST_REPLICATED_ID, AM_FILE | AM_NOEDIT);

		//todo
	}

	bool Scene::Load(Deserializer &source)
	{
		StopAsyncLoading();

		//Check ID
		if(source.ReadFileID() != "USCN")
		{
			URHO3D_LOGERROR(source.GetName() + " is not a valid scene file");
			return false;
		}

		URHO3D_LOGINFO("Loading scene from " + source.GetName());
		Clear();

		// Load the whole scene, then perform post-load if successfully loaded
		if(Node::Load(source))
		{
			FinishLoading(&source);
			return true;
		}
		return false;
	}

	bool Scene::Save(Serializer &dest) const
	{
		if(!dest.WriteFileID("USCN"))
		{
			URHO3D_LOGERROR("Could not save scene, writing to stream failed");
			return false;
		}

		auto* ptr = dynamic_cast<Deserializer*>(&dest);
		if(ptr)
			URHO3D_LOGINFO("Saving scene to " + ptr->GetName());

		if(Node::Save(dest))
		{
			FinishSaving(&dest);
			return true;
		}
		else
			return false;
	}

	bool Scene::LoadXML(const XMLElement &source)
	{
		StopAsyncLoading();

		// Load the whole scene, the perform post-load if successfully loaded
		// Note, the scene name and checksum can not be set, as we only used an XMLElement
		if(Node::LoadXML(source))
		{
			FinishLoading(nullptr);
			return true;
		}
		else
			return false;
	}

	void Scene::MarkNetworkUpdate()
	{
		if(!networkUpdate_)
		{
			MarkNetworkUpdate(this);
			networkUpdate_ = true;
		}
	}

	void Scene::AddReplicationState(NodeReplicationState *state)
	{
	}

	bool Scene::LoadXML(Deserializer &source)
	{
		StopAsyncLoading();

		SharedPtr<XMLFile> xml(new XMLFile(context_));
		if(!xml->Load(source))
			return false;

		URHO3D_LOGINFO("Loading scene from " + source.GetName());

		Clear();

		if(Node::LoadXML(xml->GetRoot()))
		{
			FinishLoading(&source);
			return true;
		}
		else
			return false;
	}

	bool Scene::SaveXML(Serializer &dest, const String &identation) const
	{
		SharedPtr<XMLFile> xml(new XMLFile(context_));
		XMLElement rootElem = xml->CreateRoot("scene");
		if(!Node::SaveXML(rootElem))
			return false;

		auto* ptr = dynamic_cast<Deserializer*>(&dest);
		if(ptr)
			URHO3D_LOGINFO("Saving scene to " + ptr->GetName());

		if(xml->Save(dest, identation))
		{
			FinishSaving(&dest);
			return true;
		}
		else
			return false;
	}

	bool Scene::LoadAsync(File *file, LoadMode mode)
	{
		if(!file)
		{
			URHO3D_LOGERROR("Null file for async loading");
			return false;
		}

		StopAsyncLoading();

		// Check ID
		bool isSceneFile = file->ReadFileID() == "USCN";
		if(!isSceneFile)
		{
			// In resource load mode can load also object prefabs, which have no identifier
			if(mode > LOAD_RESOURCES_ONLY)
			{
				URHO3D_LOGERROR(file->GetName() + " is not a valid scene file");
				return false;
			}
			else
				file->Seek(0);
		}

		if(mode > LOAD_RESOURCES_ONLY)
		{
			URHO3D_LOGERROR("Loading scene from " + file->GetName());
			Clear();
		}

		asyncLoading_ = true;
		asyncProgress_.file_ = file;
		asyncProgress_.mode_ = mode;
		asyncProgress_.loadedNodes_ = asyncProgress_.totalNodes_ = asyncProgress_.loadedResources_ = asyncProgress_.totalResources_ = 0;
		asyncProgress_.resources_.Clear();

		if(mode > LOAD_RESOURCES_ONLY)
		{
			// Preload resources fi appropriate, then return to the original position for loading the scene content
			// if mode == LOAD_SCENE_AND_RESOURCES
			if(mode != LOAD_SCENE)
			{
				//todo profile
				unsigned currentPos = file->GetPosition();
				PreloadResources(file, isSceneFile);
				file->Seek(currentPos);
			}

			// Store own old ID for resolving possible root node references
			unsigned nodeID = file->ReadUInt();
			resolver_.AddNode(nodeID, this);

			//Load root level components first
			if(!Node::Load(*file, resolver_, false))
			{
				StopAsyncLoading();
				return false;
			}

			// Then prepare to load child nodes in the async updates
			asyncProgress_.totalNodes_ = file->ReadVLE();
		}
		else
		{
			URHO3D_LOGINFO("Preloading resources from " + file->GetName());
			PreloadResources(file, isSceneFile);
		}
		return true;
	}

	bool Scene::LoadAsyncXML(File *file, LoadMode mode)
	{
		if(!file)
		{
			URHO3D_LOGERROR("Null file for async loading");
			return false;
		}

		StopAsyncLoading();

		SharedPtr<XMLFile> xml(new XMLFile(context_));
		if(!xml->Load(*file))
			return false;

		if(mode > LOAD_RESOURCES_ONLY)
		{
			URHO3D_LOGINFO("Loading scene from " + file->GetName());
			Clear();
		}

		asyncLoading_ = true;
		asyncProgress_.xmlFile_ = xml;
		asyncProgress_.file_ = file;
		asyncProgress_.mode_ = mode;
		asyncProgress_.loadedNodes_ = asyncProgress_.totalNodes_ = asyncProgress_.loadedResources_ = asyncProgress_.totalResources_ = 0;
		asyncProgress_.resources_.Clear();

		if(mode > LOAD_RESOURCES_ONLY)
		{
			XMLElement rootElement = xml->GetRoot();

			// Preload resources if appropriate
			if(mode != LOAD_SCENE)
			{
				PreloadResourceXML(rootElement);
			}

			// Store own old ID for resolving possible root node references
			unsigned nodeID = rootElement.GetUInt("id");
			resolver_.AddNode(nodeID, this);

			// Load the root level components first;
			if(!Node::LoadXML(rootElement, resolver_, false))
				return false;

			// Then prepare for loading all root level child nodes in the async update
			XMLElement childNodeElement = rootElement.GetChild("node");
			asyncProgress_.xmlElement_ = childNodeElement;

			// Count the amount of child nodes
			while(childNodeElement)
			{
				asyncProgress_.totalNodes_++;
				childNodeElement = childNodeElement.GetNext("node");
			}
		}
		else
		{
			URHO3D_LOGINFO("Preloading resource from " + file->GetName());
			PreloadResourceXML(xml->GetRoot());
		}
		return true;
	}

	void Scene::StopAsyncLoading()
	{
		asyncLoading_ = false;
		asyncProgress_.file_.Reset();
		asyncProgress_.xmlFile_.Reset();
		asyncProgress_.xmlElement_ = XMLElement::EMPTY;
		asyncProgress_.resources_.Clear();
		resolver_.Reset();
	}

	Node *Scene::Instantiate(Deserializer &source, const Vector3 &position, const Quaternion &rotation, CreateMode mode)
	{
		return nullptr;
	}

	Node *Scene::InstantiateXML(const XMLElement &source, const Vector3 &position, const Quaternion &rotation,
	                            CreateMode mode)
	{
		return nullptr;
	}

	Node *
	Scene::InstantiateXML(Deserializer &source, const Vector3 &position, const Quaternion &rotation, CreateMode mode)
	{
		return nullptr;
	}

	void Scene::Clear(bool clearReplicated, bool clearLocal)
	{
		StopAsyncLoading();

		RemoveChildren(clearReplicated, clearLocal, true);
		RemoveComponents(clearReplicated, clearLocal);

		if(clearReplicated && clearLocal)
		{
			UnregisterAllVars();
			SetName(String::EMPTY);
			fileName_.Clear();
			checksum_ = 0;
		}

		if(clearReplicated)
		{
			replicatedNodeID_ =
		}
	}

	void Scene::SetUpdateEnabled(bool enable)
	{

	}

	void Scene::SetTimeScale(float scale)
	{

	}

	void Scene::SetElapsedTime(float time)
	{

	}

	void Scene::SetSmoothingConstant(float constant)
	{

	}

	void Scene::SetSnapThreshold(float threshold)
	{

	}

	void Scene::SetAsyncLoadingMs(int ms)
	{

	}

	void Scene::AddRequiredPackageFile(PackageFile *package)
	{

	}

	void Scene::ClearRequiredPackageFiles()
	{

	}

	void Scene::HandleUpdate(StringHash eventType, VariantMap &eventData)
	{

	}

	void Scene::HandleResourceBackgroundLoaded(StringHash eventType, VariantMap &eventData)
	{

	}

	void Scene::UpdateAsyncLoading()
	{
		{
			// Profile
		}
		// If resource left to load, do not load nodes yet
		//todo
	}

	void Scene::FinishAsyncLoading()
	{

	}

	void Scene::FinishLoading(Deserializer *source)
	{
		if(source)
		{
			fileName_ = source->GetName();
			checksum_ = source->GetChecksum();
		}
	}

	void Scene::FinishSaving(Serializer *dest) const
	{
		auto* ptr = dynamic_cast<Deserializer*>(dest);
		if(ptr)
		{
			fileName_ = ptr->GetName();
			checksum_ = ptr->GetChecksum();
		}
	}

	void Scene::PreloadResources(File *file, bool isSceneFile)
	{
#ifdef URHO3D_THREADING
		auto* cache = GetSubsystem<ResourceCache>();

		// Read node ID (not needed)
		file->ReadUInt();

		// Read Node or Scene attributes; these do not include any resource
		const Vector<AttributeInfo>* attributes = context_->GetAttributes(isSceneFile ?  Scene::GetTypeStatic() : Node::GetTypeStatic());
		assert(attributes);

		for(unsigned i = 0; i< attributes->Size(); ++i)
		{
			const AttributeInfo& attr = attributes->At(i);
			if(!(attr.mode_ & AM_FILE))
				continue;
			file->ReadVariant(attr.type_);
		}

		// Read component attributes
		unsigned numComponents = file->ReadVLE();
		for(unsigned i=0; i< numComponents; ++i)
		{
			VectorBuffer compBuffer(*file, file->ReadVLE());
			StringHash compType = compBuffer.ReadStringHash();
			compBuffer.ReadUInt();

			attributes = context_->GetAttributes(compType);
			if(attributes)
			{
				for(unsigned k = 0; k< attributes->Size(); ++k)
				{
					const auto& attr = attributes->At(k);
					if(!(attr.mode_ & AM_FILE))
						continue;

					Variant varValue = compBuffer.ReadVariant(attr.type_);
					if(attr.type_ == VAR_RESOURCEREF)
					{
						const ResourceRef& ref = varValue.GetResourceRef();
						// Sanitate resource name beforehand so that when we get the background load event, the name matches exactly.
						String name = cache->SanitateResourceName(ref.name_);
						bool success = cache->BackgroundLoadResource(ref.type_, name);
						if(success)
						{
							asyncProgress_.totalResources_++;
							asyncProgress_.resources_.Insert(StringHash(name));
						}
					}
					else if(attr.type_ == VAR_RESOURCEREFLIST)
					{
						const ResourceRefList& refList = varValue.GetResourceRefList();
						for(unsigned kk = 0; kk < refList.names_.Size(); ++kk)
						{
							String name = cache->SanitateResourceName(refList.names_[kk]);
							bool success = cache->BackgroundLoadResource(refList.type_, name);
							if(success)
							{
								asyncProgress_.totalResources_++;
								asyncProgress_.resources_.Insert(StringHash(name));
							}
						}
					}
				}
			}
		}

		// Read child nodes
		unsigned numChildren = file->ReadVLE();
		for(unsigned i = 0; i< numChildren; ++i)
		{
			PreloadResources(file, false);
		}
#endif
	}

	void Scene::PreloadResourceXML(const XMLElement &element)
	{
		//todo
	}

	void Scene::NodeAdded(Node *node)
	{
		if(!node || node->GetScene() == this)
			return;
		Scene* oldScene = node->GetScene();
		if(oldScene)
			oldScene->NodeRemoved(node);

		node->SetScene(this);

		// If the new node has an ID of zero(default), assign a replicated ID now
		unsigned id = node->GetID();
		if(!id)
		{
			id = GetFreeNodeID(REPLICATED);
			node->SetID(id);
		}

		// If node with the same ID exists, remove the scene reference from it and overwrite with the new node
		if(id < FIRST_LOCAL_ID)
		{
			auto it = replicatedNodes_.Find(id);
			if(it != replicatedNodes_.End() && it->second_ != node)
			{
				URHO3D_LOGWARNING("Overwriting node with ID " + String(id));
				NodeRemoved(it->second_);
			}

			replicatedNodes_[id] = node;

			MarkNetworkUpdate(node);
			MarkReplicationDirty(node);
		}
		else
		{
			auto it = localNodes_.Find(id);
			if(it != localNodes_.End() && it->second_ != node)
			{
				URHO3D_LOGWARNING("Overwriting node with ID " + String(id));
				NodeRemoved(it->second_);
			}
			localNodes_[id] = node;
		}

		if(!node->GetTags().Empty())
		{
			auto tags = node->GetTags();
			for(unsigned i=0; i< tags.Size(); ++i)
			{
				taggedNodes_[tags[i]].Push(node);
			}
		}

		// Add already created components and child nodes now
		// Note, the components will NEVER be null, because it is an container, which is allocated on the stack
		// ref https://stackoverflow.com/questions/8036474/when-vectors-are-allocated-do-they-use-memory-on-the-heap-or-the-stack
		const Vector<SharedPtr<Component>>& components = node->GetComponents();
		for(auto it = components.Begin(); it != components.End(); ++it)
		{
			ComponentAdded(*it);
		}

		const Vector<SharedPtr<Node>>& children = node->GetChildren();
		for(auto it = children.Begin(); it != children.End(); ++it)
		{
			NodeAdded(*it);
		}
	}

	void Scene::NodeRemoved(Node *node)
	{
		if(!node || node->GetScene() != this)
			return;
		unsigned id = node->GetID();
		if(id < FIRST_LOCAL_ID)
		{
			replicatedNodes_.Erase(id);
			MarkReplicationDirty(node);
		}
		else
			localNodes_.Erase(id);

		node->ResetScene();

		if(!node->GetTags().Empty())
		{
			const StringVector& tags = node->GetTags();
			for(unsigned i=0; i< tags.Size(); ++i)
				taggedNodes_[tags[i]].Remove(node);
		}

		auto&& components = node->GetComponents();
		for(auto it = components.Begin(); it != components.End(); ++it)
			ComponentRemoved(*it);
		auto&& children = node->GetChildren();
		for(auto it = children.Begin(); it != children.End(); ++it)
			NodeRemoved(*it);
	}

	void Scene::ComponentAdded(Component *component)
	{
		if(!component)
			return;
		unsigned id = component->GetID();
		if(!id)
		{
			id = GetFreeComponentID(REPLICATED);
			component->SetID(id);
		}

		if(id < FIRST_LOCAL_ID)
		{
			auto it = replicatedComponents_.Find(id);
			if(it != replicatedComponents_.End() && it->second_ != component)
			{
				URHO3D_LOGWARNING("Overwriting component with ID " + String(id));
				ComponentRemoved(it->second_);
			}
			replicatedComponents_[id] = component;
		}
		else
		{
			auto it = localComponents_.Find(id);
			if(it != localComponents_.End() && it->second_ != component)
			{
				URHO3D_LOGWARNING("Overwriting component with ID " + String(id));
				// todo, will free the component memory?
				ComponentRemoved(it->second_);
			}
			localComponents_[id] = component;
		}

		component->OnSceneSet(this);
	}

	// todo, need to destroy the memeory that component use ?
	void Scene::ComponentRemoved(Component *component)
	{
		if(!component)
			return;
		unsigned id = component->GetID();
		if(id < FIRST_LOCAL_ID)
		{
			replicatedComponents_.Erase(id);
		}
		else
			localComponents_.Erase(id);

		component->SetID(0);
		component->OnSceneSet(nullptr);
	}

	unsigned Scene::GetFreeNodeID(CreateMode mode)
	{
		if(mode == REPLICATED)
		{
			for(;;)
			{
				unsigned ret = replicatedNodeID_;
				if(replicatedNodeID_ < LAST_REPLICATED_ID)
					++replicatedNodeID_;
				else
					replicatedNodeID_ = FIRST_REPLICATED_ID;

				if(!replicatedNodes_.Contains(ret))
					return ret;
			}
		}
		else
		{
			for(;;)
			{
				unsigned ret = localNodeID_;
				if(localNodeID_ < LAST_LOCAL_ID)
					++localNodeID_;
				else
					localNodeID_ = FIRST_LOCAL_ID;

				if(!localNodes_.Contains(ret))
					return ret;
			}
		}
	}

	unsigned Scene::GetFreeComponentID(CreateMode mode)
	{
		if(mode == REPLICATED)
		{
			for(;;)
			{
				unsigned ret = replicatedComponentID_;
				if(replicatedComponentID_ < LAST_REPLICATED_ID)
					++replicatedComponentID_;
				else
					replicatedComponentID_ = FIRST_REPLICATED_ID;

				if(!replicatedComponents_.Contains(ret))
					return ret;
			}
		}
		else
		{
			for(;;)
			{
				unsigned ret = localComponentID_;
				if(localComponentID_ < LAST_LOCAL_ID)
					++localComponentID_;
				else
					localComponentID_ = FIRST_LOCAL_ID;

				if(!localComponents_.Contains(ret))
					return ret;
			}
		}
	}

	void Scene::MarkNetworkUpdate(Node *node)
	{
		if(node)
		{
			if(!threadedUpdate_)
				networkUpdateNodes_.Insert(node->GetID());
			else
			{
				MutexLock lock(sceneMutex_);
				networkUpdateNodes_.Insert(node->GetID());
			}
		}
	}

	void Scene::MarkNetworkUpdate(Component *component)
	{
		if(component)
		{
			if(!threadedUpdate_)
				networkUpdateComponents_.Insert(component->GetID());
			else
			{
				MutexLock lock(sceneMutex_);
				networkUpdateComponents_.Insert(component->GetID());
			}
		}
	}

	void Scene::MarkReplicationDirty(Node *node)
	{
		//todo
	}

	void Scene::RegisterVar(const String &name)
	{
		varNames_[name] = name;
	}

	void Scene::UnregisterVar(const String &name)
	{
		varNames_.Erase(name);
	}

	void Scene::UnregisterAllVars()
	{
		varNames_.Clear();
	}

	Node *Scene::GetNode(unsigned id) const
	{
		if(id < FIRST_LOCAL_ID)
		{
			const auto it = replicatedNodes_.Find(id);
			return it != replicatedNodes_.End() ? it->second_ : nullptr;
		}
		else
		{
			const auto it = localNodes_.Find(id);
			return it != localNodes_.End() ? it->second_ : nullptr;
		}
	}

	Component *Scene::GetComponent(unsigned id) const
	{
		if(id < FIRST_LOCAL_ID)
		{
			auto it = replicatedComponents_.Find(id);
			return it != replicatedComponents_.End() ? it->second_ : nullptr;
		}
		else
		{
			auto it = localComponents_.Find(id);
			return it != localComponents_.End() ? it->second_ : nullptr;
		}
	}

	bool Scene::GetNodeWithTag(PODVector<Node *> &dest, const String &tag) const
	{
		dest.Clear();
		const auto it = taggedNodes_.Find(tag);
		if(it != taggedNodes_.End())
		{
			dest = it->second_;
			return true;
		}
		else
			return false;
	}

	float Scene::GetAsyncProgress() const
	{
		return !asyncLoading_ || asyncProgress_.totalNodes_ + asyncProgress_.totalResources_ == 0 ? 1.0f :
		       (float)(asyncProgress_.loadedNodes_ + asyncProgress_.loadedResources_ ) /
				       (float)(asyncProgress_.totalResources_ + asyncProgress_.totalNodes_);
	}

	const String &Scene::GetVarName(StringHash hash) const
	{
	}

	void Scene::Update(float timeStep)
	{
		if(asyncLoading_)
		{
			UpdateAsyncLoading();
			// If only preloading resources, scene update can continue
			// todo, why?
			if(asyncProgress_.mode_ > LOAD_RESOURCES_ONLY)
				return;
		}

		timeStep *= timeScale_;
		using namespace SceneUpdate;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_SCENE] = this;
		eventData[P_TIMESTEP] = timeStep;

		// Update variable timestep logic
		SendEvent(E_SCENEUPDATE, eventData);

		// Update scene attribute animation
		SendEvent(E_ATTRIBUTEANIMATIONUPDATE, eventData);

		// Update scene subsystems. If a physics world is present, it will be updated,
		// triggering fixed timestep logic updates
		// todo, physics update should not happen in this update
		SendEvent(E_SCENESUBSYSTEMUPDATE, eventData);

		// Update transform smoothing
		{
			float constant = 1.0f - Clamp(powf(2.0f, -timeStep * smoothingConstant_), 0.0f, 1.0f);
			float squaredSnapThreshold = snapThreshold_ * snapThreshold_;

			using namespace UpdateSmoothing;
			smoothingData_[P_CONSTANT] = constant;
			smoothingData_[P_SQUAREDSNAPTHRESHOLD] = squaredSnapThreshold;
			SendEvent(E_UPDATESMOOTHING, smoothingData_);
		}

		// Post update variable timestep logic
		SendEvent(E_SCENEPOSTUPDATE, eventData);
		// Note: using a float for elapsed time accumulation is inherently inaccurate
		// The purpose of this value is primarily to update material animation effects,
		// as it is available to shaders. It can be reset by calling SetElapsedTime()
		elapsedTime_ += timeStep;
	}

	void Scene::BeginThreadedUpdate()
	{
		//todo
	}

	void Scene::EndThreadedUpdate()
	{

	}

	void Scene::DelayedMarkedDirty(Component *component)
	{

	}

	void Scene::NodeTagAdded(Node *node, const String &tag)
	{

	}

	void Scene::NodeTagRemoved(Node *node, const String &tag)
	{

	}

	void Scene::SetVarNameAttr(const String &value)
	{

	}

	String Scene::GetVarNamesAttr() const
	{
		return String();
	}

	void Scene::PrepareNetworkUpdate()
	{
		Node::PrepareNetworkUpdate();
	}

	void Scene::CleanupConnection(Connection *connection)
	{
		Node::CleanupConnection(connection);
	}

	void RegisterSceneLibrary(Context *context)
	{
		ValueAnimation::RegisterObject(context);
		ObjectAnimation::RegisterObject(context);
		Node::RegisterObject(context);
		Scene::RegisterObject(context);
		//todo
	}

}