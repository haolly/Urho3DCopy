//
// Created by liuhao1 on 2018/1/4.
//

#ifndef URHO3DCOPY_GRAPHICS_H
#define URHO3DCOPY_GRAPHICS_H

#include "../Core/Object.h"

namespace Urho3D
{
	class Graphics : public Object
	{
		URHO3D_OBJECT(Graphics, Object);
	public:
		Graphics(Context* context);
		virtual ~Graphics() override ;
		//todo
	};

}



#endif //URHO3DCOPY_GRAPHICS_H
