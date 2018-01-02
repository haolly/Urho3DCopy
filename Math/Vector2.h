//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_VECTOR2_H
#define URHO3DCOPY_VECTOR2_H

#include <math.h>
#include "../Container/Str.h"
#include "MathDefs.h"

namespace Urho3D
{

	class IntVector2
	{
	public:
		/// Construct a zero vector.
		IntVector2() :
				x_(0),
				y_(0)
		{
		}

		/// Construct from coordinates.
		IntVector2(int x, int y) :
				x_(x),
				y_(y)
		{
		}

		/// Construct from an int array.
		explicit IntVector2(const int* data) :
				x_(data[0]),
				y_(data[1])
		{
		}

		/// Construct from an float array.
		explicit IntVector2(const float* data) :
				x_((int)data[0]),
				y_((int)data[1])
		{
		}
		/// Copy-construct from another vector.
		IntVector2(const IntVector2& rhs) :
				x_(rhs.x_),
				y_(rhs.y_)
		{
		}

		/// Assign from another vector.
		IntVector2& operator =(const IntVector2& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			return *this;
		}

		/// Test for equality with another vector.
		bool operator ==(const IntVector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

		/// Test for inequality with another vector.
		bool operator !=(const IntVector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

		/// Add a vector.
		IntVector2 operator +(const IntVector2& rhs) const { return IntVector2(x_ + rhs.x_, y_ + rhs.y_); }

		/// Return negation.
		IntVector2 operator -() const { return IntVector2(-x_, -y_); }

		/// Subtract a vector.
		IntVector2 operator -(const IntVector2& rhs) const { return IntVector2(x_ - rhs.x_, y_ - rhs.y_); }

		/// Multiply with a scalar.
		IntVector2 operator *(int rhs) const { return IntVector2(x_ * rhs, y_ * rhs); }

		/// Multiply with a vector.
		IntVector2 operator *(const IntVector2& rhs) const { return IntVector2(x_ * rhs.x_, y_ * rhs.y_); }

		/// Divide by a scalar.
		IntVector2 operator /(int rhs) const { return IntVector2(x_ / rhs, y_ / rhs); }

		/// Divide by a vector.
		IntVector2 operator /(const IntVector2& rhs) const { return IntVector2(x_ / rhs.x_, y_ / rhs.y_); }

		/// Add-assign a vector.
		IntVector2& operator +=(const IntVector2& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			return *this;
		}

		/// Subtract-assign a vector.
		IntVector2& operator -=(const IntVector2& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			return *this;
		}

		/// Multiply-assign a scalar.
		IntVector2& operator *=(int rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			return *this;
		}

		/// Multiply-assign a vector.
		IntVector2& operator *=(const IntVector2& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			return *this;
		}

		/// Divide-assign a scalar.
		IntVector2& operator /=(int rhs)
		{
			x_ /= rhs;
			y_ /= rhs;
			return *this;
		}

		/// Divide-assign a vector.
		IntVector2& operator /=(const IntVector2& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			return *this;
		}

		/// Return integer data.
		const int* Data() const { return &x_; }

		/// Return as string.
		String ToString() const;

		/// Return hash value for HashSet & HashMap.
		unsigned ToHash() const { return (unsigned)x_ * 31 + (unsigned)y_; }

		/// Return length.
		float Length() const { return sqrtf((float)(x_ * x_ + y_ * y_)); }

		/// X coordinate.
		int x_;
		/// Y coordinate.
		int y_;

		/// Zero vector.
		static const IntVector2 ZERO;
		/// (-1,0) vector.
		static const IntVector2 LEFT;
		/// (1,0) vector.
		static const IntVector2 RIGHT;
		/// (0,1) vector.
		static const IntVector2 UP;
		/// (0,-1) vector.
		static const IntVector2 DOWN;
		/// (1,1) vector.
		static const IntVector2 ONE;
	};

	class  Vector2
	{
	public:
		/// Construct a zero vector.
		Vector2() :
				x_(0.0f),
				y_(0.0f)
		{
		}

		/// Copy-construct from another vector.
		Vector2(const Vector2& vector) :
				x_(vector.x_),
				y_(vector.y_)
		{
		}

		/// Construct from an IntVector2.
		explicit Vector2(const IntVector2& vector) :
				x_((float)vector.x_),
				y_((float)vector.y_)
		{
		}

		/// Construct from coordinates.
		Vector2(float x, float y) :
				x_(x),
				y_(y)
		{
		}

		/// Construct from a float array.
		explicit Vector2(const float* data) :
				x_(data[0]),
				y_(data[1])
		{
		}

		/// Assign from another vector.
		Vector2& operator =(const Vector2& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			return *this;
		}

		/// Test for equality with another vector without epsilon.
		bool operator ==(const Vector2& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_; }

		/// Test for inequality with another vector without epsilon.
		bool operator !=(const Vector2& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_; }

		/// Add a vector.
		Vector2 operator +(const Vector2& rhs) const { return Vector2(x_ + rhs.x_, y_ + rhs.y_); }

		/// Return negation.
		Vector2 operator -() const { return Vector2(-x_, -y_); }

		/// Subtract a vector.
		Vector2 operator -(const Vector2& rhs) const { return Vector2(x_ - rhs.x_, y_ - rhs.y_); }

		/// Multiply with a scalar.
		Vector2 operator *(float rhs) const { return Vector2(x_ * rhs, y_ * rhs); }

		/// Multiply with a vector.
		Vector2 operator *(const Vector2& rhs) const { return Vector2(x_ * rhs.x_, y_ * rhs.y_); }

		/// Divide by a scalar.
		Vector2 operator /(float rhs) const { return Vector2(x_ / rhs, y_ / rhs); }

		/// Divide by a vector.
		Vector2 operator /(const Vector2& rhs) const { return Vector2(x_ / rhs.x_, y_ / rhs.y_); }

		/// Add-assign a vector.
		Vector2& operator +=(const Vector2& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			return *this;
		}

		/// Subtract-assign a vector.
		Vector2& operator -=(const Vector2& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			return *this;
		}

		/// Multiply-assign a scalar.
		Vector2& operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			return *this;
		}

		/// Multiply-assign a vector.
		Vector2& operator *=(const Vector2& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			return *this;
		}

		/// Divide-assign a scalar.
		Vector2& operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			return *this;
		}

