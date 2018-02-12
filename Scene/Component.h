//
// Created by liuhao1 on 2018/1/23.
//

#ifndef URHO3DCOPY_COMPONENT_H
#define URHO3DCOPY_COMPONENT_H

#include "../Core/Context.h"
#include "Animatable.h"

namespace Urho3D
{
	class Node;
	class Scene;

	class Component : public Animatable
	{
		URHO3D_OBJECT(Component, Animatable);
		friend class Node;
		friend class Scene;

	public:
		Component(Context* context);
		virtual ~Component() override;

		virtual void OnSetEnabled() {}

		virtual bool Save(Serializer& dest) const override;
		virtual bool SaveXML(XMLElement& dest) const override;
		virtual void MarkNetworkUpdate() override;

		virtual void GetDependencyNodes(PODVector<Node*>& dest);
		virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

		void SetEnabled(bool enable);
		void Remove();

		unsigned GetID() const { return id_; }
		bool IsReplicated() const;
		Node* GetNode() const { return node_; }
		Scene* GetScene() const ;
		bool IsEnabled() const { return enabled_; }

		// Return whether is effectively enabled(node is also enabled)
		bool IsEnabledEffective() const;
		Component* GetComponent(StringHash type) const;
		void GetComponents(PODVector<Component*>& dest, StringHash type) const;
		template <class T> T* GetComponent() const;
		template <class T> void GetComponents(PODVector<T*>& dest) const;
		//todo

	protected:
		virtual void OnAttributeAnimationAdded() override ;
		virtual void OnAttributeAnimationRemoved() override ;
		virtual void OnNodeSet(Node* node);
		virtual void OnSceneSet(Scene* scene);
		virtual void OnMarkedDirty(Node* node);
		virtual void OnNodeSetEnabled(Node* node);
		void SetID(unsigned id);
		void SetNode(Node* node);
		void HandleAttributeAnimationUpdate(StringHash eventType, VariantMap& eventData);

		// Return a component from the scene root that sends out fixed update events
		// eightr PhysicsWorld or PhysicWorld2D. Return null if neighter exists
		Component* GetFixedUpdateSource();
		void DoAutoRemove(AutoRemoveNode node);

		Node* node_;
		unsigned id_;
		bool networkUpdate_;
		bool enabled_;
	};

	template <class T>
	T* Component::GetComponent() const
	{
		return static_cast<T*>(GetComponent(T::GetTypeStatic()));
	}

	template <class T>
	void Component::GetComponents(PODVector<T*>& dest) const
	{
		GetComponents(reinterpret_cast<PODVector<Component*>&>(dest), T::GetTypeStatic());
	}


}


#endif //URHO3DCOPY_COMPONENT_H
