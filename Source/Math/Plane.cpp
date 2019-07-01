//
// Created by liuhao1 on 2017/12/22.
//

#include "Plane.h"

namespace Urho3D
{
	const Plane Plane::UP(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

	void Plane::Transform(const Matrix3 &transform)
	{

	}

	void Plane::Transform(const Matrix3x4 &transform)
	{

	}

	Matrix3x4 Plane::ReflectMatrix() const
	{
		return Matrix3x4();
	}

	Plane Plane::Transformed(const Matrix3 &transform) const
	{
		return Plane();
	}

	Plane Plane::Transformed(const Matrix3x4 &transform) const
	{
		return Plane();
	}
}