//
// Created by liuhao on 2018/1/17.
//

#ifndef URHO3DCOPY_NODE_H
#define URHO3DCOPY_NODE_H

#include "Animatable.h"
#include "../IO/VectorBuffer.h"
#include "SceneResolver.h"

namespace Urho3D
{
	class Component;
	class Connection;
	class Node;
	class Scene;

	enum CreateMode
	{
		//todo, replicated are synced through network ??
		REPLICATED = 0,
		LOCAL = 1
	};

	enum TransformSpace
	{
		TS_LOCAL = 0,
		TS_PARENT,
		TS_WORLD
	};

	struct NodeImpl
	{
		PODVector<Node*> dependencyNodes_;
		// Network onwer connection
		Connection* owner_;
		String name_;
		StringVector tags_;
		StringHash nameHash_;
		mutable VectorBuffer attrBuffer_;
	};


	// Note, Analogy to unity transform
	class Node : public Animatable
	{
		URHO3D_OBJECT(Node, Animatable);

		friend class Connection;

	public:
		Node(Context* context);
		~Node() override;
		static void RegisterObject(Context* context);

		bool Load(Deserializer& source) override;
		bool LoadXML(const XMLElement& source) override;
		bool Save(Serializer& dest) const override ;
		bool SaveXML(XMLElement& dest) const override ;
		void ApplyAttributes() override ;

		bool SaveDefaultAttributes() const override { return true; }
		void MarkNetworkUpdate() override ;
		virtual void AddReplicationState(NodeReplicationState* state);

		bool SaveXML(Serializer& dest, const String& indentation = "\t") const;
		bool SetName(const String& name);

		void SetTags(const StringVector& tags);
		void AddTag(const String& tags);
		void AddTags(const String& tags, char separator = ';');
		void AddTags(const StringVector& tags);
		bool RemoveTag(const String& tag);
		void RemoveAllTags();

		void SetPosition(const Vector3& position);
		void SetPosition2D(const Vector2& position)
		{
			SetPosition(Vector3(position));
		}
		void SetPosition2D(float x, float y)
		{
			SetPosition(Vector3(x, y, 0.0f));
		}
		void SetRotation(const Quaternion& rotation);
		void SetRotation2D(float rotation)
		{
			SetRotation(Quaternion(rotation));
		}

		void SetDirection(const Vector3& direction);
		void SetScale(float scale);
		void SetScale(const Vector3& scale);
		void SetScale2D(const Vector2& scale)
		{
			SetScale(Vector3(scale, 1.0f));
		}
		void SetScale2D(float x, float y)
		{
			SetScale(Vector3(x, y, 1.0f));
		}

		void SetTransform(const Vector3& position, const Quaternion& rotation);
		void SetTransform(const Vector3& position, const Quaternion& rotation, float scale);
		void SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
		void SetTransform(const Matrix3x4& matrix);

		void SetTransform2D(const Vector2& position, float rotation)
		{
			SetTransform(Vector3(position), Quaternion(rotation));
		}

		void SetTransform2D(const Vector2& position, float rotation, float scale)
		{
			SetTransform(Vector3(position), Quaternion(rotation), scale);
		}

		void SetTransform2D(const Vector2& position, float rotation, const Vector2& scale)
		{
			SetTransform(Vector3(position), Quaternion(rotation), Vector3(scale, 1.0f));
		}

		void SetWorldPosition(const Vector3& position);
		void SetWorldPosition2D(const Vector2& position)
		{
			SetWorldPosition(Vector3(position));
		}

		void SetWorldPosition2D(float x, float y)
		{
			SetWorldPosition(Vector3(x, y, 0.0f));
		}

		void SetWorldRotation(const Quaternion& rotation);
		void SetWorldRotation2D(float rotation)
		{
			SetWorldRotation(Quaternion(rotation));
		}

		void SetWorldDirection(const Vector3& direction);
		void SetWorldScale(float scale);
		void SetWorldScale(const Vector3& scale);
		void SetWorldScale2D(const Vector2& scale)
		{
			SetWorldScale(Vector3(scale, 1.0f));
		}
		void SetWorldScale2D(float x, float y)
		{
			SetWorldScale(Vector3(x, y, 1.0f));
		}

