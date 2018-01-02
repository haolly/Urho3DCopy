//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_VECTOR3_H
#define URHO3DCOPY_VECTOR3_H

#include <math.h>
#include "../Container/Str.h"
#include "Vector2.h"
#include "MathDefs.h"

namespace Urho3D
{
	class IntVector3
	{
	public:
		IntVector3() :
			x_(0),
			y_(0),
			z_(0)
		{
		}

		IntVector3(int x, int y, int z) :
			x_(x),
			y_(y),
			z_(z)
		{
		}

		IntVector3(const int* data) :
			x_(data[0]),
			y_(data[1]),
			y_(data[2])
		{
		}

		IntVector3(const IntVector3& rhs) :
			x_(rhs.x_),
			y_(rhs.y_),
			z_(rhs.z_)
		{
		}

		IntVector3&operator =(const IntVector3& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
			return *this;
		}

		bool operator ==(const IntVector3& rhs) const
		{
			return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
		}

		bool operator !=(const IntVector3& rhs) const
		{
			return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_;
		}

		IntVector3 operator +(const IntVector3& rhs) const
		{
			return IntVector3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
		}

		IntVector3 operator -(const IntVector3& rhs) const
		{
			return IntVector3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
		}

		IntVector3 operator -() const
		{
			return IntVector3(-x_, -y_, -z_);
		}

		IntVector3 operator *(int rhs) const
		{
			return IntVector3(x_ * rhs, y_ * rhs, z_ * rhs);
		}

		IntVector3 operator *(const IntVector3& rhs) const
		{
			return IntVector3(x_ * rhs.x_, y_ * rhs.y_, z_* rhs.z_);
		}

		IntVector3 operator /(int rhs) const
		{
			return IntVector3(x_ / rhs, y_ / rhs, z_ / rhs);
		}

		IntVector3 operator /(const IntVector3& rhs) const
		{
			return IntVector3(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_);
		}

		IntVector3&operator +=(const IntVector3& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			return *this;
		}

		IntVector3&operator -=(const IntVector3& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			return *this;
		}

		IntVector3&operator *=(int rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			return *this;
		}

		IntVector3&operator *=(const IntVector3& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			return *this;
		}

		IntVector3&operator /=(int rhs)
		{
			x_ /= rhs;
			y_ /= rhs;
			z_ /= rhs;
			return *this;
		}

		IntVector3&operator /=(const IntVector3& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
			return *this;
		}

		// Todo, standard memory layout ??
		const int* Data() const { return &x_; }

		String ToString() const;

		unsigned ToHash() const
		{
			return (unsigned)x_ * 31 * 31 + (unsigned)y_ * 31 + (unsigned)z_;
		}

		float Length() const
		{
			return sqrtf((float)(x_ * x_ + y_ * y_ + z_ * z_));
		}

		int x_;
		int y_;
		int z_;

		static const IntVector3 ZERO;
		static const IntVector3 LEFT;
		static const IntVector3 RIGHT;
		static const IntVector3 UP;
		static const IntVector3 DOWN;
		static const IntVector3 FORWARD;
		static const IntVector3 BACK;
		static const IntVector3 ONE;
	};

	class Vector3
	{
	public:
		Vector3() :
			x_(0.0f),
			y_(0.0f),
			z_(0.0f)
		{
		}

		Vector3(float x, float y, float z) :
				x_(x),
				y_(y),
				z_(z)
		{
		}


		Vector3(const Vector3& rhs) :
			x_(rhs.x_),
			y_(rhs.y_),
			z_(rhs.z_)
		{
		}

		Vector3(const Vector2& vector2, float z) :
				x_(vector2.x_),
				y_(vector2.y_),
				z_(z)
		{
		}

		Vector3(const Vector2& vector) :
				x_(vector.x_),
				y_(vector.y_),
				z_(0.0f)
		{
		}

		explicit Vector3(const IntVector3& vector) :
				x_((float)vector.x_),
				y_((float)vector.y_),
				z_((float)vector.z_)
		{
		}

		Vector3(float x, float y) :
				x_(x),
				y_(y),
				z_(0.0f)
		{
		}

		explicit Vector3(const float* data) :
				x_(data[0]),
				y_(data[1]),
				z_(data[2])
		{
		}

		Vector3&operator =(const Vector3& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
			return *this;
		}

		bool operator ==(const Vector3& rhs) const
		{
			return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
		}

		bool operator !=(const Vector3& rhs) const
		{
			return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_;
		}

		Vector3 operator +(const Vector3& rhs) const
		{
			return Vector3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
		}

