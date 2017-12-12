//
// Created by LIUHAO on 2017/12/12.
//

#ifndef URHO3DCOPY_IOEVENT_H
#define URHO3DCOPY_IOEVENT_H

#include "../Core/Object.h"

namespace Urho3D
{
	URHO3D_EVENT(E_LOGMESSAGE, LogMessage)
	{
		URHO3D_PARAM(P_MESSAGE, Message);
		URHO3D_PARAM(P_LEVEL, Level);
	}
}

#endif //URHO3DCOPY_IOEVENT_H
