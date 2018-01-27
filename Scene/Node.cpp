//
// Created by liuhao on 2018/1/17.
//

#include "Node.h"
#include "../Scene/Component.h"
#include "SceneEvents.h"
#include "../Scene/Scene.h"
#include "UnknownComponent.h"

namespace Urho3D
{

	Node::Node(Context *context) :
			Animatable(context),
			worldTransform_(Matrix3x4::IDENTITY),
			dirty_(false),
			enabled_(true),
			enabledPrev_(true),
			networkUpdate_(false),
			parent_(nullptr),
			scene_(nullptr),
			id_(0),
			position_(Vector3::ZERO),
			rotation_(Quaternion::IDENTITY),
			scale_(Vector3::ONE),
			worldRotation_(Quaternion::IDENTITY)
	{
		impl_ = new NodeImpl();
		impl_->owner_ = nullptr;
	}

	Node::~Node()
	{
		RemoveAllChildren();
		RemoveAllComponents();
		if(scene_)
			scene_->NodeRemoved(this);
	}

	void Node::RegisterObject(Context *context)
	{
		context->RegisterFactory<Node>();

		URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Name", GetName, SetName, String, String::EMPTY, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Tags", GetTags, SetTags, StringVector, Variant::emptyStringVector, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Position", GetPosition, SetPosition, Vector3, Vector3::ZERO, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Rotation", GetRotation, SetRotation, Quaternion, Quaternion::IDENTITY, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Scale", GetScale, SetScale, Vector3, Vector3::ONE, AM_DEFAULT);
		URHO3D_ATTRIBUTE("Variables", VariantMap, vars_, Variant::emptyVariantMap, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Network Position", GetNetPositionAttr, SetNetPositionAttr, Vector3, Vector3::ZERO,
								AM_NET | AM_LATESTDATA | AM_NOEDIT);
		URHO3D_ACCESSOR_ATTRIBUTE("Network Rotation", GetNetRotationAttr, SetNetRotationAttr, PODVector<unsigned char>,
								Variant::emptyBuffer, AM_NET | AM_LATESTDATA | AM_NOEDIT);
		URHO3D_ACCESSOR_ATTRIBUTE("Network Parent Node", GetNetParentAttr, SetNetParentAttr, PODVector<unsigned char>,
								Variant::emptyBuffer, AM_NET | AM_NOEDIT);
//		context->RegisterAttribute<ClassName>(
//				Urho3D::AttributeInfo(
//						Urho3D::GetVariantType<bool >(),
//						"Is Enabled",
//						Urho3D::MakeVariantAttributeAccessor<ClassName>(
//								[](const ClassName& self, Urho3D::Variant& value) { value = self.IsEnabled(); },
//								[](ClassName& self, const Urho3D::Variant& value) { self.SetEnabled(value.Get<bool>()); }),
//						nullptr,
//						true,
//						AM_DEFAULT
//				)
//		);

	}

	bool Node::Load(Deserializer &source, bool setInstanceDefault)
	{
		SceneResolver resolver;

		// Read own ID. Will  not be applied, only stored for resolving possible references
		unsigned nodeID = source.ReadUInt();
		resolver.AddNode(nodeID, this);

		bool success = Load(source, resolver);
		if(success)
		{
			resolver.Resolve();
			ApplyAttributes();
		}
		return success;
	}

	bool Node::LoadXML(const XMLElement &source, bool setInstanceDefault)
	{
		SceneResolver resolver;
		unsigned nodeID = source.GetUInt("id");
		resolver.AddNode(nodeID, this);

		// Read attributes, components and child nodes
		bool success = LoadXML(source, resolver);
		if(success)
		{
			resolver.Resolve();
			ApplyAttributes();
		}
		return success;
	}

	bool Node::Save(Serializer &dest) const
	{
		if(!dest.WriteUInt(id_))
			return false;

		// Write attributes
		if(!Animatable::Save(dest))
			return false;

		// Write components
		dest.WriteVLE(GetNumPersistentComponents());
		for(unsigned i=0; i< components_.Size(); ++i)
		{
			Component* component = components_[i];
			if(component->IsTemporary())
				continue;

			VectorBuffer compBuffer;
			if(!component->Save(compBuffer))
				return false;

			dest.WriteVLE(compBuffer.GetSize());
			dest.Write(compBuffer.GetData(), compBuffer.GetSize());
		}

		// Write child nodes
		dest.WriteVLE(GetNumPersistentChildren());
		for(unsigned i=0; i< children_.Size(); ++i)
		{
			Node* node = children_[i];
			if(node->IsTemporary())
				continue;
			if(!node->Save(dest))
				return false;
		}
		return true;
	}

