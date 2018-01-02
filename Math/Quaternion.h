//
// Created by liuhao1 on 2017/12/23.
//

#ifndef URHO3DCOPY_QUATERNION_H
#define URHO3DCOPY_QUATERNION_H

#include "Vector3.h"
#include "Matrix3.h"

namespace Urho3D
{
	/**
	 * 基本都是直接拷贝原有的代码，完全没理解背后的数学原理，todo, 后面有空再看
	 */
	class Quaternion
	{
	public:
		Quaternion() :
			w_(1.0f),
			x_(0.0f),
			y_(0.0f),
			z_(0.0f)
		{
		}

		Quaternion(const Quaternion& quat) :
			w_(quat.w_),
			x_(quat.x_),
			y_(quat.y_),
			z_(quat.z_)
		{
		}

		Quaternion(float w, float x, float y, float z) :
			w_(w),
			x_(x),
			y_(y),
			z_(z)
		{
		}

		explicit Quaternion(const float* data) :
			w_(data[0]),
			x_(data[1]),
			y_(data[2]),
			z_(data[3])
		{
		}

		Quaternion(float angle, const Vector3& axis)
		{
			FromAngleAxis(angle, axis);
		}



		void FromAngleAxis(float angle, const Vector3& axis);
		void FromEulerAngles(float x, float y, float z);
		void FromRotationTo(const Vector3& start, const Vector3& end);
		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
		void FromRotationMatrix(const Matrix3& matrix);
		bool FromLookRotation(const Vector3& direction, const Vector3& up = Vector3::UP);

		bool IsNaN() const
		{
			return Urho3D::IsNaN(w_) || Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_) || Urho3D::IsNaN(z_);
		}

		Vector3 EulerAngles() const;

		float w_;
		float x_;
		float y_;
		float z_;
		static const Quaternion IDENTITY;
	};
}



#endif //URHO3DCOPY_QUATERNION_H
