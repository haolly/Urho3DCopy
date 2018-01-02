//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_MATRIX3X4_H
#define URHO3DCOPY_MATRIX3X4_H

#include "Vector3.h"
#include "Matrix3.h"
#include "Quaternion.h"

namespace Urho3D
{
	class Matrix3x4
	{
	public:
		// Construct an identity matrix
		Matrix3x4() :
			m00_(1.0f),
			m01_(0.0f),
			m02_(0.0f),
			m03_(0.0f),
			m10_(0.0f),
			m11_(1.0f),
			m12_(0.0f),
			m13_(0.0f),
			m20_(0.0f),
			m21_(0.0f),
			m22_(1.0f),
			m23_(0.0f)
		{
		}

		Matrix3x4(const Matrix3x4& matrix) :
			m00_(matrix.m00_),
			m01_(matrix.m01_),
			m02_(matrix.m02_),
			m03_(matrix.m03_),
			m10_(matrix.m10_),
			m11_(matrix.m11_),
			m12_(matrix.m12_),
			m13_(matrix.m13_),
			m20_(matrix.m20_),
			m21_(matrix.m21_),
			m22_(matrix.m22_),
			m23_(matrix.m23_)
		{
		}

		Matrix3x4(const Matrix3& matrix) :
			m00_(matrix.m00_),
			m01_(matrix.m01_),
			m02_(matrix.m02_),
			m03_(0.0f),
			m10_(matrix.m10_),
			m11_(matrix.m11_),
			m12_(matrix.m12_),
			m13_(0.0f),
			m20_(matrix.m20_),
			m21_(matrix.m21_),
			m22_(matrix.m22_),
			m23_(0.0f)
		{
		}

		Matrix3x4(float v00, float v01, float v02, float v03,
				  float v10, float v11, float v12, float v13,
				  float v20, float v21, float v22, float v23) :
				m00_(v00),
				m01_(v01),
				m02_(v02),
				m03_(v03),
				m10_(v10),
				m11_(v11),
				m12_(v12),
				m13_(v13),
				m20_(v20),
				m21_(v21),
				m22_(v22),
				m23_(v23)
		{
		}

		explicit Matrix3x4(const float* data) :
				m00_(data[0]),
				m01_(data[1]),
				m02_(data[2]),
				m03_(data[3]),
				m10_(data[4]),
				m11_(data[5]),
				m12_(data[6]),
				m13_(data[7]),
				m20_(data[8]),
				m21_(data[9]),
				m22_(data[10]),
				m23_(data[11])
		{
		}

		Matrix3x4(const Vector3& translation, const Quaternion& rotation, float scale)
		{
			SetRotation(rotation.RotationMatrix() * scale);
			SetTranslation(translation);
		}

		Matrix3x4(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
		{
			SetRotation(rotation.RotationMatrix().Scaled(scale));
			SetTranslation(translation);
		}

		Matrix3x4&operator= (const Matrix3x4& rhs)
		{
			m00_ = rhs.m00_;
			m01_ = rhs.m01_;
			m02_ = rhs.m02_;
			m03_ = rhs.m03_;
			m10_ = rhs.m10_;
			m11_ = rhs.m11_;
			m12_ = rhs.m12_;
			m13_ = rhs.m13_;
			m20_ = rhs.m20_;
			m21_ = rhs.m21_;
			m22_ = rhs.m22_;
			m23_ = rhs.m23_;
			return *this;
		}

		Matrix3x4&operator =(const Matrix3& rhs)
		{
			m00_ = rhs.m00_;
			m01_ = rhs.m01_;
			m02_ = rhs.m02_;
			m03_ = 0.0f;
			m10_ = rhs.m10_;
			m11_ = rhs.m11_;
			m12_ = rhs.m12_;
			m13_ = 0.0f;
			m20_ = rhs.m20_;
			m21_ = rhs.m21_;
			m22_ = rhs.m22_;
			m23_ = 0.0f;
			return *this;
		}

		bool operator ==(const Matrix3x4& rhs) const
		{
			const float* leftData = Data();
			const float* rightData = rhs.Data();
			for(unsigned i= 0; i< 12; ++i)
			{
				if(leftData[i] != rightData[i])
					return false;
			}
			return true;
		}

		bool operator !=(const Matrix3x4& rhs) const
		{
			return !(*this == rhs);
		}

		//Note, apply rotation and transform
		Vector3 operator* (const Vector3& rhs) const
		{
			return Vector3(
					(m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_ + m03_),
					(m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_ + m13_),
					(m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_ + m23_)
			);
		}

		Matrix3x4 operator +(const Matrix3x4& rhs) const
		{
			//todo
		}

		//todo




		void SetTranslation(const Vector3& translation)
		{
			m03_ = translation.x_;
			m13_ = translation.y_;
			m23_ = translation.z_;
		}

		void SetRotation(const Matrix3& rotation)
		{
			m00_ = rotation.m00_;
			m01_ = rotation.m01_;
			m02_ = rotation.m02_;
			m10_ = rotation.m10_;
			m11_ = rotation.m11_;
			m12_ = rotation.m12_;
			m20_ = rotation.m20_;
			m21_ = rotation.m21_;
			m22_ = rotation.m22_;
		}

		void SetScale(const Vector3& scale)
		{
			m00_ = scale.x_;
			m11_ = scale.y_;
			m22_ = scale.z_;
		}

		void SetScale(float scale)
		{
			m00_ = scale;
			m11_ = scale;
			m22_ = scale;
		}

		Matrix3 ToMatrix3() const
		{
			return Matrix3(
				m00_,
				m01_,
				m02_,
				m10_,
				m11_,
				m12_,
				m20_,
				m21_,
				m22_
			);
		}

		//todo,
		// Return the rotation matrix with scaling removed
		Matrix3 RotationMatrix() const
		{
			Vector3 invScale(
				1.0f / sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
				1.0f / sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				1.0f / sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
			);
			return ToMatrix3().Scaled(invScale);
		}

		Vector3 Translation() const
		{
			return Vector3(
				m03_,
				m13_,
				m23_
			);
		}

		Quaternion Rotation() const
		{
			return Quaternion(RotationMatrix());
		}

		//todo
		//return scaling part
		Vector3 Scale() const
		{
			return Vector3(
				sqrtf(m00_ * m00_ + m10_ * m10_ * m20_ * m20_),
				sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
				sqrtf(m02_ * m02_ * m12_ * m12_ + m22_ * m22_)
			);
		}

		//todo

		void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
		Matrix3x4 Inverse() const;
		const float* Data() const { return &m00_; }
		float Element(unsigned i, unsigned j) const
		{
			return Data()[i * 4 + j];
		}

		//todo

		float m00_;
		float m01_;
		float m02_;
		float m03_;

		float m10_;
		float m11_;
		float m12_;
		float m13_;

		float m20_;
		float m21_;
		float m22_;
		float m23_;

		static const Matrix3x4 ZERO;
		static const Matrix3x4 IDENTITY;

	};

}


#endif //URHO3DCOPY_MATRIX3X4_H
