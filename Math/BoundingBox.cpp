//
// Created by liuhao1 on 2017/12/22.
//

#include "BoundingBox.h"

namespace Urho3D
{

	void BoundingBox::Define(const Vector3 *vertices, unsigned count)
	{
		Clear();
		if(!count)
			return;
		Merge(vertices, count);
	}

	void BoundingBox::Merge(const Vector3* vertices, unsigned count)
	{
		while(count--)
			Merge(*vertices++);
	}

	void BoundingBox::Merge(const Sphere &sphere)
	{
		const Vector3& center = sphere.center_;
		float radius = sphere.radius_;
		Merge(center + Vector3(radius, radius, radius));
		Merge(center + Vector3(-radius, -radius, -radius));
	}

	void BoundingBox::Clip(const BoundingBox &box)
	{
		if(box.min_.x_ > min_.x_)
			min_.x_ = box.min_.x_;
		if(box.max_.x_ < max_.x_)
			max_.x_ = box.max_.x_;
		if(box.min_.y_ > min_.y_)
			min_.y_ = box.min_.y_;
		if(box.max_.y_ < max_.y_)
			max_.y_ = box.max_.y_;
		if(box.min_.z_ > min_.z_)
			min_.z_ = box.min_.z_;
		if(box.max_.z_ < max_.z_)
			max_.z_ = box.max_.z_;

		if(min_.x_ > max_.x_ || min_.y_ > max_.y_ || min_.z_ > max_.z_)
		{
			min_ = Vector3(M_INFINITY, M_INFINITY, M_INFINITY);
			max_ = Vector3(-M_INFINITY, -M_INFINITY, -M_INFINITY);
		}
	}

	void BoundingBox::Transform(const Matrix3 &transform)
	{
		*this = Transformed(transform);
	}

	void BoundingBox::Transform(const Matrix3x4 &transform)
	{
		*this = Transformed(transform);
	}

	BoundingBox BoundingBox::Transformed(const Matrix3 &transform) const
	{
		return Transformed(Matrix3x4(transform));
	}

	BoundingBox BoundingBox::Transformed(const Matrix3x4 &transform) const
	{
		Vector3 newCenter = transform * Center();
		Vector3 oldEdge = Size() * 0.5f;
		Vector3 newEdge = Vector3(
				Abs(transform.m00_) * oldEdge.x_ + Abs(transform.m01_) * oldEdge.y_ + Abs(transform.m02_) * oldEdge.z_,
				Abs(transform.m10_) * oldEdge.x_ + Abs(transform.m11_) * oldEdge.y_ + Abs(transform.m12_) * oldEdge.z_,
				Abs(transform.m20_) * oldEdge.x_ + Abs(transform.m21_) * oldEdge.y_ + Abs(transform.m22_) * oldEdge.z_
		);
		return BoundingBox(newCenter - newEdge, newCenter + newEdge);
	}

	float BoundingBox::DistanceToPoint(const Vector3 &point) const
	{
		const Vector3 offset = Center() - point;
		const Vector3 absOffset(Abs(offset.x_), Abs(offset.y_), Abs(offset.z_));
		return VectorMax(Vector3::ZERO, absOffset - HalfSize()).Length();
	}

	Intersection BoundingBox::IsInside(const Sphere &sphere) const
	{
		//The closest point on AABB to the center fo sphere to determine whether box is out of sphere
		float distSquare = 0;
		float temp;
		const Vector3& center = sphere.center_;
		if(center.x_ < min_.x_)
		{
			temp = center.x_ - min_.x_;
			distSquare += temp * temp;
		}
		else if(center.x_ > max_.x_)
		{
			temp = center.x_ - max_.x_;
			distSquare += temp * temp;
		}
		if(center.y_ < min_.y_)
		{
			temp = center.y_ - min_.y_;
			distSquare += temp * temp;
		}
		else if(center.y_ > max_.y_)
		{
			temp = center.y_ - max_.y_;
			distSquare += temp * temp;
		}
		if(center.z_ < min_.z_)
		{
			temp = center.z_ - min_.z_;
			distSquare += temp * temp;
		}
		else if(center.z_ > max_.z_)
		{
			temp = center.z_ - max_.z_;
			distSquare += temp * temp;
		}

		float radius = sphere.radius_;
		if(distSquare >= radius * radius)
			return OUTSIDE;
		else if(center.x_ - radius < min_.x_ || center.x_ + radius > max_.x_ || center.y_ - radius < min_.y_ ||
				center.y_ + radius > max_.y_ || center.z_ - radius < min_.z_ || center.z_ + radius > max_.z_)
			return INTERSECTS;
		else
			return INSIDE;
	}

	Intersection BoundingBox::IsInsideFast(const Sphere &sphere) const
	{
		float distSquare = 0;
		float temp;
		const Vector3& center = sphere.center_;
		if(center.x_ < min_.x_)
		{
			temp = center.x_ - min_.x_;
			distSquare += temp * temp;
		}
		else if(center.x_ > max_.x_)
		{
			temp = center.x_ - max_.x_;
			distSquare += temp * temp;
		}
		if(center.y_ < min_.y_)
		{
			temp = center.y_ - min_.y_;
			distSquare += temp * temp;
		}
		else if(center.y_ > max_.y_)
		{
			temp = center.y_ - max_.y_;
			distSquare += temp * temp;
		}
		if(center.z_ < min_.z_)
		{
			temp = center.z_ - min_.z_;
			distSquare += temp * temp;
		}
		else if(center.z_ > max_.z_)
		{
			temp = center.z_ - max_.z_;
			distSquare += temp * temp;
		}

		float radius = sphere.radius_;
		if(distSquare >= radius * radius)
			return OUTSIDE;
		else
			return INSIDE;
	}

	String BoundingBox::ToString() const
	{
		return min_.ToString() + " - " + max_.ToString();
	}

}