		/// Divide-assign a vector.
		Vector2& operator /=(const Vector2& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			return *this;
		}

		/// Normalize to unit length.
		void Normalize()
		{
			float lenSquared = LengthSquared();
			if (!Urho3D::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				x_ *= invLen;
				y_ *= invLen;
			}
		}

		/// Return length.
		float Length() const { return sqrtf(x_ * x_ + y_ * y_); }

		/// Return squared length.
		float LengthSquared() const { return x_ * x_ + y_ * y_; }

		/// Calculate dot product.
		float DotProduct(const Vector2& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_; }

		/// Calculate absolute dot product.
		float AbsDotProduct(const Vector2& rhs) const { return Urho3D::Abs(x_ * rhs.x_) + Urho3D::Abs(y_ * rhs.y_); }

		/// Project vector onto axis.
		float ProjectOntoAxis(const Vector2& axis) const { return DotProduct(axis.Normalized()); }

		/// Returns the angle between this vector and another vector in degrees.
		float Angle(const Vector2& rhs) const { return Urho3D::Acos(DotProduct(rhs) / (Length() * rhs.Length())); }

		/// Return absolute vector.
		Vector2 Abs() const { return Vector2(Urho3D::Abs(x_), Urho3D::Abs(y_)); }

		/// Linear interpolation with another vector.
		Vector2 Lerp(const Vector2& rhs, float t) const { return *this * (1.0f - t) + rhs * t; }

		/// Test for equality with another vector with epsilon.
		bool Equals(const Vector2& rhs) const { return Urho3D::Equals(x_, rhs.x_) && Urho3D::Equals(y_, rhs.y_); }

		/// Return whether is NaN.
		bool IsNaN() const { return Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_); }

		/// Return normalized to unit length.
		Vector2 Normalized() const
		{
			float lenSquared = LengthSquared();
			if (!Urho3D::Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				return *this * invLen;
			}
			else
				return *this;
		}

		/// Return float data.
		const float* Data() const { return &x_; }

		/// Return as string.
		String ToString() const;

		/// X coordinate.
		float x_;
		/// Y coordinate.
		float y_;

		/// Zero vector.
		static const Vector2 ZERO;
		/// (-1,0) vector.
		static const Vector2 LEFT;
		/// (1,0) vector.
		static const Vector2 RIGHT;
		/// (0,1) vector.
		static const Vector2 UP;
		/// (0,-1) vector.
		static const Vector2 DOWN;
		/// (1,1) vector.
		static const Vector2 ONE;

	};
}



#endif //URHO3DCOPY_VECTOR2_H
