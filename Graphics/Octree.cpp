//
// Created by liuhao1 on 2018/1/27.
//

#include "Octree.h"
#include "../Core/WorkQueue.h"
#include "Graphics.h"
#include "../Core/CoreEvent.h"
#include "../Container/Sort.h"
#include "../Scene/Scene.h"
#include "../Core/Thread.h"

namespace Urho3D
{

	static const float DEFAULT_OCTREE_SIZE = 1000.0f;
	static const int DEFAULT_OCTREE_LEVELS = 8;

	extern const char* SUBSYSTEM_CATEGORY;

	void UpdateDrawablesWork(const WorkItem* item, unsigned threadIndex)
	{
		const FrameInfo& frame = *(reinterpret_cast<FrameInfo*>(item->aux_));
		auto** start = reinterpret_cast<Drawable**>(item->start_);
		auto** end = reinterpret_cast<Drawable**>(item->end_);

		while(start != end)
		{
			Drawable* drawable = *start;
			if(drawable)
				drawable->Update(frame);
			++start;
		}
	}

	inline bool CompareRayQueryResult(const RayQueryResult& lhs, const RayQueryResult& rhs)
	{
		return lhs.distance_ < rhs.distance_;
	}

	Octant::Octant(const BoundingBox &box, unsigned level, Octant *parent, Octree *root, unsigned int index) :
		level_(level),
		numDrawables_(0),
		parent_(parent),
		root_(root),
		index_(index)
	{
		Initialize(box);

		for(unsigned i=0; i< NUM_OCTANTS; ++i)
		{
			childrent_[i] = nullptr;
		}
	}

	Octant::~Octant()
	{
		if(root_)
		{
			for(auto it = drawables_.Begin(); it != drawables_.End(); ++it)
			{
				(*it)->SetOctant(root_);
				root_->drawables_.Push(*it);
				root_->QueueUpdate(*it);
			}
			drawables_.Clear();
			numDrawables_ = 0;
		}

		for(unsigned i=0; i< NUM_OCTANTS; ++i)
			DeleteChild(i);
	}

	// left hand coordinate
	Octant *Octant::GetOrCreateChild(unsigned index)
	{
		if(childrent_[index])
			return childrent_[index];

		Vector3 newMin = worldBoundingBox_.min_;
		Vector3 newMax = worldBoundingBox_.max_;
		Vector3 oldCenter = worldBoundingBox_.Center();

		// 1,3,5,7, right
		if(index & 1)
			newMin.x_ = oldCenter.x_;
		else
			newMax.x_ = oldCenter.x_;

		// 2,3,6,7 , upper
		if(index & 2)
			newMin.y_ = oldCenter.y_;
		else
			newMax.y_ = oldCenter.y_;

		// 4,5,6,7 , back
		if(index & 4)
			newMin.z_ = oldCenter.z_;
		else
			newMax.z_ = oldCenter.z_;

		childrent_[index] = new Octant(BoundingBox(newMin, newMax), level_ + 1, this, root_, index);
	}

	void Octant::DeleteChild(unsigned index)
	{
		assert(index < NUM_OCTANTS);
		delete childrent_[index];
		childrent_[index] = nullptr;
	}

	void Octant::InsertDrawable(Drawable *drawable)
	{
		const BoundingBox& box = drawable->GetWorldBoundingBox();

		bool insertHere;
		if(this == root_)
			insertHere = !drawable->IsOccludee() || cullingBox_.IsInside(box) != INSIDE || CheckDrawableFit(box);
		else
			insertHere = CheckDrawableFit(box);

		if(insertHere)
		{
			Octant* oldOctant = drawable->octant_;
			if(oldOctant != this)
			{
				// Add first, then remove, because drawable count going to zero deletes the octree branch in question
				AddDrawable(drawable);
				if(oldOctant)
					oldOctant->RemoveDrawable(drawable, false);
			}
		}
		else
		{
			Vector3 boxCenter = box.Center();
			unsigned x = boxCenter.x_ < center_.x_ ? 0 : 1;
			unsigned y = boxCenter.y_ < center_.y_ ? 0 : 2;
			unsigned z = boxCenter.z_ < center_.z_ ? 0 : 4;

			GetOrCreateChild(x + y + z)->InsertDrawable(drawable);
		}
	}

