//
// Created by liuhao1 on 2018/1/24.
//

#ifndef URHO3DCOPY_SCENE_H
#define URHO3DCOPY_SCENE_H

#include "Node.h"
#include "../Resource/XMLFile.h"

namespace Urho3D
{
	class File;
	class PackageFile;

	static const unsigned FIRST_REPLICATED_ID = 0x1;
	static const unsigned LAST_REPLICATED_ID = 0xffffff;
	static const unsigned FIRST_LOCAL_ID = 0x01000000; // LAST_REPLICATED_ID + 1
	static const unsigned LAST_LOCAL_ID = 0xffffffff;

	// Asynchronous scene loading mode
	enum LoadMode
	{
		// Todo, what is the difference between scene content and resources
		// Preload resources used by a scene or object prefab file, but do not load any scene content
		LOAD_RESOURCES_ONLY = 0,
		// Load scene content without preloading. Resources will be requested synchronously when encountered
		LOAD_SCENE,
		// Default mode: preload resources used by the scene first, then load the scene content
		LOAD_SCENE_AND_RESOURCES
	};

	// Asynchronous loading progress of a scene
	struct AsyncProgress
	{
		SharedPtr<File> file_;
		SharedPtr<XMLFile> xmlFile_;

		XMLElement xmlElement_;

		LoadMode mode_;
		// Resource name hashs left to load
		HashSet<StringHash> resources_;
		unsigned loadedResources_;
		unsigned totalResources_;
		// Loaded root-level nodes
		unsigned loadedNodes_;
		// Total root-level nodes
		unsigned totalNodes_;

	};

	class Scene : public Node
	{

	public:
		Scene(Context* context);
		virtual ~Scene() override ;

		static void RegisterObject(Context* context);

		virtual bool Load(Deserializer& source, bool setInstanceDefault = false) override ;
		virtual bool Save(Serializer& dest) const override;
		virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false) override ;
		virtual void MarkNetworkUpdate() override ;
		virtual void AddReplicationState(NodeReplicationState* state) override;

		bool LoadXML(Deserializer& source);
		bool SaveXML(Serializer& dest, const String& identation = "\t") const override ;

		bool LoadAsync(File* file, LoadMode mode = LOAD_SCENE_AND_RESOURCES);
		bool LoadAsyncXML(File* file, LoadMode mode = LOAD_SCENE_AND_RESOURCES);
		void StopAsyncLoading();
		Node* Instantiate(Deserializer& source, const Vector3& position, const Quaternion& rotation, CreateMode mode = REPLICATED);
		Node* InstantiateXML(const XMLElement& source, const Vector3& position, const Quaternion& rotation, CreateMode mode = REPLICATED);
		Node* InstantiateXML(Deserializer& source, const Vector3& position, const Quaternion& rotation, CreateMode mode = REPLICATED);

		void Clear(bool clearReplicated = true, bool clearLocal = true);
		void SetUpdateEnabled(bool enable);
		void SetTimeScale(float scale);
		void SetElapsedTime(float time);
		// Set network client motion smoothing constant
		void SetSmoothingConstant(float constant);
		// Set network client motion smoothing snap threshold
		void SetSnapThreshold(float threshold);
		// Set maximum millisenconds per frame to spend on async scene loading
		void SetAsyncLoadingMs(int ms);
		void AddRequiredPackageFile(PackageFile* package);
		void ClearRequiredPackageFiles();
		void RegisterVar(const String& name);
		void UnregisterVar(const String& name);
		void UnregisterAllVars();

		Node* GetNode(unsigned id) const;
		Component* GetComponent(unsigned id) const;
		bool GetNodeWithTag(PODVector<Node*>& dest, const String& tag) const;

		bool IsUpdateEnabled() const { return updateEnabled_; }
		bool IsAsyncLoading() const { return asyncLoading_; }
		float GetAsyncProgress() const;
		LoadMode GetAsyncLoadMode() const { return asyncProgress_.mode_; }
		const String& GetFileName() const { return fileName_; }
		unsigned GetChecksum() const { return checksum_; }
		float GetTimeScale() const { return timeScale_; }
		float GetElapsedTime() const { return elapsedTime_; }
		float GetSmoothingConstant() const { return smoothingConstant_; }
		float GetSnapThreshold() const { return snapThreshold_; }
		int GetAsyncLoadingMs() const { return asyncLoadingMs_; }

		const Vector<SharedPtr<PackageFile>>& GetRequiredPackageFiles() const { return requiredPackageFiles_; }

		const String& GetVarName(StringHash hash) const;

		void Update(float timeStep);
		void BeginThreadedUpdate();
		void EndThreadedUpdate();
		void DelayedMarkedDirty(Component* component);

		bool IsThreadedUpdate() const  { return threadedUpdate_; }

		unsigned GetFreeNodeID(CreateMode mode);
		unsigned GetFreeComponentID(CreateMode mode);

		void NodeTagAdded(Node* node, const String& tag);
		void NodeTagRemoved(Node* node, const String& tag);

		void NodeAdded(Node* node);
		void NodeRemoved(Node* node);
		void ComponentAdded(Component* component);
		void ComponentRemoved(Component* component);

		void SetVarNameAttr(const String& value);
		String GetVarNamesAttr() const;
		void PrepareNetworkUpdate();
		void CleanupConnection(Connection* connection);
		void MarkNetworkUpdate(Node* node);
		void MarkNetworkUpdate(Component* component);
		void MarkReplicationDirty(Node* node);

	private:
		void HandleUpdate(StringHash eventType, VariantMap& eventData);
		void HandleResourceBackgroundLoaded(StringHash eventType, VariantMap& eventData);
		void UpdateAsyncLoading();
		void FinishAsyncLoading();
		void FinishLoading(Deserializer* source);
		void FinishSaving(Serializer* dest) const;
		void PreloadResources(File* file, bool isSceneFile);
		void PreloadResourceXML(const XMLElement& element);

		// todo, what's the difference between local and replicated ?
		// todo, what's the difference between node's children and this?
		HashMap<unsigned, Node*> replicatedNodes_;
		HashMap<unsigned, Node*> localNodes_;
		// todo, what's the difference with components_ in node class ?
		HashMap<unsigned, Component*> replicatedComponents_;
		HashMap<unsigned, Component*> localComponents_;

		HashMap<StringHash, PODVector<Node*>> taggedNodes_;
		AsyncProgress asyncProgress_;
		SceneResolver resolver_;
		mutable String fileName_;
		Vector<SharedPtr<PackageFile>> requiredPackageFiles_;
		HashMap<StringHash, String> varNames_;
		HashSet<unsigned> networkUpdateNodes_;
		HashSet<unsigned> networkUpdateComponents_;
		PODVector<Component*> delayedDirtyComponents_;
		Mutex sceneMutex_;
		VariantMap smoothingData_;

		unsigned replicatedNodeID_;
		unsigned replicatedComponentID_;
		unsigned localNodeID_;
		unsigned localComponentID_;

		mutable unsigned checksum_;
		int asyncLoadingMs_;
		float timeScale_;
		float elapsedTime_;
		float smoothingConstant_;
		float snapThreshold_;
		bool updateEnabled_;
		bool asyncLoading_;
		bool threadedUpdate_;
	};

	void RegisterSceneLibrary(Context* context);
}



#endif //URHO3DCOPY_SCENE_H
