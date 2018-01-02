//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_SPHERE_H
#define URHO3DCOPY_SPHERE_H

#include "Vector3.h"
#include "BoundingBox.h"

namespace Urho3D
{
	class Sphere
	{
	public:
		Sphere() :
			center_(Vector3::ZERO),
			radius_(-M_INFINITY)
		{
		}

		Sphere(const Sphere& sphere) :
			center_(sphere.center_),
			radius_(sphere.radius_)
		{
		}

		Sphere(const Vector3& center, float radisu) :
			center_(center),
			radius_(radius_)
		{
		}

		Sphere(const Vector3* vertices, unsigned count)
		{
			Define(vertices, count);
		}

		Sphere(const BoundingBox& box)
		{
			Define(box);
		}

		bool operator==(const Sphere& rhs) const
		{
			return center_ == rhs.center_ && radius_ == rhs.radius_;
		}

		bool operator !=(const Sphere& rhs) const
		{
			return center_ != rhs.center_ || radius_ !=rhs.radius_;
		}

		void Define(const Sphere& sphere)
		{
			Define(sphere.center_, sphere.radius_);
		}

		void Define(const Vector3& center, float radius)
		{
			center_ = center;
			radius_ = radius;
		}

		void Define(const Vector3* vertices, unsigned count);

		void Define(const const BoundingBox& box);

		void Merge(const Vector3& point)
		{
			if(radius_ < 0.0f)
			{
				center_ = point;
				radius_ = 0.0f;
				return;
			}
			Vector3 offset = point - center_;
			float dist = offset.Length();

			if(dist > radius_)
			{
				float half = (dist - radius_) * 0.5f;
				radius_ += half;
				center_ += (offset.Normalized() * half);
			}
		}

		void Merge(const Vector3& vertices, unsigned count);
		void Merge(const BoundingBox& box);
		void Merge(const Sphere& sphere);

		void Clear()
		{
			center_ = Vector3::ZERO;
			radius_ = -M_INFINITY;
		}

		bool Defined() const
		{
			return radius_ >= 0.0f;
		}

		Intersection IsInside(const Vector3& point) const
		{
			float distSquared = (point - center_).LengthSquared();
			if(distSquared < radius_ * radius_)
			{
				return INSIDE;
			}
			return OUTSIDE;
		}

		Intersection IsInside(const Sphere& sphere) const
		{

		}



		Vector3 center_;
		float radius_;
	};
}



#endif //URHO3DCOPY_SPHERE_H