	bool Octant::CheckDrawableFit(const BoundingBox& drawableBox) const
	{
		Vector3 boxSize = drawableBox.Size();

		// If max split level, size always OK, otherwise check that box is at least half size of octant
		// Note, must be HALF size bigger in either axis
		if(level_ >= root_->GetNumLevels() || boxSize.x_ >= halfSize_.x_ || boxSize.y_ >= halfSize_.y_ || boxSize.z_ >= halfSize_.z_)
			return true;
		else
		{
			// Check if the box can not fit a child octant's culling box, in that case size OK(must insert here)
			// todo, culling box ??
			if(drawableBox.min_.x_ <= worldBoundingBox_.min_.x_ - 0.5 * halfSize_.x_ ||
				drawableBox.max_.x_ >= worldBoundingBox_.max_.x_ + 0.5 * halfSize_.x_ ||
				drawableBox.min_.y_ <= worldBoundingBox_.min_.y_ - 0.5 * halfSize_.y_ ||
				drawableBox.max_.y_ >= worldBoundingBox_.max_.y_ + 0.5 * halfSize_.y_ ||
				drawableBox.min_.z_ <= worldBoundingBox_.min_.z_ - 0.5 * halfSize_.z_ ||
				drawableBox.max_.z_ >= worldBoundingBox_.max_.z_ + 0.5 * halfSize_.z_)
				return true;
		}

		// Bounding box is too small, should crate a child octant
		return false;
	}

	void Octant::ResetRoot()
	{
		root_ = nullptr;

		for(auto it = drawables_.Begin(); it != drawables_.End(); ++it)
			(*it)->SetOctant(nullptr);

		for(unsigned i=0; i< NUM_OCTANTS; ++i)
		{
			if(childrent_[i])
				childrent_[i]->ResetRoot();
		}
	}

	void Octant::DrawDebugGeometry(DebugRenderer *debug, bool depthTest)
	{

	}

	void Octant::Initialize(const BoundingBox &box)
	{
		worldBoundingBox_ = box;
		center_ = box.Center();
		halfSize_ = 0.5f * box.Size();
		// todo, Why bigger than worldBoundingBox ??
		cullingBox_ = BoundingBox(worldBoundingBox_.min_ - halfSize_, worldBoundingBox_.max_ + halfSize_);
	}

	void Octant::GetDrawablesInternal(OctreeQuery &query, bool inside) const
	{
		if(this != root_)
		{
			Intersection res = query.TestOctant(cullingBox_, inside);
			if(res == INSIDE)
				inside = true;
			else if(res == OUTSIDE)
			{
				return;
			}
		}

		if(drawables_.Size())
		{
			Drawable** start = const_cast<Drawable**>(&drawables_[0]);
			Drawable** end = start + drawables_.Size();
			query.TestDrawables(start, end, inside);
		}

		for(unsigned i=0; i < NUM_OCTANTS; ++i)
		{
			if(childrent_[i])
				childrent_[i]->GetDrawablesInternal(query, inside);
		}
	}

	void Octant::GetDrawablesInternal(RayOctreeQuery &query) const
	{
		float octantDist = query.ray_.HitDistance(cullingBox_);
		if(octantDist >= query.maxDistance_)
			return;

		if(drawables_.Size())
		{
			Drawable** start = const_cast<Drawable**>(&drawables_[0]);
			Drawable** end = start + drawables_.Size();

			while(start != end)
			{
				Drawable* drawable = *start++;

				if((drawable->GetDrawableFlags() & query.drawableFlags_) && (drawable->GetViewMask() & query.viewMask_))
					drawable->ProcessRayQuery(query, query.result_);
			}
		}

		for(unsigned i=0; i< NUM_OCTANTS; ++i)
		{
			if(childrent_[i])
				childrent_[i]->GetDrawablesInternal(query);
		}
	}

	void Octant::GetDrawablesOnlyInternal(RayOctreeQuery &query, PODVector<Drawable *> &drawables) const
	{
		float octantDist = query.ray_.HitDistance(cullingBox_);
		if(octantDist >= query.maxDistance_)
			return;

		if(drawables_.Size())
		{
			Drawable** start = static_cast<Drawable**>(&drawables_[0]);
			Drawable** end = start + drawables_.Size();

			while(start != end)
			{
				Drawable* drawable = *start++;
				if((drawable->GetDrawableFlags() & query.drawableFlags_) && (drawable->GetViewMask() & query.viewMask_))
					drawables.Push(drawable);
			}
		}

		for(unsigned i=0; i< NUM_OCTANTS; ++i)
		{
			if(childrent_[i])
				childrent_[i]->GetDrawablesOnlyInternal(query, drawables);
		}
	}