	bool Node::SaveXML(XMLElement &dest) const
	{
		if(!dest.SetUInt("id", id_))
			return false;

		// Write attributes and animation
		if(!Animatable::SaveXML(dest))
			return false;

		// Write components
		for(unsigned i=0; i< components_.Size(); ++i)
		{
			Component* component = components_[i];
			if(component->IsTemporary())
				continue;

			XMLElement compElem = dest.CreateChild("component");
			if(!component->SaveXML(compElem))
				return false;
		}

		//Write children
		for(unsigned i=0; i<children_.Size(); ++i)
		{
			Node* node = children_[i];
			if(node->IsTemporary())
				continue;

			XMLElement childElem = dest.CreateChild("node");
			if(!node->SaveXML(childElem))
				return false;
		}
		return true;
	}

	void Node::ApplyAttributes()
	{
		for(unsigned i=0; i< components_.Size(); ++i)
			components_[i]->ApplyAttributes();

		for(unsigned i=0; i< children_.Size(); ++i)
			children_[i]->ApplyAttributes();
	}

	void Node::MarkNetworkUpdate()
	{
		if(!networkUpdate_ && scene_ && id_ < FIRST_LOCAL_ID)
		{
			scene_->MarkNetworkUpdate(this);
			networkUpdate_ = false;
		}
	}

	void Node::AddReplicationState(NodeReplicationState *state)
	{

	}

	bool Node::SaveXML(Serializer &dest, const String &indentation) const
	{
		SharedPtr<XMLFile> xml(new XMLFile(context_));
		XMLElement rootElem = xml->CreateRoot("node");
		// Save to XMLElement
		if(!SaveXML(rootElem))
			return false;
		// Real save
		return xml->Save(dest, indentation);
	}

