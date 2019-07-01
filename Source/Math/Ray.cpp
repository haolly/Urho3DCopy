//
// Created by liuhao1 on 2017/12/27.
//

#include "Ray.h"
#include "MathDefs.h"
#include "Vector4.h"

namespace Urho3D
{

	//3D 空间中两条射线之间的最短连线
	Vector3 Ray::ClosestPoint(const Ray &ray) const
	{
		//Algorithm based on http://paulbourke.net/geometry/pointlineplane/
		Vector3 p13 = origin_ - ray.origin_;
		Vector3 p43 = ray.direction_;
		Vector3 p21 = direction_;

		float d1343 = p13.DotProduct(p43);
		float d4321 = p43.DotProduct(p21);
		float d1321 = p13.DotProduct(p21);
		float d4343 = p43.DotProduct(p43);
		float d2121 = p21.DotProduct(p21);

		float d = d2121 * d4343 - d4321 * d4321;
		if(Abs(d) < M_EPSILON)
			return origin_;
		float n = d1343 * d4321 - d1321 * d4343;
		float a = n / d;
		return origin_ + a * direction_;
	}

	float Ray::HitDistance(const Plane &plane) const
	{
		float d = plane.normal_.DotProduct(direction_);
		if(Abs(d) >= M_EPSILON)
		{
			float t = -(plane.normal_.DotProduct(origin_) + plane.d_) / d;
			if(t >= 0.0f)
				return t;
			else
				return M_INFINITY;
		}
		return M_INFINITY;
	}

