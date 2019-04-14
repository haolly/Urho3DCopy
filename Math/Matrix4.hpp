//
// Created by liuhao on 2019-04-13.
//

#ifndef URHO3DCOPY_MATRIX4_HPP
#define URHO3DCOPY_MATRIX4_HPP

#include "../Math/Quaternion.h"
#include "../Math/Vector4.h"

namespace Urho3D
{
	class Matrix3x4;

	class Matrix4
	{
	public:
		Matrix4() noexcept
			: m00_(1.0f),
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
			m23_(0.0f),

			m30_(0.0f),
			m31_(0.0f),
			m32_(0.0f),
			m33_(1.0f)
		{
		}

		Matrix4(float v00, float v01, float v02, float v03,
				float v10, float v11, float v12, float v13,
				float v20, float v21, float v22, float v23,
				float v30, float v31, float v32, float 33)
		{
			//todo
		}

		Matrix4(const Matrix4& matrix) noexcept
		{
			//todo
		}

		Matrix4 operator *(float rhs) const
		{
			return Matrix4(
					m00_ * rhs,
					m01_ * rhs,
					m02_ * rhs,
					m03_ * rhs,

					m10_ * rhs,
					m11_ * rhs,
					m12_ * rhs,
					m13_ * rhs,

					m20_ * rhs,
					m21_ * rhs,
					m22_ * rhs,
					m23_ * rhs,

					m30_ * rhs,
					m31_ * rhs,
					m32_ * rhs,
					m33_ * rhs
					)
		}

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

		float m30_;
		float m31_;
		float m32_;
		float m33_;

		static const Matrix4 ZERO;
		static const Matrix4 IDENTITY;
	};

	inline Matrix4 operator *(float lhs, const Matrix4& rhs)
	{
		return rhs * lhs;
	}
}



#endif //URHO3DCOPY_MATRIX4_HPP
