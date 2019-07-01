//
// Created by liuhao1 on 2018/1/23.
//

#include "Component.h"
#include "../Resource/XMLElement.h
#include "SceneEvents.h"
#include "Scene.h"

namespace Urho3D
{
	Component::Component(Context *context) :
			Animatable(context),
			node_(nullptr),
			id_(0),
			networkUpdate_(false),
			enabled_(true)
	{
	}

	Component::~Component()
	{
	}

	bool Component::Save(Serializer &dest) const
	{
		// Write type and ID
		if(!dest.WriteStringHash(GetType()))
			return false;
		if(!dest.WriteUInt(id_))
			return false;

		// Write attributes
		return Animatable::Save(dest);
	}

	bool Component::SaveXML(XMLElement &dest) const
	{
		if(!dest.SetString("type", GetTypeName()))
			return false;
		if(!dest.SetUInt("id", id_))
			return false;

		return Animatable::SaveXML(dest);
	}

	void Component::MarkNetworkUpdate()
	{
		for(!networkUpdate_ && id_ < FIRST_LOCAL_ID)
		{
			Scene* scene = GetScene();
			if(scene)
			{
				scene->MarkNetworkUpdate(this);
				networkUpdate_ = true;
			}
		}
	}

	void Component::GetDependencyNodes(PODVector<Node *> &dest)
	{
	}

	void Component::DrawDebugGeometry(DebugRenderer *debug, bool depthTest)
	{

	}

	void Component::SetEnabled(bool enable)
	{
		if(enable != enabled_)
		{
			enabled_ = enable;
			OnSetEnabled();
			MarkNetworkUpdate();

			// Send change event for the component
			Scene* scene = GetScene();
			if(scene)
			{
				using namespace ComponentEnabledChanged;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_SCENE] = scene;
				eventData[P_NODE] = node_;
				eventData[P_COMPONENT] = this;

				scene->SendEvent(E_COMPONENTENABLEDCHANGED, eventData);
			}
		}
	}

	void Component::Remove()
	{
		if(node_)
			node_->RemoveComponent(this);
	}

	bool Component::IsReplicated() const
	{
		return id_ < FIRST_LOCAL_ID;
	}

	Scene *Component::GetScene() const
	{
		return node_ ? node_->GetScene() : nullptr;
	}

	bool Component::IsEnabledEffective() const
	{
		return enabled_ && node_ && node_->IsEnabled();
	}

	Component *Component::GetComponent(StringHash type) const
	{
		return node_ ? node_->GetComponent(type) : nullptr;
	}

	void Component::GetComponents(PODVector<Component *> &dest, StringHash type) const
	{
		if(node_)
			return node_->GetComponents(dest, type);
		else
			dest.Clear();
	}

	void Component::OnAttributeAnimationAdded()
	{
		// todo, only one attribute ??
		if(attributeAnimationInfos_.Size() == 1)
			SubscribeToEvent(GetScene(), E_ATTRIBUTEANIMATIONUPDATE, URHO3D_HANDLER(Component, HandleAttributeAnimationUpdate));
	}

	void Component::OnAttributeAnimationRemoved()
	{
		if(attributeAnimationInfos_.Empty())
			UnsubscribeFromEvent(GetScene(), E_ATTRIBUTEANIMATIONUPDATE);
	}

	void Component::OnNodeSet(Node *node)
	{

	}

	void Component::OnSceneSet(Scene *scene)
	{

	}

	void Component::OnMarkedDirty(Node *node)
	{

	}

	void Component::OnNodeSetEnabled(Node *node)
	{

	}

	void Component::SetID(unsigned id)
	{
		id_ = id;
	}

	void Component::SetNode(Node *node)
	{
		node_ = node;
		OnNodeSet(node_);
	}

	void Component::HandleAttributeAnimationUpdate(StringHash eventType, VariantMap &eventData)
	{
		using namespace AttributeAnimationUpdate;
		UpdateAttributeAnimations(eventData[P_TIMESTEP].GetFloat());
	}

	Component *Component::GetFixedUpdateSource()
	{
		Component* ret = nullptr;
		Scene* scene = GetScene();

		if(scene)
		{
#ifdef URHO3D_PHYSICS
			//todo
#endif
#ifdef URHO3D_URHO2D
			if(!ret)
				//todo
#endif
		}
	}

	void Component::DoAutoRemove(AutoRemoveNode node)
	{

	}
}