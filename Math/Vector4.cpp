//
// Created by liuhao1 on 2018/1/3.
//

#include <cstdio>
#include "Vector4.h"

namespace Urho3D
{
	const Vector4 Vector4::ZERO;
	const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);

	String Vector4::ToString() const
	{
		char temp[CONVERSION_BUFFER_LENGTH];
		sprintf(temp, "%g %g %g %g", x_, y_, z_, w_);
		return String(temp);
	}
}
