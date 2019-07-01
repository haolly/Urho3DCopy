//
// Created by LIUHAO on 2017/12/12.
//

#ifndef URHO3DCOPY_ENGINEEVENTS_H
#define URHO3DCOPY_ENGINEEVENTS_H

#include "../Core/Object.h"

namespace Urho3D
{
	URHO3D_EVENT(E_CONSOLECOMMAND, ConsoleCommand)
	{
		URHO3D_PARAM(P_COMMAND, Command);
		URHO3D_PARAM(P_ID, Id);
	}

}

#endif //URHO3DCOPY_ENGINEEVENTS_H