		Vector3 operator -(const Vector3& rhs) const
		{
			return Vector3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
		}

		Vector3 operator -() const
		{
			return Vector3(-x_, -y_, -z_);
		}

		Vector3 operator *(int rhs) const
		{
			return Vector3(x_ * rhs, y_ * rhs, z_ * rhs);
		}

		Vector3 operator *(const Vector3& rhs) const
		{
			return Vector3(x_ * rhs.x_, y_ * rhs.y_, z_* rhs.z_);
		}

		Vector3 operator /(int rhs) const
		{
			return Vector3(x_ / rhs, y_ / rhs, z_ / rhs);
		}

		Vector3 operator /(const Vector3& rhs) const
		{
			return Vector3(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_);
		}

		Vector3&operator +=(const Vector3& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			return *this;
		}

		Vector3&operator -=(const Vector3& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			return *this;
		}

		Vector3&operator *=(int rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			return *this;
		}

		Vector3&operator *=(const Vector3& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			return *this;
		}

		Vector3&operator /=(int rhs)
		{
			x_ /= rhs;
			y_ /= rhs;
			z_ /= rhs;
			return *this;
		}

		Vector3&operator /=(const Vector3& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
			return *this;
		}

		void Normalize()
		{
			float lenSquared = LengthSquared();
			if(!Urho3D::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				x_ *= invLen;
				y_ *= invLen;
				z_ *= invLen;
			}
		}

		float Length() const
		{
			return sqrtf(x_*x_ + y_* y_ + z_* z_);
		}

		float LengthSquared() const
		{
			return x_* x_ + y_* y_ + z_ * z_;
		}

		float DotProduct(const Vector3& rhs) const
		{
			return x_ * rhs.x_ + y_* rhs.y_ + z_* rhs.z_;
		}

		float AbsDotProduct(const Vector3& rhs) const
		{
			return Urho3D::Abs(x_ * rhs.x_) + Urho3D::Abs(y_* rhs.y_) + Urho3D::Abs(z_ * rhs.z_);
		}

		float ProjectOntoAxis(const Vector3& axis) const
		{
			return DotProduct(axis.Normalized());
		}

		Vector3 Orthogonalize(const Vector3& axis) const
		{
			return axis.CrossProduct(*this).CrossProduct(axis).Normalized();
		}

		Vector3 CrossProduct(const Vector3& rhs) const
		{
			return Vector3(
					y_ * rhs.z_ - z_ * rhs.y_,
					z_ * rhs.x_ - x_ * rhs.z_,
					x_ * rhs.y_ - y_ * rhs.x_
			);
		}

		Vector3 Abs() const
		{
			return Vector3(Urho3D::Abs(x_), Urho3D::Abs(y_), Urho3D::Abs(z_));
		}

		Vector3 Lerp(const Vector3& rhs, float t) const
		{
			return *this * (1.0 - t) + rhs * t;
		}

		bool Equals(const Vector3& rhs) const
		{
			return Urho3D::Equals(x_, rhs.x_) && Urho3D::Equals(y_, rhs.y_) && Urho3D::Equals(z_, rhs.z_);
		}

		float Angle(const Vector3& rhs) const
		{
			return Urho3D::Acos(DotProduct(rhs) / (Length() * rhs.Length()));
		}

		bool IsNaN() const
		{
			return Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_) || Urho3D::IsNaN(z_);
		}

		Vector3 Normalized() const
		{
			float lenSquared = LengthSquared();
			if(!Urho3D::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				return *this * invLen;
			}
			else
				return *this;
		}

		const float* Data() const
		{
			return &x_;
		}

		String ToString() const ;

		unsigned ToHash() const
		{
			unsigned hash = 37;
			hash = 37 * hash + FloatToRawIntBits(x_);
			hash = 37 * hash + FloatToRawIntBits(y_);
			hash = 37 * hash + FloatToRawIntBits(z_);

			return hash;
		}

		float x_;
		float y_;
		float z_;

		static const Vector3 ZERO;
		static const Vector3 LEFT;
		static const Vector3 RIGHT;
		static const Vector3 UP;
		static const Vector3 DOWN;
		static const Vector3 FORWARD;
		static const Vector3 BACK;
		static const Vector3 ONE;
	};

	inline Vector3 operator *(float lhs, const Vector3& rhs)
	{
		return lhs * rhs;
	}

	inline Vector3 VectorMax(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(Max(lhs.x_, rhs.x_), Max(lhs.y_, rhs.y_), Max(lhs.z_, rhs.z_));
	}
}



#endif //URHO3DCOPY_VECTOR3_H
