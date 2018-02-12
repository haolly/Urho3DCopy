//
// Created by liuhao1 on 2017/12/23.
//

#include "Quaternion.h"

namespace Urho3D
{

	void Quaternion::FromAngleAxis(float angle, const Vector3 &axis)
	{
		Vector3 normAxis = axis.Normalized();
		angle *= M_DEGTORAD_2;
		float sinAngle = sinf(angle);
		float cosAngle = cosf(angle);

		w_ = cosAngle;
		x_ = normAxis.x_ * sinAngle;
		y_ = normAxis.y_ * sinAngle;
		z_ = normAxis.z_ * sinAngle;
	}

	void Quaternion::FromEulerAngles(float x, float y, float z)
	{
		// Order of rotations: Z first, then X, then Y (mimics typical FPS camera with gimbal lock at top/bottom)
		x *= M_DEGTORAD_2;
		y *= M_DEGTORAD_2;
		z *= M_DEGTORAD_2;
		float sinX = sinf(x);
		float cosX = cosf(x);
		float sinY = sinf(y);
		float cosY = cosf(y);
		float sinZ = sinf(z);
		float cosZ = cosf(z);

		w_ = cosY * cosX * cosZ + sinY * sinX * sinZ;
		x_ = cosY * sinX * cosZ + sinY * cosX * sinZ;
		y_ = sinY * cosX * cosZ - cosY * sinX * sinZ;
		z_ = cosY * cosX * sinZ - sinY * sinX * cosZ;
	}

	void Quaternion::FromRotationTo(const Vector3 &start, const Vector3 &end)
	{
		Vector3 normStart = start.Normalized();
		Vector3 normEnd = end.Normalized();
		float d = normStart.DotProduct(normEnd);

		if (d > -1.0f + M_EPSILON)
		{
			Vector3 c = normStart.CrossProduct(normEnd);
			float s = sqrtf((1.0f + d) * 2.0f);
			float invS = 1.0f / s;

			x_ = c.x_ * invS;
			y_ = c.y_ * invS;
			z_ = c.z_ * invS;
			w_ = 0.5f * s;
		}
		else {
			Vector3 axis = Vector3::RIGHT.CrossProduct(normStart);
			if (axis.Length() < M_EPSILON)
				axis = Vector3::UP.CrossProduct(normStart);

			FromAngleAxis(180.f, axis);
		}
	}

	void Quaternion::FromAxes(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis)
	{
		Matrix3 matrix(
				xAxis.x_, yAxis.x_, zAxis.x_,
				xAxis.y_, yAxis.y_, zAxis.y_,
				xAxis.z_, yAxis.z_, zAxis.z_
		);

		FromRotationMatrix(matrix);
	}

	void Quaternion::FromRotationMatrix(const Matrix3 &matrix)
	{
		float t = matrix.m00_ + matrix.m11_ + matrix.m22_;

		if (t > 0.0f)
		{
			float invS = 0.5f / sqrtf(1.0f + t);

			x_ = (matrix.m21_ - matrix.m12_) * invS;
			y_ = (matrix.m02_ - matrix.m20_) * invS;
			z_ = (matrix.m10_ - matrix.m01_) * invS;
			w_ = 0.25f / invS;
		}
		else
		{
			if (matrix.m00_ > matrix.m11_ && matrix.m00_ > matrix.m22_)
			{
				float invS = 0.5f / sqrtf(1.0f + matrix.m00_ - matrix.m11_ - matrix.m22_);

				x_ = 0.25f / invS;
				y_ = (matrix.m01_ + matrix.m10_) * invS;
				z_ = (matrix.m20_ + matrix.m02_) * invS;
				w_ = (matrix.m21_ - matrix.m12_) * invS;
			}
			else if (matrix.m11_ > matrix.m22_)
			{
				float invS = 0.5f / sqrtf(1.0f + matrix.m11_ - matrix.m00_ - matrix.m22_);

				x_ = (matrix.m01_ + matrix.m10_) * invS;
				y_ = 0.25f / invS;
				z_ = (matrix.m12_ + matrix.m21_) * invS;
				w_ = (matrix.m02_ - matrix.m20_) * invS;
			}
			else
			{
				float invS = 0.5f / sqrtf(1.0f + matrix.m22_ - matrix.m00_ - matrix.m11_);

				x_ = (matrix.m02_ + matrix.m20_) * invS;
				y_ = (matrix.m12_ + matrix.m21_) * invS;
				z_ = 0.25f / invS;
				w_ = (matrix.m10_ - matrix.m01_) * invS;
			}
		}
	}

	// Note, direction and upDirection is arbitrary, they are not need perpendicular to each other
	bool Quaternion::FromLookRotation(const Vector3 &direction, const Vector3& upDirection)
	{
		Quaternion ret;
		Vector3 forward = direction.Normalized();

		Vector3 v = forward.CrossProduct(upDirection);
		// If direction & upDirection are parallel and crossproduct becomes zero, use FromRotationTo() fallback
		if (v.LengthSquared() >= M_EPSILON)
		{
			v.Normalize();
			Vector3 up = v.CrossProduct(forward);
			Vector3 right = up.CrossProduct(forward);
			ret.FromAxes(right, up, forward);
		}
		else
			ret.FromRotationTo(Vector3::FORWARD, forward);

		if (!ret.IsNaN())
		{
			(*this) = ret;
			return true;
		}
		else
			return false;
	}
}