	bool Node::SetName(const String &name)
	{
		if(name != impl_->name_)
		{
			impl_->name_ = name;
			impl_->nameHash_ = name;

			MarkNetworkUpdate();

			if(scene_)
			{
				using namespace NodeNameChanged;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_SCENE] = scene_;
				eventData[P_NODE] = this;

				scene_->SendEvent(E_NODENAMECHANGED, eventData);
			}
		}
	}

	void Node::SetTags(const StringVector &tags)
	{
		RemoveAllTags();
		AddTags(tags);
	}

	void Node::AddTag(const String &tag)
	{
		if(tag.Empty() || HasTag(tag))
			return;

		impl_->tags_.Push(tag);

		// Cache
		scene_->NodeTagAdded(this, tag);

		using namespace NodeTagAdded;
		VariantMap& eventData = GetEventDataMap();
		eventData[P_SCENE] = scene_;
		eventData[P_NODE] = this;
		eventData[P_TAG] = tag;
		scene_->SendEvent(E_NODETAGADDED, eventData);

		//Sync
		MarkNetworkUpdate();
	}

	void Node::AddTags(const String &tags, char separator)
	{
		StringVector tagVector = tags.Split(separator);
		AddTags(tagVector);
	}

	void Node::AddTags(const StringVector &tags)
	{
		for(unsigned i=0; i< tags.Size(); ++i)
			AddTag(tags[i]);
	}

	bool Node::RemoveTag(const String &tag)
	{
		bool removed = impl_->tags_.Remove(tag);

		if(!removed)
			return false;

		if(scene_)
		{
			scene_->NodeTagRemoved(this, tag);
			using namespace NodeTagRemoved;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene_;
			eventData[P_NODE] = this;
			eventData[P_TAG] = tag;
			scene_->SendEvent(E_NODETAGREMOVED, eventData);
		}

		MarkNetworkUpdate();
	}

	void Node::RemoveAllTags()
	{
		// Remove old scene cache
		if(scene_)
		{
			for(unsigned i=0; i< impl_->tags_.Size(); ++i)
			{
				scene_->NodeTagRemoved(this, impl_->tags_[i]);

				using namespace NodeTagRemoved;
				VariantMap& eventData = GetEventDataMap();
				eventData[P_SCENE] = scene_;
				eventData[P_NODE] = this;
				eventData[P_TAG] = impl_->tags_[i];
				scene_->SendEvent(E_NODETAGREMOVED, eventData);
			}
		}

		impl_->tags_.Clear();

		// Sync
		MarkNetworkUpdate();
	}

	void Node::SetPosition(const Vector3 &position)
	{
		position_ = position;
		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::SetRotation(const Quaternion &rotation)
	{
		rotation_ = rotation;
		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::SetDirection(const Vector3 &direction)
	{
		SetRotation(Quaternion(Vector3::FORWARD, direction));
	}

	void Node::SetScale(float scale)
	{
		SetScale(Vector3(scale, scale, scale));
	}

	void Node::SetScale(const Vector3 &scale)
	{
		scale_ = scale;
		//todo, compare float value is not accuracy
		if(scale_.x_ == 0.0f)
			scale_.x_ = M_EPSILON;
		if(scale_.y_ == 0.0f)
			scale_.y_ = M_EPSILON;
		if(scale_.z_ == 0.0f)
			scale_.z_ = M_EPSILON;

		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::SetTransform(const Vector3 &position, const Quaternion &rotation)
	{
		position_ = position;
		rotation_ = rotation;
		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::SetTransform(const Vector3 &position, const Quaternion &rotation, float scale)
	{
		SetTransform(position, rotation, Vector3(scale, scale, scale));
	}

	void Node::SetTransform(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
	{
		position_ = position;
		rotation_ = rotation;
		scale_ = scale_;

		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::SetTransform(const Matrix3x4 &matrix)
	{
		SetTransform(matrix.Translation(), matrix.Rotation(), matrix.Scale());
	}

	void Node::SetWorldPosition(const Vector3 &position)
	{
		SetPosition((parent_ == scene_ || !parent_) ? position : parent_->GetWorldTransform().Inverse() * position);
	}

	void Node::SetWorldRotation(const Quaternion &rotation)
	{
		SetRotation((parent_ == scene_ || !parent_) ? rotation : parent_->GetWorldRotation().Inverse() * rotation);
	}

	void Node::SetWorldDirection(const Vector3 &direction)
	{
		Vector3 localDirection = (parent_ == scene_ || !parent_) ? direction : parent_->GetWorldRotation().Inverse() * direction;
		SetRotation(Quaternion(Vector3::FORWARD, localDirection));
	}

	void Node::SetWorldScale(float scale)
	{
		SetWorldScale(Vector3(scale, scale, scale));
	}

	void Node::SetWorldScale(const Vector3 &scale)
	{
		SetScale((parent_ == scene_ || !parent_) ? scale : scale / parent_->GetWorldScale());
	}

	void Node::SetWorldTransform(const Vector3 &position, const Quaternion &rotation)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
	}

	void Node::SetWorldTransform(const Vector3 &position, const Quaternion &rotation, float scale)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	void Node::SetWorldTransform(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
	{
		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	void Node::Translate(const Vector3 &delta, TransformSpace space)
	{
		switch (space)
		{
			case TS_LOCAL:
				// Note: local space translation disregards local scale for scale-independent movement speed
				position_ += rotation_ * delta;
				break;
			case TS_PARENT:
				position_ += delta;
				break;
			case TS_WORLD:
				position_ += (parent_ == scene_ || !parent_) ? delta : parent_->GetWorldTransform().Inverse() * Vector4(delta, 0.0f);
				break;
		}

		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::Rotate(const Quaternion &delta, TransformSpace space)
	{
		switch (space)
		{
			case TS_LOCAL:
				rotation_ = (rotation_ * delta).Normalized();
				break;
			case TS_PARENT:
				rotation_ = (delta * rotation_).Normalized();
				break;
			case TS_WORLD:
				if((parent_ == scene_ || !parent_))
					rotation_ = (delta * rotation_).Normalized();
				else
				{
					Quaternion worldRotation = GetWorldRotation();
					rotation_ = rotation_ * worldRotation.Inverse() * delta * worldRotation;
				}
				break;
		}

		MarkDirty();
		MarkNetworkUpdate();
	}

	void Node::RotateAround(const Vector3 &point, const Quaternion &delta, TransformSpace space)
	{

	}

	void Node::Pitch(float angle, TransformSpace space)
	{
		Rotate(Quaternion(angle, Vector3::RIGHT), space);
	}

	void Node::Yaw(float angle, TransformSpace space)
	{
		Rotate(Quaternion(angle, Vector3::UP), space);
	}

	void Node::Roll(float angle, TransformSpace space)
	{
		Rotate(Quaternion(angle, Vector3::FORWARD), space);
	}

	bool Node::LookAt(const Vector3 &target, const Vector3 &up, TransformSpace space)
	{
		return false;
	}

	void Node::Scale(float scale)
	{

	}

	void Node::Scale(const Vector3 &scale)
	{

	}

	void Node::SetEnabled(bool enable)
	{

	}

	void Node::SetDeepEnabled(bool enable)
	{

	}

	void Node::ResetDeepEnabled()
	{

	}

	void Node::SetEnabledRecursive(bool enable)
	{

	}

	void Node::SetOwner(Connection *owner)
	{

	}

	void Node::MarkDirty()
	{
		Node* cur = this;
		for(;;)
		{
			// Precondition:
			// a) whenever a node is marked dirty, all its children are marked dirty as well.
			// b) whenever a node is cleared from being dirty, all its parents must have been cleared as well
			// Therefore if we are recursing here to mark this node dirty, and it already was, then all children of this
			// node must also be already dirty, an we don't need to reflag them again

			if(cur->dirty_)
				return;
			cur->dirty_ = true;

			// Notify listener components first, then mark child nodes
			for(auto it = cur->listeners_.Begin(); it != cur->listeners_.End(); ++it)
			{
				Component* c = *it;
				if(c)
				{
					c->OnMarkedDirty(cur);
					++it;
				}
					// if listener has expired, erase from list(swap with the last element to avoid O(n^2) behavior)
				else
				{
					*it = cur->listeners_.Back();
					cur->listeners_.Pop();
				}
			}

			// Depth traversal
			// Tail call optimization: Don't recurse to mark the first child dirty, but instead process it in the context
			// of the current function. If there are more than one child, then recurse to the excess children
			auto it = cur->children_.Begin();
			if(it != cur->children_.End())
			{
				Node* next = *it;
				for(++it; it != cur->children_.End(); ++it)
					(*it)->MarkDirty();
				cur = next;
			}
			else
				return;
		}
	}

	Node *Node::CreateChild(const String &name, CreateMode mode, unsigned int id, bool temporary)
	{
		Node* newNode = CreateChild(id, mode, temporary);
		newNode->SetName(name);
		return newNode;
	}

	Node *Node::CreateTemporaryChild(const String &name, CreateMode mode, unsigned int id)
	{
		return CreateChild(name, mode, id, true);
	}

	void Node::AddChild(Node *node, unsigned int index)
	{
		if(!node || node == this || node->parent_ == this)
			return;
		if(IsChildOf(node))
			return;

		SharedPtr<Node> nodeShared(node);
		Node* oldParent = node->parent_;
		if(oldParent)
		{
			if(oldParent->GetScene() != scene_)
				oldParent->RemoveChild(node);
			else
			{
				if(scene_)
				{
					// do not remove from the scene during reparenting, just send the necessary change event
					using namespace NodeRemoved;
					VariantMap& eventData = GetEventDataMap();
					eventData[P_SCENE] = scene_;
					eventData[P_PARENT] = oldParent;
					eventData[P_NODE] = node;

					scene_->SendEvent(E_NODEREMOVED, eventData);
				}

				// then perform remove
				oldParent->children_.Remove(nodeShared);
			}
		}
		// Add to the child vector, then add to the scene if not added yet
		children_.Insert(index, nodeShared);
		if(scene_ && node->GetScene() != scene_)
			scene_->NodeAdded(node);
		node->parent_ = this;
		node->MarkDirty();
		node->MarkNetworkUpdate();

		// If the child node has components, also mark network update on them to ensure they have a valid NetworkState
		for(auto it = node->components_.Begin(); it != node->components_.End(); ++it)
		{
			(*it)->MarkNetworkUpdate();
		}

		// Send change event
		if(scene_)
		{
			using namespace NodeAdded;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene_;
			eventData[P_PARENT] = this;
			eventData[P_NODE] = node;

			scene_->SendEvent(E_NODEADDED,eventData);
		}
	}

	void Node::RemoveChild(Node *node)
	{
		if(!node)
			return;

		for(auto it = children_.Begin(); it != children_.End(); ++it)
		{
			if(*it == node)
			{
				RemoveChild(it);
				return;
			}
		}
	}

	void Node::RemoveAllChildren()
	{
		RemoveChildren(true, true, true);
	}

	void Node::RemoveChildren(bool removeReplicated, bool removeLocal, bool recursive)
	{
		unsigned numRemoved = 0;
		for(unsigned i= children_.Size() -1; i < children_.Size(); --i)
		{
			bool remove = false;
			Node* childNode = children_[i];
			if(recursive)
				childNode->RemoveChildren(removeReplicated, removeLocal, true);
			if(childNode->GetID() < FIRST_LOCAL_ID && removeReplicated)
				remove = true;
			else if(childNode->GetID() >= FIRST_LOCAL_ID && removeLocal)
				remove = true;
			if(remove)
			{
				RemoveChild(children_.Begin() + 1);
				++numRemoved;
			}
		}

		if(numRemoved)
			MarkReplicationDirty();
	}

	Component *Node::CreateComponent(StringHash type, CreateMode mode, unsigned int id)
	{
		if(id_ >= FIRST_LOCAL_ID && mode == REPLICATED)
			mode = LOCAL;

		SharedPtr<Component> newComponent = DynamicCast<Component>(context_->CreateObject(type));
		if(!newComponent)
		{
			URHO3D_LOGERROR("Could not create unknown component type " + type.ToString());
			return nullptr;
		}

		AddComponent(newComponent, id, mode);
		return newComponent;
	}

	Component *Node::GetOrCreateComponent(StringHash type, CreateMode mode, unsigned int id)
	{
		return nullptr;
	}

	Component *Node::CloneComponent(Component *component, unsigned int id)
	{
		return nullptr;
	}

	Component *Node::CloneComponent(Component *component, CreateMode mode, unsigned int id)
	{
		return nullptr;
	}

	void Node::RemoveComponent(Component *component)
	{
		auto it = components_.Find(component);
		if(it != components_.End())
		{
			RemoveComponent(it);

			MarkReplicationDirty();
		}
	}

	void Node::RemoveComponent(StringHash type)
	{

	}

	void Node::OnAttributeAnimationAdded()
	{

	}

	void Node::OnAttributeAnimationRemoved()
	{

	}

	Animatable *Node::FindAttributeAnimationTarget(const String &name, String &outName)
	{
		return Animatable::FindAttributeAnimationTarget(name, outName);
	}

	void Node::SetEnabled(bool enable, bool recursive, bool storeSelf)
	{

	}

	Component *Node::SafeCreateComponent(const String &typeName, StringHash type, CreateMode mode, unsigned id)
	{
		if(id_ >= FIRST_LOCAL_ID && mode == REPLICATED)
			mode = LOCAL;

		if(!context_->GetTypeName(type).Empty())
			return CreateComponent(type, mode, id);
		else
		{
			URHO3D_LOGWARNING("Component type " + type.ToString() + " not know, creating UnknowComponent as placeholder");
			SharedPtr<UnknownComponent> newComponent(new UnknowComponent(context_));
			//todo
		}
	}

	void Node::UpdateWorldTransform() const
	{
		Matrix3x4 transform = GetTransform();

		if(parent_ == scene_ || !parent_)
		{
			worldTransform_ = transform;
			worldRotation_ = rotation_;
		}
		else
		{
			worldTransform_ = parent_->GetWorldTransform() * transform;
			worldRotation_ = parent_->GetWorldTransform() * rotation_;
		}

		dirty_ = false;
	}

	//todo, because child is an SharedPtr so we do not need to free the memory it use ?
	void Node::RemoveChild(Vector::Iterator it)
	{
		// Keep a shared pointer to the child about to be removed, to make sure the erase from the container completes first.
		// Otherwise it would be possible that other child nodes get removed as part of the nodes's components' cleanup, causing
		// a re-entrant erase and a crash
		SharedPtr<Node> child(*it);
		if(Refs() > 0 && scene_)
		{
			using namespace NodeRemoved;

			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene_;
			eventData[P_PARENT] = this;
			eventData[P_NODE] = child;

			scene_->SendEvent(E_NODEREMOVED, eventData);
		}

		child->parent_ = nullptr;
		child->MarkDirty();
		child->MarkNetworkUpdate();
		if(scene_)
			scene_->NodeRemoved(child);
		children_.Erase(it);
	}

	void Node::GetChildrenRecursive(PODVector<Node *> &dest) const
	{

	}

	void Node::GetChildrenWithComponentRecursive(PODVector<Node *> &dest, StringHash type) const
	{

	}

	void Node::GetChildrenWithTagRecursive(PODVector<Node *> &dest, const String &tag) const
	{

	}

	void Node::GetComponentsRecursive(PODVector<Component *> &dest, StringHash type) const
	{

	}

	Node *Node::CloneRecursive(Node *parent, SceneResolver &resolver, CreateMode mode)
	{
		return nullptr;
	}

	void Node::RemoveComponent(Vector<SharedPtr<Component>>::Iterator it)
	{
		if(Refs() > 0 && scene_)
		{
			using namespace ComponentRemoved;
			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene_;
			eventData[P_NODE] = this;
			eventData[P_COMPONENT] = (*it).Get();

			scene_->SendEvent(E_COMPONENTREMOVED, eventData);
		}

		RemoveListener(*it);
		if(scene_)
			scene_->ComponentRemoved(*it);
		(*it)->SetNode(nullptr);
		components_.Erase(it);
	}

	void Node::HanldeAttributeAnimationUpdate(StringHash eventType, VariantMap &eventData)
	{

	}

	void Node::RemoveComponents(bool removeReplicated, bool removeLocal)
	{
		unsigned numRemoved = 0;
		for(unsigned i= components_.Size() -1; i < components_.Size(); --i)
		{
			bool remove = false;
			Component* component = components_[i];

			if(component->GetID() < FIRST_LOCAL_ID && removeReplicated)
				remove = true;
			else if(component->GetID() >= FIRST_LOCAL_ID && removeLocal)
				remove = true;

			if(remove)
			{
				RemoveComponent(components_.Begin() + i);
				++numRemoved;
			}
		}

		if(numRemoved)
			MarkReplicationDirty();
	}

	void Node::RemoveComponents(StringHash type)
	{

	}

	void Node::RemoveAllComponents()
	{
		RemoveComponents(true, true);
	}

	void Node::ReorderComponent(Component *component, unsigned index)
	{

	}

	Node *Node::Clone(CreateMode mode)
	{
		return nullptr;
	}

	void Node::Remove()
	{

	}

	bool Node::HasTag(const String &tag) const
	{
		return false;
	}

	bool Node::IsChildOf(Node *node) const
	{
		return false;
	}

	bool Node::Load(Deserializer &source, SceneResolver &resolver, bool loadChildren, bool rewriteIDs, CreateMode mode)
	{
		RemoveAllChildren();
		RemoveAllComponents();

		if(!Animatable::Load(source))
			return false;
		unsigned numComponents = source.ReadVLE();
		for(unsigned i=0; i< numComponents; ++i)
		{
			VectorBuffer compBuffer(source, source.ReadVLE());
			StringHash comType = compBuffer.ReadStringHash();
			unsigned compID = compBuffer.ReadUInt();

			Component* newComponent = SafeCreateComponent(String::EMPTY, comType,
			                                              (mode == REPLICATED && compID < FIRST_LOCAL_ID) ? REPLICATED : LOCAL,
			                                              rewriteIDs ? 0 : compID);
			if(newComponent)
			{
				resolver.AddComponent(compID, newComponent);
				// Do not abort if component fails to load, as the component buffer is nested and we can skip to the next
				newComponent->Load(compBuffer);
			}
		}

		if(!loadChildren)
			return true;

		unsigned numChildren = source.ReadVLE();
		for(unsigned i=0; i< numChildren; ++i)
		{
			unsigned nodeID = source.ReadUInt();
			Node* newNode = CreateChild(rewriteIDs ? 0 : nodeID, (mode == REPLICATED && nodeID < FIRST_LOCAL_ID) ? REPLICATED : LOCAL);
			resolver.AddNode(nodeID, newNode);

			if(!newNode->Load(source, resolver, loadChildren, rewriteIDs, mode))
				return false;
		}
		return true;
	}

	bool Node::LoadXML(const XMLElement &source, SceneResolver &resolver, bool loadChildren, bool rewriteIDs,
	                   CreateMode mode)
	{
		RemoveAllChildren();
		RemoveAllComponents();

		if(!Animatable::LoadXML(source))
			return false;

		XMLElement compElem = source.GetChild("component");
		while(compElem)
		{
			String typeName = compElem.GetAttribute("type");
			unsigned compID = compElem.GetUInt("id");
			Component* newComponent = SafeCreateComponent(typeName, StringHash(typeName),
			                                              (mode == REPLICATED && compID < FIRST_LOCAL_ID) ? REPLICATED : LOCAL,
															rewriteIDs ? 0 : compID);
			if(newComponent)
			{
				resolver.AddComponent(compID, newComponent);
				if(!newComponent->LoadXML(compElem))
					return false;
			}
			compElem = compElem.GetNext("component");
		}

		if(!loadChildren)
			return true;
		XMLElement childElem = source.GetChild("node");
		while (childElem)
		{
			unsigned nodeID = childElem.GetUInt("id");
			Node* newNode = CreateChild(rewriteIDs ? 0 : nodeID, (mode == REPLICATED && nodeID < FIRST_LOCAL_ID) ? REPLICATED: LOCAL);
			resolver.AddNode(nodeID, newNode);
			if(!newNode->LoadXML(childElem, resolver, loadChildren, rewriteIDs, mode))
				return false;

			childElem = childElem.GetNext("node");
		}
		return true;
	}

	void Node::PrepareNetworkUpdate()
	{

	}

	void Node::CleanupConnection(Connection *connection)
	{

	}

	void Node::MarkReplicationDirty()
	{
		if(networkState_)
		{
			for(auto it = networkState_->replicationStates_.Begin(); it != networkState_->replicationStates_.End(); ++it)
			{
				//todo
			}
		}
	}

	Node *Node::CreateChild(unsigned id, CreateMode mode, bool temporary)
	{
		//Note , SharedPtr DO NOT NEED allocate , ie, call new() on it
		//ref https://www.quora.com/Whats-the-difference-between-passing-a-shared_ptr-by-value-and-by-const-reference
		//ref https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/ not read yet :(
		//ref https://herbsutter.com/2013/05/29/gotw-89-solution-smart-pointers/ not read yet :(
		SharedPtr<Node> newNode(new Node(context_));
		newNode->SetTemporary(temporary);

		if(scene_)
		{
			if(!id || scene_->GetNode(id))
				id = scene_->GetFreeNodeID(mode);
			newNode->SetID(id);
		}
		else
			newNode->SetID(id);

		AddChild(newNode);
		return newNode;
	}

	void Node::AddComponent(Component *component, unsigned id, CreateMode mode)
	{
		if(!component)
			return;

		components_.Push(SharedPtr<Component>(component));
		if(component->GetNode())
			URHO3D_LOGWARNING("Component " + component->GetTypeName() + " already belongs to a node");

		component->SetNode(this);

		if(scene_)
		{
			if(!id || scene_->GetComponent(id))
				id = scene_->GetFreeComponentID(mode);
			component->SetID(id);
			scene_->ComponentAdded(component);
		}
		else
		{
			component->SetID(id);
		}

		component->OnMarkedDirty(this);

		// Check attributes of the new component on next network update, and mark node dirty in all replication state
		component->MarkNetworkUpdate();
		MarkNetworkUpdate();
		MarkReplicationDirty();

		if(scene_)
		{
			using namespace ComponentAdded;

			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene_;
			eventData[P_NODE] = this;
			eventData[P_COMPONENT] = component;

			scene_->SendEvent(E_COMPONENTADDED, eventData);
		}

	}

	unsigned Node::GetNumPersistentChildren() const
	{
		return 0;
	}

	unsigned Node::GetNumPersistentComponents() const
	{
		return 0;
	}

	void Node::SetTransformSilent(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
	{

	}
}