	//todo, read
	float Ray::HitDistance(const BoundingBox &box) const
	{
		if(!box.Defined())
			return M_INFINITY;

		if(box.IsInside(origin_))
			return 0.0f;

		float dist = M_INFINITY;

		// Check for intersection in the X-direction
		if(origin_.x_ < box.min_.x_ && direction_.x_ > 0.0f)
		{
			float x = (box.min_.x_ - origin_.x_) / direction_.x_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box)
					dist = x;
			}
		}

		if(origin_.x_ > box.max_.x_ && direction_.x_ < 0.0f)
		{
			float x = (box.max_.x_ - origin_.x_) / direction_.x_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
					dist = x;
			}
		}

		// Check for intersection in the Y-direction
		if(origin_.y_ < box.min_.y_ && direction_.y_ > 0.0f)
		{
			float x = (box.min_.y_ - origin_.y_) / direction_.y_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
					dist = x;
			}
		}

		if(origin_.y_ > box.max_.y_ && direction_.y_ <0.0f)
		{
			float x = (box.max_.y_ - origin_.y_) / direction_.y_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.z_ >= box.min_.z_ && point.z_ <= box.max_.z_)
					dist = x;
			}
		}

		// Check for intersection in the Z-direction
		if(origin_.z_ < box.min_.z_ && direction_.z_ > 0.0f)
		{
			float x = (box.min_.z_ - origin_.z_) / direction_.z_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.z_)
					dist = x;
			}
		}

		if(origin_.z_ > box.max_.z_ && direction_.z_ < 0.0f)
		{
			float x = (box.max_.z_ - origin_.z_) / direction_.z_;
			if(x < dist)
			{
				Vector3 point = origin_ + x * direction_;
				if(point.x_ >= box.min_.x_ && point.x_ <= box.max_.x_ && point.y_ >= box.min_.y_ && point.y_ <= box.max_.y_)
					dist = x;
			}
		}
		return dist;
	}

	float Ray::HitDistance(const Sphere &sphere) const
	{
		Vector3 centeredOrigin = origin_ - sphere.center_;
		float squaredRadius = sphere.radius_ * sphere.radius_;

		if(centeredOrigin.LengthSquared() <= squaredRadius)
			return 0.0f;

		// Calculate intersection by quadratic equation
		float a = direction_.DotProduct(direction_);
		float b = 2.0f * centeredOrigin.DotProduct(direction_);
		float c = centeredOrigin.DotProduct(centeredOrigin) - squaredRadius;
		float d = b * b - 4.0f * a * c;

		if(d < 0.0f)
			return M_INFINITY;

		float dSqrt = sqrtf(d);
		float dist = (-b - dSqrt) / (2.0f * a);
		if(dist >= 0.0f)
			return dist;
		else
			return (-b + dSqrt) / (2.0f * a);
	}

	//Todo, return the distance from the ray origin ??
	//Note, key algorithm
	float Ray::HitDistance(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, Vector3 *outNormal,
	                       Vector3 *outBary) const
	{
		// Based on Fast, Minimum Storage Ray/Triangle Intersection by Möller & Trumbore
		// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
		// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
		// Calculate edge vectors
		Vector3 edge1(v1 - v0);
		Vector3 edge2(v2 - v0);

		// Calculate determinant & check backfacing
		Vector3 p(direction_.CrossProduct(edge2));
		float det = edge1.DotProduct(p);
		if (det >= M_EPSILON)
		{
			// Calculate u & v parameters and test
			Vector3 t(origin_ - v0);
			float u = t.DotProduct(p);
			if (u >= 0.0f && u <= det)
			{
				Vector3 q(t.CrossProduct(edge1));
				float v = direction_.DotProduct(q);
				if (v >= 0.0f && u + v <= det)
				{
					float distance = edge2.DotProduct(q) / det;
					// Discard hits behind the ray
					if (distance >= 0.0f)
					{
						// There is an intersection, so calculate distance & optional normal
						if (outNormal)
							*outNormal = edge1.CrossProduct(edge2);
						if (outBary)
							*outBary = Vector3(1 - (u / det) - (v / det), u / det, v / det);

						return distance;
					}
				}
			}
		}

		return M_INFINITY;
	}

	float Ray::HitDistance(const void *vertexData, unsigned vertexStride, unsigned vertexStart, unsigned vertexCount,
	                       Vector3 *outNormal, Vector2 *outUV, unsigned int uvOffset) const
	{
		float nearest = M_INFINITY;
		const unsigned char* vertices = (const unsigned char*)vertexData + vertexStart * vertexStride;
		unsigned index = 0;
		unsigned nearestIdx = M_MAX_UNSIGNED;
		Vector3 barycentric;
		Vector3* outBary = outUV ? &barycentric : nullptr;

		while (index + 2 < vertexCount)
		{
			const Vector3& v0 = *(const Vector3*)(&vertices[index * vertexStride]);
			const Vector3& v1 = *(const Vector3*)(&vertices[(index + 1) * vertexStride]);
			const Vector3& v2 = *(const Vector3*)(&vertices[(index + 2) * vertexStride]);
			float distance = HitDistance(v0, v1, v2, outNormal, outBary);
			if(distance < nearest)
			{
				nearestIdx = index;
				nearest = distance;
			}
			index += 3;
		}

		if(outUV)
		{
			if(nearest == M_MAX_UNSIGNED)
				*outUV = Vector2::ZERO;
			else
			{
				//Todo, vertex data plus uvOffset could get uv data ??
				const Vector2& uv0 = *(const Vector2*)(&vertices[uvOffset + nearestIdx * vertexStride]);
				const Vector2& uv1 = *(const Vector2*)(&vertices[uvOffset + (nearestIdx + 1) * vertexStride]);
				const Vector2& uv2 = *(const Vector2*)(&vertices[uvOffset + (nearestIdx + 2) * vertexStride]);
				//Note, barycenter interpolate
				*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
								 uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
			}
		}
		return nearest;
	}

	float Ray::HitDistance(const void *vertexData, unsigned vertexStride, const void *indexData, unsigned indexSize,
	                       unsigned indexStart, unsigned indexCount, Vector3 *outNormal, Vector2 *outUV,
	                       unsigned int uvOffset) const
	{
		float nearest = M_INFINITY;
		const unsigned char* vertices = (const unsigned char*)vertexData;
		Vector3 barycentric;
		Vector3* outBary = outUV ? &barycentric : nullptr;

		if(indexSize == sizeof(unsigned short))
		{
			const unsigned short* indices = (const unsigned short*)indexData + indexStart;
			const unsigned short* indicesEnd = indices + indexCount;
			const unsigned short* nearestIndices = nullptr;
			while(indices < indicesEnd)
			{
				const Vector3& v0 = *(const Vector3*)(&vertices[indices[0] * vertexStride]);
				const Vector3& v1 = *(const Vector3*)(&vertices[indices[1] * vertexStride]);
				const Vector3& v2 = *(const Vector3*)(&vertices[indices[2] * vertexStride]);

				float distance = HitDistance(v0, v1, v2, outNormal, outBary);
				if(distance < nearest)
				{
					nearestIndices = indices;
					nearest = distance;
				}
				indices += 3;
			}

			if(outUV)
			{
				if(nearestIndices == nullptr)
				{
					*outUV = Vector2::ZERO;
				}
				else
				{
					//todo, uvOffset ??
					const Vector2& uv0 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[0] * vertexStride]);
					const Vector2& uv1 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[1] * vertexStride]);
					const Vector2& uv2 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[2] * vertexStride]);

					*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
									uv0.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
				}
			}
		}
		else
		{
			const unsigned* indices = (const unsigned*)indexData + indexStart;
			const unsigned* indicesEnd = indices + indexCount;
			const unsigned* nearestIndices = nullptr;

			while (indices < indicesEnd)
			{
				const Vector3& v0 = *(const Vector3*)(&vertices[indices[0] * vertexStride]);
				const Vector3& v1 = *(const Vector3*)(&vertices[indices[1] * vertexStride]);
				const Vector3& v2 = *(const Vector3*)(&vertices[indices[2] * vertexStride]);
				float distance = HitDistance(v0, v1, v2, outNormal, outBary);
				if(distance < nearest)
				{
					nearestIndices = indices;
					nearest = distance;
				}
				indices += 3;
			}

			if(outUV)
			{
				if(nearestIndices == nullptr)
					*outUV = Vector2::ZERO;
				else
				{
					const Vector2& uv0 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[0] * vertexStride]);
					const Vector2& uv1 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[1] * vertexStride]);
					const Vector2& uv2 = *(const Vector2*)(&vertices[uvOffset + nearestIndices[2] * vertexStride]);
					*outUV = Vector2(uv0.x_ * barycentric.x_ + uv1.x_ * barycentric.y_ + uv2.x_ * barycentric.z_,
					                 uv1.y_ * barycentric.x_ + uv1.y_ * barycentric.y_ + uv2.y_ * barycentric.z_);
				}
			}
		}
		return nearest;
	}

	bool
	Ray::InsideGeometry(const void *vertexData, unsigned vertexSize, unsigned vertexStart, unsigned vertexCount) const
	{
		float currentFrontFace = M_INFINITY;
		float currentBackFace = M_INFINITY;
		const unsigned char* vertices = (const unsigned char*)vertexData + vertexStart * vertexSize;
		unsigned index = 0;

		while(index + 2 < vertexCount)
		{
			//todo, why not the point be sequential ?
			//todo, the vertexSize is equal to v0/v1/v2?
			const Vector3& v0 = *(const Vector3*)(&vertices[index * vertexSize]);
			const Vector3& v1 = *(const Vector3*)(&vertices[(index + 1) * vertexSize]);
			const Vector3& v2 = *(const Vector3*)(&vertices[(index + 2) * vertexSize]);
			float frontFaceDistance = HitDistance(v0, v1, v2);
			float backFaceDistance = HitDistance(v2, v1, v0);
			currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : M_INFINITY, currentFrontFace);
			currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : M_INFINITY, currentBackFace);
			index += 3;
		}

		if(currentFrontFace != M_INFINITY || currentBackFace != M_INFINITY)
			return currentBackFace < currentFrontFace;
		return false;
	}

	bool Ray::InsideGeometry(const void *vertexData, unsigned vertexSize, const void *indexData, unsigned indexSize,
	                         unsigned indexStart, unsigned indexCount) const
	{
		float currentFrontFace = M_INFINITY;
		float currentBackFace = M_INFINITY;
		const unsigned char* vertices = (const unsigned char*)vertexData;

		//16 bit indices
		if(indexSize == sizeof(unsigned short))
		{
			const unsigned short* indices = (const unsigned short*)indexData + indexStart;
			const unsigned short* indicesEnd = indices + indexCount;

			while (indices < indicesEnd)
			{
				const Vector3& v0 = *(const Vector3*)(&vertices[ indices[0] * vertexSize]);
				const Vector3& v1 = *(const Vector3*)(&vertices[ indices[1] * vertexSize]);
				const Vector3& v2 = *(const Vector3*)(&vertices[ indices[2] * vertexSize]);
				//Todo, what is the difference between frontFace and backFace??
				//一个三角形的正面和反面和射线的距离应该是一样的呀....
				float frontFaceDistance = HitDistance(v0, v1, v2);
				float backFaceDistance = HitDistance(v2, v1, v0);
				currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : M_INFINITY, currentFrontFace);
				currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : M_INFINITY, currentBackFace);
				indices += 3;
			}
		}
		else
		{
			//32-bit indices
			const unsigned* indices = (const unsigned*)indexData + indexStart;
			const unsigned* indicesEnd = indices + indexCount;

			while (indices < indicesEnd)
			{
				const Vector3& v0 = *(const Vector3*)(&vertices[ indices[0] * vertexSize]);
				const Vector3& v1 = *(const Vector3*)(&vertices[ indices[1] * vertexSize]);
				const Vector3& v2 = *(const Vector3*)(&vertices[ indices[2] * vertexSize]);
				float frontFaceDistance = HitDistance(v0, v1, v2);
				float backFaceDistance = HitDistance(v2, v1, v0);
				currentFrontFace = Min(frontFaceDistance > 0.0f ? frontFaceDistance : M_INFINITY, currentFrontFace);
				currentBackFace = Min(backFaceDistance > 0.0f ? backFaceDistance : M_INFINITY, currentBackFace);
				indices += 3;
			}
		}

		// If the closest face is a backface, that means the ray originates from the inside of the geometry
		// todo, why?
		if(currentFrontFace != M_INFINITY || currentBackFace != M_INFINITY)
			return currentBackFace < currentFrontFace;
		return false;
	}

	Ray Ray::Transformed(const Matrix3x4 &transform) const
	{
		Ray ret;
		//Note, 右乘向量
		ret.origin_ = transform * origin_;
		ret.direction_ = transform * Vector4(direction_, 0.0f);
		return ret;
	}
}