		void SetWorldTransform(const Vector3& position, const Quaternion& rotation);
		void SetWorldTransform(const Vector3& position, const Quaternion& rotation, float scale);
		void SetWorldTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
		void SetWorldTransform2D(const Vector2& position, float rotation)
		{
			SetWorldTransform(Vector3(position), Quaternion(rotation));
		}
		void SetWorldTransform3D(const Vector2& position, float rotation, const Vector2& scale)
		{
			SetWorldTransform(Vector3(position), Quaternion(rotation), Vector3(scale, 1.0f));
		}

		void Translate(const Vector3& delta, TransformSpace space = TS_LOCAL);
		void Translate2D(const Vector2& delta, TransformSpace space = TS_LOCAL)
		{
			Translate(Vector3(delta), space);
		}

		void Rotate(const Quaternion& delta, TransformSpace space = TS_LOCAL);
		void Rotate2D(float delta, TransformSpace space = TS_LOCAL)
		{
			Rotate(Quaternion(delta), space);
		}

		void RotateAround(const Vector3& point, const Quaternion& delta, TransformSpace space = TS_LOCAL);
		void RotateAround2D(const Vector2& point, float delta, TransformSpace space = TS_LOCAL)
		{
			RotateAround(Vector3(point), Quaternion(delta), space);
		}

		// Rotate around the x axis
		void Pitch(float angle, TransformSpace space = TS_LOCAL);
		// Rotate around the Y axis
		void Yaw(float angle, TransformSpace space = TS_LOCAL);
		// Rotate around the z axis
		void Roll(float angle, TransformSpace space = TS_LOCAL);

		bool LookAt(const Vector3& target, const Vector3& up = Vector3::UP, TransformSpace space = TS_WORLD);
		void Scale(float scale);
		void Scale(const Vector3& scale);

		void Scale2D(const Vector2& scale)
		{
			Scale(Vector3(scale, 1.0f));
		}

		void SetEnabled(bool enable);
		void SetDeepEnabled(bool enable);
		void ResetDeepEnabled();
		void SetEnabledRecursive(bool enable);
		void SetOwner(Connection* owner);
		void MarkDirty();

		Node* CreateChild(const String& name = String::EMPTY, CreateMode mode = REPLICATED, unsigned id = 0, bool temporary = false);
		Node* CreateTemporaryChild(const String& name = String::EMPTY, CreateMode mode = REPLICATED, unsigned id = 0);
		void AddChild(Node* node, unsigned index = M_MAX_UNSIGNED);
		void RemoveChild(Node* node);
		void RemoveAllChildren();
		void RemoveChildren(bool removeReplicated, bool removeLocal, bool recursive);

		Component* CreateComponent(StringHash type, CreateMode mode = REPLICATED, unsigned id = 0);
		Component* GetOrCreateComponent(StringHash type, CreateMode mode = REPLICATED, unsigned id = 0);
		Component* CloneComponent(Component* component, unsigned id = 0);
		Component* CloneComponent(Component* component, CreateMode mode, unsigned id = 0);

		void RemoveComponent(Component* component);
		void RemoveComponent(StringHash type);
		void RemoveComponents(bool removeReplicated, bool removeLocal);
		void RemoveComponents(StringHash type);
		void RemoveAllComponents();
		void ReorderComponent(Component* component, unsigned index);

		Node* Clone(CreateMode mode = REPLICATED);
		void Remove();

		void SetParent(Node* parent);
		void SetVar(StringHash key, const Variant& value);
		void AddListener(Component* component);
		void RemoveListener(Component* component);

		//todo

		template <class T>
		T* CreateComponent(CreateMode mode = REPLICATED, unsigned id = 0);

		template <class T>
		T* GetOrCreateComponent(CreateMode mode = REPLICATED, unsigned id = 0);

		template <class T>
		void RemoveComponent();

		template <class T>
		void RemoveComponents();

