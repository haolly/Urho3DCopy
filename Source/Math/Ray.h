//
// Created by liuhao1 on 2017/12/27.
//

#ifndef URHO3DCOPY_RAY_H
#define URHO3DCOPY_RAY_H

#include "Vector3.h"
#include "BoundingBox.h"
#include "Plane.h"
#include "Sphere.h"

namespace Urho3D
{
	class Ray
	{
	public:
		Ray()
		{

		}

		Ray(const Vector3& origin, const Vector3& direction)
		{
			Define(origin, direction);
		}

		Ray(const Ray& ray) :
				origin_(ray.origin_),
				direction_(ray.direction_)
		{
		}

		Ray&operator =(const Ray& rhs)
		{
			origin_ = rhs.origin_;
			direction_ = rhs.direction_;
			return *this;
		}

		bool operator==(const Ray& rhs) const
		{
			return origin_ == rhs.origin_ && direction_ == rhs.direction_;
		}

		bool operator!=(const Ray& rhs) const
		{
			return origin_ != rhs.origin_ || direction_ != rhs.direction_;
		}

		void Define(const Vector3& origin, const Vector3& direction)
		{
			origin_ = origin;
			direction_ = direction_;
		}

		Vector3 Project(const Vector3& point) const
		{
			Vector3 offset = point - origin_;
			return origin_ + offset.DotProduct(direction_) * direction_;
		}

		float Distance(const Vector3& point) const
		{
			Vector3 projected = Project(point);
			return (point - projected).Length();
		}

		Vector3 ClosestPoint(const Ray& ray) const;
		float HitDistance(const Plane& plane) const;
		float HitDistance(const BoundingBox& box) const;
		float HitDistance(const Sphere& sphere) const;
		float HitDistance(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector3* outNormal = nullptr,
							Vector3* outBary = nullptr) const;
		float HitDistance(const void* vertexData, unsigned vertexStride, unsigned vertexStart, unsigned vertexCount,
						 Vector3* outNormal = nullptr, Vector2* outUV = nullptr, unsigned uvOffset = 0) const;
		float HitDistance(const void* vertexData, unsigned vertexStride, const void* indexData, unsigned indexSize,
		                  unsigned indexStart, unsigned indexCount, Vector3* outNormal = nullptr,
		                  Vector2* outUV = nullptr, unsigned uvOffset = 0) const;
		bool InsideGeometry(const void* vertexData, unsigned vertexSize, unsigned vertexStart, unsigned vertexCount)
		const;

		bool InsideGeometry(const void* vertexData, unsigned vertexSize, const void* indexData, unsigned indexSize,
		                    unsigned indexStart, unsigned indexCount) const;
		Ray Transformed(const Matrix3x4& transform) const;

		Vector3 origin_;
		Vector3 direction_;
	};
}



#endif //URHO3DCOPY_RAY_H