	Octree::Octree(Context *context) :
		Component(context),
		Octant(BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), 0, nullptr, this),
		numLevels_(DEFAULT_OCTREE_LEVELS)
	{
		// If the engine is running headless, subscribe to RenderUpdate events for manually updating the octree
		// to allow raycasts and animation update
		// todo, otherwise, use which function to update ??
		if(!GetSubsystem<Graphics>())
			SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(Octree, HandleRenderUpdate));
	}

	Octree::~Octree()
	{
		drawableUpdates_.Clear();
		ResetRoot();
	}

	void Octree::RegisterObject(Context *context)
	{
		context->RegisterFactory<Octree>(SUBSYSTEM_CATEGORY);

		Vector3 defaultBoundsMin = -Vector3::ONE * DEFAULT_OCTREE_SIZE;
		Vector3 defaultBoundsMax = Vector3::ONE * DEFAULT_OCTREE_SIZE;

		URHO3D_ATTRIBUTE_EX("Bounding Box Min", Vector3, worldBoundingBox_.min_, UpdateOctreeSize, defaultBoundsMin, AM_DEFAULT);
		URHO3D_ATTRIBUTE_EX("Bounding Box Max", Vector3, worldBoundingBox_.max_, UpdateOctreeSize, defaultBoundsMax, AM_DEFAULT);
		URHO3D_ATTRIBUTE_EX("Number of Levels", int, numLevels_, UpdateOctreeSize, DEFAULT_OCTREE_LEVELS, AM_DEFAULT);
	}

	void Octree::SetSize(const BoundingBox &box, unsigned numLevels)
	{
		// If drawables exits, they are temporarily moved to the root
		//todo check
		for(unsigned i=0; i < NUM_OCTANTS; ++i)
		{
			DeleteChild(i);
		}

		Initialize(box);
		numDrawables_ = drawables_.Size();
		numLevels_ = Max(numLevels, 1U);
	}

	void Octree::Update(const FrameInfo &frame)
	{
		if(!Thread::IsMainThread())
		{
			URHO3D_LOGERROR("Octree::Update() can not be called from worker threads");
			return;
		}

		if(!drawableUpdates_.Empty())
		{
			Scene* scene = GetScene();
			WorkQueue* queue = GetSubsystem<WorkQueue>();
			scene->BeginThreadedUpdate();

			int numWorkItems = queue->GetNumThreads() + 1; // Worker thread + main thread
			// Note, floor
			int drawablesPerThread = Max((int)(drawableUpdates_.Size()/ numWorkItems), 1);

			auto&& start = drawableUpdates_.Begin();
			// Create a workItem for each thread
			for(int i=0; i< numWorkItems; ++i)
			{
				SharedPtr<WorkItem> item = queue->GetFreeItem();
				item->priority_ = M_MAX_UNSIGNED;
				item->workFunction_ = UpdateDrawablesWork;
				item->aux_ = const_cast<FrameInfo*>(&frame);

				auto&& end = drawableUpdates_.End();
				if(i < numWorkItems -1 && end - start > drawablesPerThread)
					end = start + drawablesPerThread;

				item->start_ = &(*start);
				item->end_ = &(*end);
				queue->AddWorkItem(item);

				start = end;
			}

			queue->Complete(M_MAX_UNSIGNED);
			scene->EndThreadedUpdate();
		}

		if(!threadedDrawableUpdates_.Empty())
		{
			for(auto&& it = threadedDrawableUpdates_.Begin(); it != threadedDrawableUpdates_.End(); ++it)
			{
				Drawable* drawable = *it;
				if(drawable)
				{
					drawable->Update(frame);
					drawableUpdates_.Push(drawable);
				}
			}

			threadedDrawableUpdates_.Clear();
		}

		Scene* scene = GetScene();
		if(scene)
		{
			using namespace SceneDrawableUpdateFinished;

			VariantMap& eventData = GetEventDataMap();
			eventData[P_SCENE] = scene;
			eventData[P_TIMESTEP] = frame.timeStep_;
			scene->SendEvent(E_SCENEDRAWABLEUPDATEFINISHED, eventData);
		}

		// Reinsert drawable that have been moved or resized, or that have been newly added to the octree and do not
		// sit inside the proper octant yet
		if(!drawableUpdates_.Empty())
		{
			for(auto it = drawableUpdates_.Begin(); it != drawableUpdates_.End(); ++it)
			{
				Drawable* drawable = *it;
				drawable->updateQueued_ = false;
				Octant* octant = drawable->GetOctant();
				const BoundingBox& box = drawable->GetWorldBoundingBox();

				// Skip if no octant or does not belong to this octree anymore
				if(!octant || octant->GetRoot() != this)
					continue;

				// Skip if still fits the current octant
				// todo, ref https://docs.unity3d.com/Manual/OcclusionCulling.html
				if(drawable->IsOccludee() && octant->GetCullingBox().IsInside(box) == INSIDE && octant->CheckDrawableFit(box))
					continue;

				InsertDrawable(drawable);

#ifdef _DEBUG
				octant = drawable->GetOctant();
				if(octant != this && octant->GetCullingBox().IsInside(box) != INSIDE)
				{
					URHO3D_LOGERROR("Drawable is not fully inside its octant's culling bounds: drawable box " + box.ToString()
					                + " octant box " + octant->GetCullingBox().ToString());
				}
#endif
			}
		}

		drawableUpdates_.Clear();
	}

	void Octree::AddManualDrawable(Drawable *drawable)
	{
		if(!drawable || drawable->GetOctant())
			return;
		AddDrawable(drawable);
	}

	void Octree::RemoveManualDrawable(Drawable *drawable)
	{
		if(!drawable)
			return;
		Octant* octant = drawable->GetOctant();
		if(octant && octant->GetRoot() == this)
			octant->RemoveDrawable(drawable);
	}

	void Octree::GetDrawables(OctreeQuery &query) const
	{
		query.result_.Clear();
		GetDrawablesInternal(query, false);
	}

	void Octree::Raycast(RayOctreeQuery &query) const
	{
		query.result_.Clear();
		GetDrawablesInternal(query);
		Sort(query.result_.Begin(), query.result_.End(), CompareRayQueryResult);
	}

	void Octree::RaycastSingle(RayOctreeQuery &query) const
	{
		query.result_.Clear();
		rayQueryDrawables_.Clear();

		GetDrawablesOnlyInternal(query, rayQueryDrawables_);

		for(auto&& it = rayQueryDrawables_.Begin(); it != rayQueryDrawables_.End(); ++it)
		{
			Drawable* drawable = *it;
			drawable->SetSortValue(query.ray_.HitDistance(drawable->GetWorldBoundingBox()));
		}

		Sort(rayQueryDrawables_.Begin(), rayQueryDrawables_.End(), CompareDrawables);

		// Then do the actual test according to the query, and early-out as possible
		float closestHit = M_INFINITY;
		for(auto&& it = rayQueryDrawables_.Begin(); it != rayQueryDrawables_.End(); ++it)
		{
			Drawable* drawable = *it;
			if(drawable->GetSortValue() < Min(closestHit, query.maxDistance_))
			{
				unsigned oldSize = query.result_.Size();
				drawable->ProcessRayQuery(query, query.result_);
				if(query.result_.Size() > oldSize)
					closestHit = Min(closestHit, query.result_.Back().distance_);
			}
			else
				break;
		}

		if(query.result_.Size() > 1)
		{
			Sort(query.result_.Begin(), query.result_.End(), CompareRayQueryResult);
			query.result_.Resize(1);
		}
	}

	void Octree::QueueUpdate(Drawable *drawable)
	{
		Scene* scene = GetScene();
		if(scene && scene->IsThreadedUpdate())
		{
			MutexLock lock(octreeMutex_);
			threadedDrawableUpdates_.Push(drawable);
		}
		else
			drawableUpdates_.Push(drawable);

		drawable->updateQueued_ = true;
	}

	void Octree::CancelUpdate(Drawable *drawable)
	{
		// This doesn't have to take into account scene being in threaded update, because it is called only when removing
		// a drawable from octree, which should only ever happen from the main thread
		// todo , check
		drawableUpdates_.Remove(drawable);
		drawable->updateQueued_ = false;
	}

	void Octree::DrawDebugGeometry(bool depthTest)
	{

	}

	// When running in headless mode, update the octree manually during the RenderUpdate event
	void Octree::HandleRenderUpdate(StringHash eventType, VariantMap &eventData)
	{
		Scene* scene = GetScene();
		if(!scene || !scene->IsUpdateEnabled())
			return;

		using namespace RenderUpdate;

		FrameInfo frame;
		frame.frameNumber_ = GetSubsystem<Time>()->GetFrameNumber();
		frame.timeStep_ = eventData[P_TIMESTEP].GetFloat();
		frame.camera_ = nullptr;

		Update(frame);
	}
}