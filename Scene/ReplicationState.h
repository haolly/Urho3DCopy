//
// Created by liuhao1 on 2018/1/24.
//

#ifndef URHO3DCOPY_REPLICATIONSTATE_H
#define URHO3DCOPY_REPLICATIONSTATE_H

#include "../Core/Variant.h"
#include "../Core/Attribute.h"

namespace Urho3D
{
	class Component;
	class Node;
	class Scene;

	struct ReplicationState;


	struct NetworkState
	{
		NetworkState() :
				interceptMask_(0)
		{
		}

		// Cached network attribute infos
		const Vector<AttributeInfo>* attributes_;
		Vector<Variant> currentValues_;
		Vector<Variant> previousValues_;
		PODVector<ReplicationState*> replicationStates_;
		VariantMap previousVars_;
		unsigned long long interceptMask_;
	};



}


#endif //URHO3DCOPY_REPLICATIONSTATE_H
