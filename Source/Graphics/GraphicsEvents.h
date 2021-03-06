//
// Created by liuhao1 on 2018/1/19.
//

#ifndef URHO3DCOPY_GRAPHICSEVENTS_H
#define URHO3DCOPY_GRAPHICSEVENTS_H

#include "../Core/Object.h"

namespace Urho3D
{
	URHO3D_EVENT(E_SCREENMODE, ScreenMode)
	{
		URHO3D_PARAM(P_WIDTH, Width);
		URHO3D_PARAM(P_HEIGHT, Height);
		URHO3D_PARAM(P_FULLSCREEN, FullScreen);
		URHO3D_PARAM(P_BOREDRLESS, Borderless);
		URHO3D_PARAM(P_RESIZABLE, Resizable);
		URHO3D_PARAM(P_HIGHDPI, HighDPI);
		URHO3D_PARAM(P_MONITOR, Monitor);
		URHO3D_PARAM(P_REFRESHRATE, RefreshRate);
	}

	URHO3D_EVENT(E_RENDERSURFACEUPDATE, RendersurfaceUpdat)
	{

	}

	URHO3D_EVENT(E_BEGINRENDERING, BeginRendering)
	{

	}

	URHO3D_EVENT(E_ENDRENDERING, EndRendering)
	{
	}

	URHO3D_EVENT(E_DEVICELOST, DeviceLost)
	{
	}

	URHO3D_EVENT(E_DEVICERESET, DeviceReset)
	{
	}

}

#endif //URHO3DCOPY_GRAPHICSEVENTS_H
