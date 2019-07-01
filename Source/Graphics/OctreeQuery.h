//
// Created by liuhao1 on 2018/1/29.
//

#ifndef URHO3DCOPY_OCTREEQUERY_H
#define URHO3DCOPY_OCTREEQUERY_H

#include "../Container/Vector.h"
#include "../Math/BoundingBox.h"
#include "Drawable.h"
#include "../Math/Ray.h"

namespace Urho3D
{
	class Drawable;
	class Node;

	class OctreeQuery
	{
	public:
		OctreeQuery(PODVector<Drawable*>& result, unsigned char drawableFlags, unsigned viewMask)  :
				result_(result),
				drawableFlags_(drawableFlags),
				viewMask_(viewMask)
		{
		}

		virtual ~OctreeQuery()
		{

		}

		virtual Intersection TestOctant(const BoundingBox& box, bool inside) = 0;
		virtual void TestDrawables(Drawable** start, Drawable** end, bool inside) = 0;

		PODVector<Drawable*>& result_;
		unsigned char drawableFlags_;
		// Drawable layers to include
		unsigned viewMask_;

		OctreeQuery(const OctreeQuery& rhs) = delete;
		OctreeQuery&operator =(const OctreeQuery& rhs) = delete;
	};

	class PointOctreeQuery : public OctreeQuery
	{
	public:
		PointOctreeQuery(PODVector<Drawable*>& result, const Vector3& point, unsigned char drawableFlags = DRAWABLE_ANY,
						unsigned viewMask = DEFAULT_VIEWMASK) :
				OctreeQuery(result, drawableFlags, viewMask),
				point_(point)
		{
		}

		virtual Intersection TestOctant(const BoundingBox& box, bool inside) override ;
		virtual void TestDrawables(Drawable** start, Drawable** end, bool inside) override ;

		Vector3 point_;
	};

	//todo

	struct OctreeQueryResult
	{
		OctreeQueryResult() :
				drawable_(nullptr),
				node_(nullptr)
		{
		}

		bool operator !=(const OctreeQueryResult& rhs) const
		{
			return drawable_ != rhs.drawable_ || node_ != rhs.node_;
		}

		Drawable* drawable_;
		Node* node_;
	};

	enum RayQueryLevel
	{
		RAY_AABB = 0,
		RAY_OBB,
		RAY_TRIANGLE,
		RAY_TRIANGLE_UV
	};

	struct RayQueryResult
	{
		RayQueryResult() :
				drawable_(nullptr),
				node_(nullptr)
		{
		}

		bool operator !=(const RayQueryResult& rhs) const
		{
			return position_ != rhs.position_ ||
			       normal_ != rhs.normal_ ||
			       textureUV_ != rhs.textureUV_ ||
			       distance_ != rhs.distance_ ||
			       drawable_ != rhs.drawable_ ||
			       node_ != rhs.node_ ||
			       subObject_ != rhs.subObject_;
		}

		// Hit position in world space
		Vector3 position_;
		Vector3 normal_;
		Vector2 textureUV_;
		float distance_;
		Drawable* drawable_;
		Node* node_;
		// Drawable specific subobject if applicable
		unsigned subObject_;
	};

	class RayOctreeQuery
	{
	public:
		RayOctreeQuery(PODVector<RayQueryResult>& result, const Ray& ray, RayQueryLevel level = RAY_TRIANGLE, float maxDistance = M_INFINITY,
						unsigned char drawableFlags = DRAWABLE_ANY, unsigned viewMask = DEFAULT_VIEWMASK) :
				result_(result),
				ray_(ray),
				drawableFlags_(drawableFlags),
				viewMask_(viewMask),
				maxDistance_(maxDistance),
				level_(level)
		{
		}

		PODVector<RayQueryResult>& result_;
		Ray ray_;
		unsigned char drawableFlags_;
		unsigned viewMask_;
		float maxDistance_;
		RayQueryLevel level_;

		RayOctreeQuery(const RayOctreeQuery& rhs) = delete;
		RayOctreeQuery&operator =(const RayOctreeQuery& rhs) = delete;

	private:
	};

	class AllContentOctreeQuery : public OctreeQuery
	{
	public:
		AllContentOctreeQuery(PODVector<Drawable *> &result, unsigned char drawableFlags, unsigned viewMask) :
				OctreeQuery(result, drawableFlags, viewMask)
		{
		}

		virtual Intersection TestOctant(const BoundingBox& box, bool inside) override;
		virtual void TestDrawables(Drawable** start, Drawable** end, bool inside) override ;
	};
}


#endif //URHO3DCOPY_OCTREEQUERY_H
