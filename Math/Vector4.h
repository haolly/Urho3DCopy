//
// Created by liuhao1 on 2018/1/3.
//

#ifndef URHO3DCOPY_VECTOR4_H
#define URHO3DCOPY_VECTOR4_H

#include "Vector3.h"

namespace Urho3D
{
	class Vector4
	{
	public:
		Vector4() :
			x_(0.0f),
			y_(0.0f),
			z_(0.0f),
			w_(0.0f)
		{
		}

		Vector4(const Vector4& vector) :
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_),
			w_(vector.w_)
		{
		}

		Vector4(const Vector3& vector, float w) :
			x_(vector.x_),
			y_(vector.y_),
			z_(vector.z_),
			w_(w)
		{
		}

		Vector4(float x, float y, float z, float w) :
			x_(x),
			y_(y),
			z_(z),
			w_(w)
		{
		}

		explicit Vector4(const float* data) :
			x_(data[0]),
			y_(data[1]),
			z_(data[2]),
			w_(data[3])
		{
		}

		Vector4&operator =(const Vector4& rhs)
		{
			x_ = rhs.x_;
			y_ = rhs.y_;
			z_ = rhs.z_;
			w_ = rhs.w_;
			return *this;
		}

		bool operator ==(const Vector4& rhs) const
		{
			return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_ && w_ == rhs.w_;
		}

		bool operator !=(const Vector4& rhs) const
		{
			return !(this == rhs);
		}

		Vector4 operator+(const Vector4& rhs) const
		{
			return Vector4(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_, w_ + rhs.w_);
		}

		Vector4 operator-() const
		{
			return Vector4(-x_, -y_, -z_, -w_);
		}

		Vector4 operator-(const Vector4& rhs) const
		{
			return Vector4(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_, w_ - rhs.w_);
		}

		Vector4 operator *(float rhs) const
		{
			return Vector4(x_ * rhs, y_ * rhs, z_ * rhs , w_ * rhs);
		}

		Vector4 operator *(const Vector4& rhs) const
		{
			return Vector4(x_ * rhs.x_, y_ * rhs.y_, z_ * rhs.z_, w_ * rhs.w_);
		}

		Vector4 operator /(float rhs) const
		{
			return Vector4(x_ / rhs , y_/rhs, z_/rhs, w_/rhs);
		}

		Vector4 operator /(const Vector4& rhs) const
		{
			return Vector4(x_ / rhs.x_, y_ /rhs.y_, z_/rhs.z_, w_/rhs.w_);
		}

		Vector4&operator +=(const Vector4& rhs)
		{
			x_ += rhs.x_;
			y_ += rhs.y_;
			z_ += rhs.z_;
			w_ += rhs.w_;
			return *this;
		}

		Vector4&operator -=(const Vector4& rhs)
		{
			x_ -= rhs.x_;
			y_ -= rhs.y_;
			z_ -= rhs.z_;
			w_ -= rhs.w_;
			return *this;
		}

		Vector4&operator *=(float rhs)
		{
			x_ *= rhs;
			y_ *= rhs;
			z_ *= rhs;
			w_ *= rhs;
			return *this;
		}

		Vector4&operator *=(const Vector4& rhs)
		{
			x_ *= rhs.x_;
			y_ *= rhs.y_;
			z_ *= rhs.z_;
			w_ *= rhs.w_;
			return *this;
		}

		Vector4&operator /=(float rhs)
		{
			float invRhs = 1.0f / rhs;
			x_ *= invRhs;
			y_ *= invRhs;
			z_ *= invRhs;
			w_ *= invRhs;
			return *this;
		}

		Vector4&operator /=(const Vector4& rhs)
		{
			x_ /= rhs.x_;
			y_ /= rhs.y_;
			z_ /= rhs.z_;
			w_ /= rhs.w_;
			return *this;
		}

		float operator[](unsigned index) const { return (&x_)[index]; }
		float&operator[](unsigned index) { return (&x_)[index]; }

		float DotProduct(const Vector4& rhs) const
		{
			return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_;
		}

		float AbsDotProduct(const Vector4& rhs) const
		{
			return Urho3D::Abs(x_ * rhs.x_) + Urho3D::Abs(y_ * rhs.y_) + Urho3D::Abs(z_ * rhs.z_) + Urho3D::Abs(w_ * rhs.w_);
		}

		float ProjectOntoAxis(const Vector3& axis) const
		{
			return DotProduct(Vector4(axis.Normalized(), 0.0f));
		}

		Vector4 Abs() const
		{
			return Vector4(Urho3D::Abs(x_), Urho3D::Abs(y_), Urho3D::Abs(z_), Urho3D::Abs(w_));
		}

		Vector4 Lerp(const Vector4& rhs, float t) const
		{
			return *this * (1.0 - t) + rhs * t;
		}

		bool Equals(const Vector4& rhs) const
		{
			return Urho3D::Equals(x_, rhs.x_) && Urho3D::Equals(y_, rhs.y_) && Urho3D::Equals(z_, rhs.z_) && Urho3D::Equals(w_, rhs.w_);
		}

		bool IsNaN() const
		{
			return Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_) || Urho3D::IsNaN(z_) || Urho3D::IsNaN(w_);
		}

		const float* Data() const
		{
			return &x_;
		}

		String ToString() const;

		unsigned ToHash() const
		{
			unsigned hash = 37;
			hash = 37 * hash + FloatToRawIntBits(x_);
			hash = 37 * hash + FloatToRawIntBits(y_);
			hash = 37 * hash + FloatToRawIntBits(z_);
			hash = 37 * hash + FloatToRawIntBits(w_);
			return hash;
		}

		float x_;
		float y_;
		float z_;
		float w_;

		static const Vector4 ZERO;
		static const Vector4 ONE;
	};

	inline Vector4 operator *(float lhs, const Vector4& rhs) { return rhs * lhs; }
	inline Vector4 VectorLerp(const Vector4& lhs, const Vector4& rhs, const Vector4& t) { return lhs + (rhs - lhs) * t; }
	//todo
}



#endif //URHO3DCOPY_VECTOR4_H
