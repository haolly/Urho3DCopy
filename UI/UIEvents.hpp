//
// Created by liuhao on 2019-02-27.
//

#ifndef URHO3DCOPY_UIEVENTS_HPP
#define URHO3DCOPY_UIEVENTS_HPP

#include "../Core/Object.h"

namespace Urho3D
{

	URHO3D_EVENT(E_UIMOUSECLICK, UIMouseClick)
	{
		URHO3D_PARAM(P_ELEMENT, Element);
		URHO3D_PARAM(P_X, X);
		URHO3D_PARAM(P_Y, Y);
		URHO3D_PARAM(P_BUTTON, Button);
		URHO3D_PARAM(P_BUTTONS, Buttons);
		URHO3D_PARAM(P_QUALIFIERS, Qualifiers);
	}

	URHO3D_EVENT(E_ELEMENTREMOVED, ElementRemoved)
	{
		URHO3D_PARAM(P_ROOT, Root);
		URHO3D_PARAM(P_PARENT, Parent);
		URHO3D_PARAM(P_ELEMENT, Element);
	}
}

#endif //URHO3DCOPY_UIEVENTS_HPP
