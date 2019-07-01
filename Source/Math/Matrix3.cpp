//
// Created by liuhao1 on 2017/12/23.
//

#include <cstdio>
#include "Matrix3.h"

namespace Urho3D
{
	const Matrix3 Matrix3::ZERO(
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	);

	const Matrix3 Matrix3::IDENTITY;

	Matrix3 Matrix3::Inverse() const
	{
		// ref http://www.purplemath.com/modules/determs2.htm
		float det = m00_ * m11_ * m22_ +
		            m01_ * m12_ * m20_ +
		            m02_ * m10_ * m21_ -
		            m02_ * m11_ * m20_ -
		            m00_ * m12_ * m21_ -
		            m01_ * m10_ * m22_;
		//todo

	}

	String Matrix3::ToString() const
	{
		char tempBuffer[MATRIX_CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer, "%g %g %g %g %g %g %g %g %g", m00_, m01_, m02_, m10_, m11_, m12_, m20_, m21_, m22_);
		return String(tempBuffer);
	}
}