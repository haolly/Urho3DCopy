//
// Created by liuhao1 on 2018/1/24.
//

#include "Scene.h"
#include "../Core/CoreEvent.h"
#include "../Resource/ResourceEvents.h"
#include "../Scene/Component.h"

namespace Urho3D
{

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
		//todo
	}

	bool Scene::Load(Deserializer &source, bool setInstanceDefault)
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
		if(Node::Load(source, setInstanceDefault))
		{
			FinishLoading(&source);
			return true;
		}
		return false;
	}

	bool Scene::Save(Serializer &dest) const
	{
		return Node::Save(dest);
	}

	bool Scene::LoadXML(const XMLElement &source, bool setInstanceDefault)
	{
		return Node::LoadXML(source, setInstanceDefault);
	}

	void Scene::MarkNetworkUpdate()
	{
		Node::MarkNetworkUpdate();
	}

	void Scene::AddReplicationState(NodeReplicationState *state)
	{
		Node::AddReplicationState(state);
	}

	bool Scene::LoadXML(Deserializer &source)
	{
		return false;
	}

	bool Scene::SaveXML(Serializer &dest, const String &identation) const
	{
		return Node::SaveXML(dest, identation);
	}

	bool Scene::LoadAsync(File *file, LoadMode mode)
	{
		return false;
	}

	bool Scene::LoadAsyncXML(File *file, LoadMode mode)
	{
		return false;
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

	}

	void Scene::PreloadResources(File *file, bool isSceneFile)
	{

	}

	void Scene::PreloadResourceXML(const XMLElement &element)
	{

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

	}

	void Scene::UnregisterVar(const String &name)
	{

	}

	void Scene::UnregisterAllVars()
	{

	}

	Node *Scene::GetNode(unsigned id) const
	{
		return nullptr;
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
		return false;
	}

	float Scene::GetAsyncProgress() const
	{
		return 0;
	}
}