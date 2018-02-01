//
// Created by liuhao1 on 2018/1/27.
//

#ifndef URHO3DCOPY_OCTREE_H
#define URHO3DCOPY_OCTREE_H

#include "../Math/MathDefs.h"
#include "../Math/BoundingBox.h"
#include "Drawable.h"
#include "OctreeQuery.h"

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
		bool CheckDrawableFit(const BoundingBox& drawableBox) const;

		void AddDrawable(Drawable* drawable)
		{
			drawable->SetOctant(this);
			drawables_.Push(drawable);
			IncDrawableCount();
		}

		void RemoveDrawable(Drawable* drawable, bool resetOctant = true)
		{
			if(drawables_.Remove(drawable))
			{
				if(resetOctant)
					drawable->SetOctant(nullptr);
				DecDrawableCount();
			}
		}

		const BoundingBox& GetWorldBoundingBox() const { return worldBoundingBox_; }
		const BoundingBox& GetCullingBox() const { return cullingBox_; }
		unsigned GetLevel() const { return level_; }
		Octant* GetParent() const { return parent_; }
		Octree* GetRoot() const { return root_;}
		unsigned GetNumDrawables() const { return numDrawables_; }

		bool IsEmpty() { return numDrawables_ == 0; }

		void ResetRoot() ;
		void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

	protected:
		void Initialize(const BoundingBox& box);
		void GetDrawablesInternal(OctreeQuery& query, bool inside) const;
		void GetDrawablesInternal(RayOctreeQuery& query) const;
		void GetDrawablesOnlyInternal(RayOctreeQuery& query, PODVector<Drawable*>& drawables) const;

		void IncDrawableCount()
		{
			++numDrawables_;
			if(parent_)
				parent_->IncDrawableCount();
		}

		void DecDrawableCount()
		{
			Octant* parent = parent_;
			--numDrawables_;
			if(!numDrawables_)
			{
				if(parent)
					parent->DeleteChild(index_);
			}
			if(parent)
				parent->DecDrawableCount();
		}

		BoundingBox worldBoundingBox_;
		// Bounding box used for drawable object fitting
		//todo ? what is the difference between worldBoundingBox??
		BoundingBox cullingBox_;
		PODVector<Drawable*> drawables_;
		// Child octants
		Octant* childrent_[NUM_OCTANTS];
		// World bounding box center
		Vector3 center_;
		//todo, what's the purpose ?
		Vector3 halfSize_;
		// Subdivision level
		// todo
		unsigned level_;
		// Number of drawable objects in this octant and child octants
		unsigned numDrawables_;
		Octant* parent_;
		Octree* root_;
		// Octant index relative to its siblings or ROOT_INDEX for root octant
		unsigned index_;
	};


	class Octree : public Component , public Octant
	{
		URHO3D_OBJECT(Octree, Component);
	public:
		Octree(Context* context);
		virtual ~Octree() override;

		static void RegisterObject(Context* context);

		// Set size and maximum subdivision levels. If octree is not empty, drawable objects will be temporarily moved to the root
		void SetSize(const BoundingBox& box, unsigned numLevels);
		void Update(const FrameInfo& frame);
		void AddManualDrawable(Drawable* drawable);
		void RemoveManualDrawable(Drawable* drawable);

		void GetDrawables(OctreeQuery& query) const;
		void Raycast(RayOctreeQuery& query) const;
		void RaycastSingle(RayOctreeQuery& query) const;

		unsigned GetNumLevels() const { return numLevels_; }

		void QueueUpdate(Drawable* drawable);
		void CancelUpdate(Drawable* drawable);
		void DrawDebugGeometry(bool depthTest);

	private:
		void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
		void UpdateOctreeSize()
		{
			SetSize(worldBoundingBox_, numLevels_);
		}

		// Drawable objects that require update
		PODVector<Drawable*> drawableUpdates_;
		// Drawable objects that were inserted during threaded update phase
		PODVector<Drawable*> threadedDrawableUpdates_;
		Mutex octreeMutex_;
		// Ray query temporary list of drawalbes
		mutable PODVector<Drawable*> rayQueryDrawables_;
		// Subdivision level
		unsigned numLevels_;

	};

}



#endif //URHO3DCOPY_OCTREE_H