		unsigned GetID() const { return id_; }
		const String& GetName() const { return impl_->name_; }
		StringHash GetNameHash() const { return impl_->nameHash_;}
		const StringVector& GetTags() const { return impl_->tags_; }
		bool HasTag(const String& tag) const;
		Node* GetParent() const { return parent_; }
		Scene* GetScene() const { return scene_; }
		bool IsChildOf(Node* node) const;
		bool IsEnabled() const { return enabled_; }
		bool IsEnabledSelf() const { return enabledPrev_; }
		Connection* GetOwner() const { return impl_->owner_; }
		const Vector3& GetPosition() const { return position_; }
		Vector2 GetPosition2D() const { return Vector2(position_.x_, position_.y_); }
		const Quaternion& GetRotation() const { return rotation_; }
		float GetRotation2D() const { return rotation_.RollAngle(); }
		Vector3 GetDirection() const { return rotation_ * Vector3::FORWARD; }

		Vector3 GetUp() const { return rotation_ * Vector3::UP; }

		Vector3 GetRight() const { return rotation_ * Vector3::RIGHT; }

		const Vector3& GetScale() const { return scale_; }

		Vector2 GetScale2D() const { return Vector2(scale_.x_, scale_.y_); }

		Matrix3x4 GetTransform() const { return Matrix3x4(position_, rotation_, scale_); }

		Vector3 GetWorldPosition() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldTransform_.Translation();
		}

		Vector2 GetWorldPosition2D() const
		{
			Vector3 worldPosition = GetWorldPosition();
			return Vector2(worldPosition.x_, worldPosition.y_);
		}


