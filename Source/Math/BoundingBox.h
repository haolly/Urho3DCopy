//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_BOUNDINGBOX_H
#define URHO3DCOPY_BOUNDINGBOX_H

#include "Vector3.h"
#include "Matrix3x4.h"
#include "Sphere.h"

namespace Urho3D
{
	// Three-dimensional AABB
	class BoundingBox
	{
	public:
		BoundingBox() :
			min_(M_INFINITY, M_INFINITY, M_INFINITY),
			max_(-M_INFINITY, -M_INFINITY, -M_INFINITY),
		{
		}

		BoundingBox(const BoundingBox& box) :
			min_(box.min_),
			max_(box.max_)
		{
		}

		BoundingBox(const Vector3& min, const Vector3& max) :
			min_(min),
			max_(max)
		{
		}

		BoundingBox(float min, float max) :
			min_(Vector3(min, min, min)),
			max_(Vector3(max, max, max))
		{
		}

		BoundingBox(const Vector3* vertices, unsigned count) :
				min_(M_INFINITY, M_INFINITY, M_INFINITY),
				max_(-M_INFINITY, -M_INFINITY, -M_INFINITY)
		{
			Define(vertices, count);
		}

		BoundingBox&operator =(const BoundingBox& rhs)
		{
			min_ = rhs.min_;
			max_ = rhs.max_;
			return *this;
		}

		bool operator ==(const BoundingBox& rhs) const
		{
			return min_ == rhs.min_ && max_ == rhs.max_;
		}

		bool operator !=(const BoundingBox& rhs) const
		{
			return min_ != rhs.min_ || max_ != rhs.max_;
		}

		void Define(const BoundingBox& box)
		{
			Define(box.min_, box.max_);
		}

		void Define(const Vector3& min, const Vector3& max)
		{
			min_ = min;
			max_ = max;
		}

		void Define(float min, float max)
		{
			min_ = Vector3(min, min, min);
			max_ = Vector3(max, max, max);
		}

		void Define(Vector3& point)
		{
			min_ = max_ = point;
		}

		void Merge(const Vector3& point)
		{
#ifdef URHO3D_SSE
			//todo
#else
			if(point.x_ < min_.x_)
				min_.x_ = point.x_;
			if(point.y_ < min_.y_)
				min_.y_ = point.y_;
			if(point.z_ < min_.z_)
				min_.z_ = point.z_;

			if(point.x_ > max_.x_)
				max_.x_ = point.x_;
			if(point.y_ > max_.y_)
				max_.y_ = max_.y_;
			if(point.z_ > max_.z_)
				max_.z_ = point.z_;
#endif
		}

		void Merge(const BoundingBox& box)
		{
			if(box.min_.x_ < min_.x_)
				min_.x_ = box.min_.x_;
			if(box.min_.y_ < min_.y_)
				min_.y_ = box.min_.y_;
			if(box.min_.z_ < min_.z_)
				min_.z_ = box.min_.z_;

			if(box.max_.x_ > max_.x_)
				max_.x_ = box.max_.x_;
			if(box.max_.y_ > max_.y_)
				max_.y_ = box.max_.y_;
			if(box.max_.z_ > max_.z_)
				max_.z_ = box.max_.z_;
		}




		void Define(const Vector3* vertices, unsigned count);
		//todo
		void Merge(const Vector3* vertices, unsigned count);
		void Merge(const Sphere& sphere);
		//todo

		void Clip(const BoundingBox& box);
		void Transform(const Matrix3& transform);
		void Transform(const Matrix3x4& transform);

		void Clear()
		{
			min_ = Vector3(M_INFINITY, M_INFINITY, M_INFINITY);
			max_ = Vector3(-M_INFINITY, -M_INFINITY, -M_INFINITY);
		}

		bool Defined() const
		{
			return min_.x_ != M_INFINITY;
		}

		Vector3 Center() const
		{
			return (max_ + min_) * 0.5f;
		}

		Vector3 Size() const
		{
			return max_ - min_;
		}

		Vector3 HalfSize() const
		{
			return (max_ - min_) * 0.5f;
		}

		BoundingBox Transformed(const Matrix3& transform) const;
		BoundingBox Transformed(const Matrix3x4& transform) const;
		float DistanceToPoint(const Vector3& point) const;

		Intersection IsInside(const Vector3& point) const
		{
			if(point.x_ < min_.x_ || point.x_ > max_.x_ ||
				point.y_ < min_.y_ || point.y_ > max_.y_ ||
				point.z_ < min_.z_ || point.z_ > max_.z_)
			{
				return OUTSIDE;
			}
			return INSIDE;
		}

		// Is box inside this box
		Intersection IsInside(const BoundingBox& box) const
		{
			if(box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ ||
				box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_||
				box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
			{
				return OUTSIDE;
			}
			else if(box.max_.x_ > max_.x_ || box.min_.x_ < min_.x_ ||
					box.max_.y_ > max_.y_ || box.min_.y_ < min_.y_ ||
					box.max_.z_ > max_.z_ || box.min_.z_ < min_.z_)
			{
				return INTERSECTS;
			}
			return INSIDE;
		}

		//Test if another bounding is partially inside or outside
		Intersection IsInsideFast(const BoundingBox& box) const
		{
			if(box.max_.x_ < min_.x_ || box.min_.x_ > max_.x_ ||
				box.max_.y_ < min_.y_ || box.min_.y_ > max_.y_||
				box.max_.z_ < min_.z_ || box.min_.z_ > max_.z_)
			{
				return OUTSIDE;
			}
			return INSIDE;
		}

		Intersection IsInside(const Sphere& sphere) const;
		Intersection IsInsideFast(const Sphere& sphere) const;

		String ToString() const;

		Vector3 min_;
		float dummyMin_;

		Vector3 max_;
		float dummyMax_;
	};
}



#endif //URHO3DCOPY_BOUNDINGBOX_H
