//
// Created by liuhao1 on 2018/1/29.
//

#include "OctreeQuery.h"

namespace Urho3D
{

	Intersection PointOctreeQuery::TestOctant(const BoundingBox &box, bool inside)
	{
		if(inside)
			return INSIDE;
		else
			return box.IsInside(point_);
	}

	void PointOctreeQuery::TestDrawables(Drawable **start, Drawable **end, bool inside)
	{
		while(start != end)
		{
			Drawable* drawable = *start++;
			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
			{
				if(inside || drawable->GetWorldBoundingBox().IsInside(point_))
					result_.Push(drawable);
			}
		}
	}

	Intersection AllContentOctreeQuery::TestOctant(const BoundingBox &box, bool inside)
	{
		return INSIDE;
	}

	void AllContentOctreeQuery::TestDrawables(Drawable **start, Drawable **end, bool inside)
	{
		while(start != end)
		{
			Drawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask()) & viewMask_)
				result_.Push(drawable);
		}
	}
}