		Quaternion GetWorldRotation() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldRotation_;
		}

		float GetWorldRotation2D() const
		{
			return GetWorldRotation().RollAngle();
		}

		Vector3 GetWorldDirection() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldRotation_ * Vector3::FORWARD;
		}

		Vector3 GetWorldUp() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldRotation_ * Vector3::UP;
		}

		Vector3 GetWorldRight() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldRotation_ * Vector3::RIGHT;
		}

		Vector3 GetWorldScale() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldTransform_.Scale();
		}

		Vector3 GetSingleWorldScale() const ;

		Vector2 GetWorldScale2D() const
		{
			Vector3 worldScale = GetWorldScale();
			return Vector2(worldScale.x_, worldScale.y_);
		}

		const Matrix3x4& GetWorldTransform() const
		{
			if(dirty_)
				UpdateWorldTransform();
			return worldTransform_;
		}

		Vector3 LocalToWorld(const Vector3& position) const;
		Vector3 LocalToWorld(const Vector4& vector) const;
		Vector2 LocalToWorld2D(const Vector2& vector) const;
		Vector3 WorldToLocal(const Vector3& position) const;
		Vector3 WorldToLodal(const Vector4& vector) const;
		Vector2 WorldToLocal(const Vector2& vector) const;

		bool IsDirty() const { return dirty_; }
		unsigned GetNumChildren(bool recursive = false) const;
		const Vector<SharedPtr<Node>>& GetChildren() const { return children_; }
		void GetChildren(PODVector<Node*>& dest, bool recursive = false) const;
		PODVector<Node*> GetChildren(bool recursive) const;
		void GetChildrenWithComponent(PODVector<Node*>& dest, StringHash type, bool recursive = false) const;
		PODVector<Node*> GetChildrenWithComponent(StringHash type, bool recursive = false) const;
		void GetChildrenWithTag(PODVector<Node*>& dest, const String& tag, bool recursive = false) const;
		PODVector<Node*> GetChildrenWithTag(const String& tag, bool recursive = false) const;

		Node* GetChild(unsigned index) const;
		Node* GetChild(const String& name, bool recursive = false) const;
		Node* GetChild(const char* name, bool recursive = false) const;
		Node* GetChild(StringHash nameHash, bool recursive = false) const;

		unsigned GetNumComponents() const { return components_.Size(); }
		unsigned GetNumNetworkComponents() const;
		const Vector<SharedPtr<Component>>& GetComponents() const { return components_; }
		void GetComponents(PODVector<Component*>& dest, StringHash type, bool recursive = false) const;
		Component* GetComponent(StringHash type, bool recursive = false) const;
		Component* GetParentComponent(StringHash type, bool fullTraversal = false) const;
		bool HashComponent(StringHash type) const;

		const Vector<WeakPtr<Component>> GetListeners() const { return listeners_; }
		const Variant& GetVar(StringHash key) const;
		const VariantMap& GetVars() const { return vars_; }

		template <class T>
		T* GetDerivedComponent(bool recursive = false) const;

		template <class T>
		T* GetParentDerivedComponent(bool fullTraversal= false) const;

		//todo

		void SetID(unsigned id);
		void SetScene(Scene* scene);
		void ResetScene();
		void SetNetPositionAttr(const Vector3& value);
		void SetNetRotationAttr(const PODVector<unsigned char>& value);
		void SetNetParentAttr(const PODVector<unsigned char>& value);
		const Vector3& GetNetPositionAttr() const;
		const PODVector<unsigned char>& GetNetRotationAttr() const;
		const PODVector<unsigned char>& GetNetParentAttr() const;

		bool Load(Deserializer& source, SceneResolver& resolver, bool loadChildren = true, bool rewriteIDs = false,
				CreateMode mode = REPLICATED);
		bool LoadXML(const XMLElement& source, SceneResolver& resolver, bool loadChildren = true, bool rewriteIDs = false,
				CreateMode mode = REPLICATED);

		const PODVector<Node*>& GetDependencyNodes() const { return impl_->dependencyNodes_; }

		void PrepareNetworkUpdate();
		void CleanupConnection(Connection* connection);
		void MarkReplicationDirty();
		Node* CreateChild(unsigned id, CreateMode mode, bool temporary = false);
		void AddComponent(Component* component, unsigned id, CreateMode mode);
		unsigned GetNumPersistentChildren() const;
		unsigned GetNumPersistentComponents() const;

		void SetPositionSilent(const Vector3& position)
		{
			position_ = position;
		}

		void SetRotationSilent(const Quaternion& rotation)
		{
			rotation_ = rotation;
		}

		void SetScaleSilent(const Vector3& scale)
		{
			scale_ = scale;
		}

		// Set local transform silently without marking the node & child dirty. Used by animation code
		void SetTransformSilent(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

	protected:
		virtual void OnAttributeAnimationAdded() override ;
		virtual void OnAttributeAnimationRemoved() override ;
		virtual Animatable* FindAttributeAnimationTarget(const String& name, String& outName) override ;

	private:
		void SetEnabled(bool enable, bool recursive, bool storeSelf);
		Component* SafeCreateComponent(const String& typeName, StringHash type, CreateMode mode, unsigned id);
		void UpdateWorldTransform() const;
		void RemoveChild(Vector<SharedPtr<Node>>::Iterator it);
		void GetChildrenRecursive(PODVector<Node*>& dest) const;
		void GetChildrenWithComponentRecursive(PODVector<Node*>& dest, StringHash type) const;
		void GetChildrenWithTagRecursive(PODVector<Node*>& dest, const String& tag) const;
		void GetComponentsRecursive(PODVector<Component*>& dest, StringHash type) const;
		Node* CloneRecursive(Node* parent, SceneResolver& resolver, CreateMode mode);
		void RemoveComponent(Vector<SharedPtr<Component>>::Iterator it);
		void HanldeAttributeAnimationUpdate(StringHash eventType, VariantMap& eventData);

		mutable Matrix3x4 worldTransform_;
		mutable bool dirty_;
		bool enabled_;
		bool enabledPrev_;

	protected:
		bool networkUpdate_;
	private:
		Node* parent_;
		// Scene (root node)
		Scene* scene_;
		unsigned id_;
		Vector3 position_;
		Quaternion rotation_;
		Vector3 scale_;
		mutable Quaternion worldRotation_;

		Vector<SharedPtr<Component>> components_;
		Vector<SharedPtr<Node>> children_;

		Vector<WeakPtr<Component>> listeners_;
		UniquePtr<NodeImpl> impl_;
	protected:
		VariantMap vars_;
	};

	template <class T>
	T* Node::CreateComponent(CreateMode mode, unsigned id)
	{
		return static_cast<T*>(CreateComponent(T::GetTypeStatic(), mode, id));
	}

	template <class T>
	T* Node::GetOrCreateComponent(CreateMode mode, unsigned int id)
	{
		return static_cast<T*>(GetOrCreateComponent(T::GetTypeStatic(), mode, id));
	}

	template <class T>
	void Node::RemoveComponent()
	{
		RemoveComponent(T::GetTypeStatic());
	}

	template <class T>
	void Node::RemoveComponents()
	{
		RemoveComponents(T::GetTypeStatic());
	}
}


#endif //URHO3DCOPY_NODE_H
