//
// Created by liuhao1 on 2018/1/27.
//

#ifndef URHO3DCOPY_OCTREE_H
#define URHO3DCOPY_OCTREE_H

#include "../Math/MathDefs.h"
#include "../Math/BoundingBox.h"

namespace Urho3D
{
	class Octree;

	static const int NUM_OCTANTS = 8;
	static const unsigned ROOT_INDEX = M_MAX_UNSIGNED;

	class Octant
	{
	public:
		Octant(const BoundingBox& box, unsigned level, Octant* parent, Octree* root, unsigned index = ROOT_INDEX);
		virtual ~Octant();

		Octant* GetOrCreateChild(unsigned index);
		void DeleteChild(unsigned index);
		void InsertDrawable(Drawable* drawable);
		bool CheckDrawableFit(const BoundingBox& box) const;

		void AddDrawable(Drawable* drawable)
		{
			drawable->SetOctant(this);
			//todo
		}

	};


	class Octree
	{

	};
}



#endif //URHO3DCOPY_OCTREE_H
