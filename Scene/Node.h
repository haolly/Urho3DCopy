//
// Created by liuhao on 2018/1/17.
//

#ifndef URHO3DCOPY_NODE_H
#define URHO3DCOPY_NODE_H

#include "Animatable.h"

namespace Urho3D
{
	class Node : public Animatable
	{
		URHO3D_OBJECT(Node, Animatable);

		friend class Connection;

	public:
		Node(Context* context);
		virtual ~Node() override;
		static void RegisterObject(Context* context);

		virtual bool Load(Deserializer& source, bool setInstanceDefault = false) override;
		virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false) override;
		virtual bool Save(Serializer& dest) const override ;
		virtual bool SaveXML(XMLElement& dest) const override ;
		virtual void ApplyAttributes() override ;

		virtual bool SaveDefaultAttributes() const override { return true; }
		virtual void MarkNetworkUpdate() override ;
		virtual void AddReplicationState(NodeReplicationState* state);

		bool SaveXML(Serializer& dest, const String& indentation = "\t") const;
		bool SetName(const String& name);

		void SetTags(const StringVector& tags);
		void AddTag(const String& tags, char separator = ':');
	};

}


#endif //URHO3DCOPY_NODE_H
