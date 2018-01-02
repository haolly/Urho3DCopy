//
// Created by liuhao1 on 2017/12/22.
//

#include "Matrix3x4.h"

namespace Urho3D
{

	const Matrix3x4 Matrix3x4::ZERO(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);

	const Matrix3x4 Matrix3x4::IDENTITY;

	//todo, the rotation part and scale part,
	void Matrix3x4::Decompose(Vector3 &translation, Quaternion &rotation, Vector3 &scale) const
	{
		translation.x_ = m03_;
		translation.y_ = m13_;
		translation.z_ = m23_;

		scale.x_ = sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_);
		scale.y_ = sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_);
		scale.z_ = sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_);

		Vector3 invScale(1.0f/ scale.x_, 1.0f/ scale.y_, 1.0f / scale.z_);
		rotation = Quaternion(ToMatrix3().Scaled(invScale));

	}

	Matrix3x4 Matrix3x4::Inverse() const
	{
		//todo
	}
}

