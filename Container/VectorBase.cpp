//
// Created by liuhao on 2017/11/28.
//

#include "VectorBase.h"

namespace Urho3D
{

	byte *VectorBase::AllocateBuffer(unsigned size)
	{
		return new unsigned char[size];
	}
}
