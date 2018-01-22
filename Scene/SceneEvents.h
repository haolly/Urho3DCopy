//
// Created by liuhao on 2018/1/6.
//

#ifndef URHO3DCOPY_SCENEEVENTS_H
#define URHO3DCOPY_SCENEEVENTS_H

#include "../Core/Object.h"

namespace Urho3D
{
	URHO3D_EVENT(E_ATTRIBUTEANIMATIONADDED, AttributeAnimationAdded)
	{
		URHO3D_PARAM(P_OBJECTANIMATION, ObjectAnimation);
		URHO3D_PARAM(P_ATTRIBUTEANIMATIONNAME, AttributeAnimationName);
	}

	URHO3D_EVENT(E_ATTRIBUTEANIMATIONREMOVED, AttributeAnimationRemoved)
	{
		URHO3D_PARAM(P_OBJECTANIMATION, ObjectAnimation);
		URHO3D_PARAM(P_ATTRIBUTEANIMATIONNAME, AttributeAnimationName);
	}
}


#endif //URHO3DCOPY_SCENEEVENTS_H
