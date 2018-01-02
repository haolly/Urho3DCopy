//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_PLANE_H
#define URHO3DCOPY_PLANE_H

#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix3x4.h"

namespace Urho3D
{
	class Plane
	{
	public:
		Plane():
			d_(0.0f)
		{
		}

		Plane(const Plane& plane) :
			normal_(plane.normal_),
			absNormal_(plane.absNormal_),
			d_(plane.d_)
		{
		}

		// Construct from 3 vertices
		Plane(const Vector3& v0, const Vector3& v1, const Vector3& v2)
		{
			Define(v0, v1, v2);
		}

		Plane(const Vector3& normal, const Vector3& point)
		{
			Define(normal, point);
		}

		Plane&operator=(const Plane& rhs)
		{
			normal_ = rhs.normal_;
			absNormal_ = rhs.absNormal_;
			d_ = rhs.d_;
			return *this;
		}

		void Define(const Vector3& v0, const Vector3& v1, const Vector3 v2)
		{
			Vector3 dist1 = v1 - v0;
			Vector3 dist2 = v2 - v0;
			Define(dist1.CrossProduct(dist2), v0);
		}

		void Define(const Vector3& normal, const Vector3& point)
		{
			normal_ = normal.Normalized();
			absNormal_ = normal_.Abs();
			//Note, d 是平面上的点在平面法线上的投影
			d_ = -normal_.DotProduct(point);
		}

		void Transform(const Matrix3& transform);
		void Transform(const Matrix3x4& transform);

		Vector3 Project(const Vector3& point) const
		{
			auto vect = point;
			return vect - normal_ * (normal_.DotProduct(point) + d_);
		}


		float Distance(const Vector3& point) const
		{
			return normal_.DotProduct(point) + d_;
		}

		Vector3 Reflect(const Vector3& direction) const
		{
			return direction - (2.0f *normal_.DotProduct(direction) * normal_);
		}

		Matrix3x4 ReflectMatrix() const;
		Plane Transformed(const Matrix3& transform) const;
		Plane Transformed(const Matrix3x4& transform) const;


		Vector3 normal_;
		Vector3 absNormal_;
		//Note, d 是平面上的点在平面法线上的投影
		// ref https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
		// ref https://brilliant.org/wiki/3d-coordinate-geometry-equation-of-a-plane/
		float d_;

		static const Plane UP;
	};
}



#endif //URHO3DCOPY_PLANE_H
