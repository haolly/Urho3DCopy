//
// Created by LIUHAO on 2017/12/7.
//

#ifndef URHO3DCOPY_COREEVENT_H
#define URHO3DCOPY_COREEVENT_H

#include "Object.h"

namespace Urho3D
{
	//Note, eventId is in Urho3D namespace, but event param is under namespace eventName
	URHO3D_EVENT(E_BEGINFRAME, BeginFrame)
	{
		URHO3D_PARAM(P_FRAMENUMBER, FrameNumber);   //unsigned
		URHO3D_PARAM(P_TIMESTEP, TimeStep);         //float
	}

	URHO3D_EVENT(E_UPDATE, Update)
	{
		URHO3D_PARAM(P_TIMESTEP, TimeStep);         // float
	}

	URHO3D_EVENT(E_RENDERUPDATE, RenderUpdate)
	{
		URHO3D_PARAM(P_TIMESTEP, TimeStep);
	}

	URHO3D_EVENT(E_ENDFRAME, EndFrame)
	{
	}

	URHO3D_EVENT(E_WORKITEMCOMPLETED, WorkItemCompleted)
	{
		URHO3D_PARAM(P_ITEM, Item);
	}

	URHO3D_EVENT(E_SCENEDRAWABLEUPDATEFINISHED, SceneDrawableUpdateFinished)
	{
		URHO3D_PARAM(P_SCENE, Scene);
		URHO3D_PARAM(P_TIMESTEP, TimeStep);
	}
}

#endif //URHO3DCOPY_COREEVENT_H
