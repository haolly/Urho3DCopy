//
// Created by liuhao1 on 2018/1/23.
//

#include "../Scene/SceneResolver.h"
#include "../Math/StringHash.h"
#include "../Container/HashSet.h"
#include "../Scene/Component.h"
#include "../Scene/Node.h"

namespace Urho3D
{

	SceneResolver::SceneResolver()
	{

	}

	SceneResolver::~SceneResolver()
	{

	}

	void SceneResolver::Reset()
	{
		nodes_.Clear();
		components_.Clear();
	}

	void SceneResolver::AddNode(unsigned oldID, Node *node)
	{
		if(node)
			nodes_[oldID] = node;
	}

	void SceneResolver::AddComponent(unsigned oldID, Component *component)
	{
		if(component)
			components_[oldID] = component;
	}

	// todo, why need this ?
	void SceneResolver::Resolve()
	{
		HashSet<StringHash> noIDAttributes;
		for(auto it = components_.Begin(); it != components_.End(); ++it)
		{
			Component* component = it->second_;
			if(!component || noIDAttributes.Contains(component->GetType()))
				continue;

			//todo, one component could noly have one ID attribute ??
			bool hasIDAttributes = false;

			const Vector<AttributeInfo>* attributes = component->GetAttributes();
			if(!attributes)
			{
				noIDAttributes.Insert(component->GetType());
				continue;
			}

			for(unsigned k = 0; k< attributes->Size(); ++k)
			{
				const AttributeInfo& info = attributes->At(k);
				// Attribute is an node id
				if(info.mode_ & AM_NODEID)
				{
					hasIDAttributes = true;
					unsigned oldNodeID = component->GetAttribute(k).GetUInt();
					//todo, why need update ?? who changed it ? when ? why ?

					if(oldNodeID)
					{
						auto iter = nodes_.Find(oldNodeID);
						if(iter != nodes_.End() && iter->second_)
						{
							unsigned newNodeID = iter->second_->GetID();
							component->SetAttribute(k, Variant(newNodeID));
						}
						else
							URHO3D_LOGERROR("Could not resolve node ID " + String(oldNodeID));
					}
				}
				else if(info.mode_ & AM_COMPONENTID)
				{
					hasIDAttributes = true;
					unsigned oldComponentID = component->GetAttribute(k).GetUInt();
					if(oldComponentID)
					{
						auto iter = components_.Find(oldComponentID);
						if(iter != components_.End() && iter->second_)
						{
							unsigned newComponentID = iter->second_->GetID();
							component->SetAttribute(k, Variant(newComponentID));
						}
						else
							URHO3D_LOGERROR("Could not resolve component ID " + String(oldComponentID));
					}
				}
				else if(info.mode_ & AM_NODEIDVECTOR)
				{
					hasIDAttributes = true;
					Variant attrValue = component->GetAttribute(k);
					const VariantVector& oldNodeIDs = attrValue.GetVariantVector();

					if(oldNodeIDs.Size())
					{
						// The first index stores the number of IDs redundantly. This is for editing
						unsigned numIDs = oldNodeIDs[0].GetUInt();
						VariantVector newIDs;
						newIDs.Push(numIDs);

						for(unsigned idx = 1; idx < oldNodeIDs.Size(); ++idx)
						{
							unsigned oldNodeID = oldNodeIDs[idx].GetUInt();
							auto iter = nodes_.Find(oldNodeID);

							if(iter != nodes_.End() && iter->second_)
							{
								newIDs.Push(iter->second_->GetID());
							}
							else
							{
								newIDs.Push(0);
								URHO3D_LOGWARNING("Could not resolve node ID " + String(oldNodeID));
							}
						}
						component->SetAttribute(k, Variant(newIDs));
					}
				}
			}

			if(!hasIDAttributes)
				noIDAttributes.Insert(component->GetType());
		}

		Reset();
	}
}
