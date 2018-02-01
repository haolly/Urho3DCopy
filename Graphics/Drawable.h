//
// Created by liuhao1 on 2018/1/27.
//

#ifndef URHO3DCOPY_DRAWABLE_H
#define URHO3DCOPY_DRAWABLE_H

#include "../Math/Vector2.h"
#include "../Container/Ptr.h"
#include "../Math/Matrix3x4.h"
#include "GraphicsDefs.h"
#include "../Scene/Component.h"

namespace Urho3D
{
	static const unsigned DRAWABLE_UNDEFINED = 0x0;
	static const unsigned DRAWABLE_GEOMETRY = 0x1;
	static const unsigned DRAWABLE_LIGHT = 0x2;
	static const unsigned DRAWABLE_ZONE = 0x4;
	static const unsigned DRAWABLE_GEOMETRY2D = 0x8;
	static const unsigned DRAWABLE_ANY = 0xff;

	static const unsigned DEFAULT_VIEWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_LIGHTMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_SHADOWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_ZONEMASK = M_MAX_UNSIGNED;
	static const int MAX_VERTEX_LIGHTS = 4;
	static const float ANIMATION_LOD_BASESCALE = 2500.0f;


	class Camera;
	class File;
	class Geometry;
	class Light;
	class Material;
	class Octant;
	class RayOctreeQuery;
	class Zone;
	struct RayQueryResult;
	struct WorkItem;

	//todo, usage
	enum UpdateGeometryType
	{
		UPDATE_NONE = 0,
		UPDATE_MAIN_THREAD,
		UPDATE_WORKER_THREAD
	};

	struct FrameInfo
	{
		unsigned frameNumber_;
		float timeStep_;
		IntVector2 viewSize_;
		Camera* camera_;
	};

	struct SourceBatch
	{
		SourceBatch();
		SourceBatch(const SourceBatch& batch);
		~SourceBatch();

		SourceBatch& operator =(const SourceBatch& rhs);

		float distance_;
		Geometry* geometry_;
		SharedPtr<Material> material_;
		// World transform(s), For a skinned model, these are the bone transforms
		const Matrix3x4* worldTransform_;
		unsigned numWorldTransforms_;
		void* instancingData_;
		GeometryType geometryType_;
	};

	class Drawable : public Component
	{
		URHO3D_OBJECT(Drawable, Component);

		friend class Octant;
		friend class Octree;
		friend void UpdateDrawablesWork(const WorkItem* item, unsigned threadIndex);

	public:
		Drawable(Context* context, unsigned char drawableFlags);
		virtual ~Drawable() override ;

		static void RegisterObject(Context* context);

		virtual void OnSetEnabled() override ;
		virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);
		virtual void Update(const FrameInfo& frame) {}
		virtual void UpdateBatches(const FrameInfo& frame);
		virtual void UpdateGeometry(const FrameInfo& frame) {}

		// Return whether a geometry udpate is necessaryk, and if it can happen in a worker thread
		virtual UpdateGeometryType GetUpdateGeometryType() { return UPDATE_NONE; }

		virtual Geometry* GetLodGeometry(unsigned batchIndex, unsigned level);

		virtual unsigned GetNumOccluderTriangles() { return 0; }
		// Draw to occlusion buffer. Return true if did not run out of triangles
		virtual bool DrawOcclusion(OcclusionBuffer* buffer);
		virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override ;

		void SetDrawDistance(float distance);
		void SetShadowDistance(float distance);
		//todo, what is lod bias ?
		void SetLodBias(float bias);
		void SetViewMask(unsigned mask);
		void SetLightMask(unsigned mask);
		void SetShadowMask(unsigned mask);
		void SetZoneMask(unsigned mask);
		void SetMaxLights(unsigned num);
		void SetCastShadows(bool enable);
		void SetOccluder(bool enable);
		void SetOccludee(bool enable);
		// Mark for update and octree reinsertion. Update is automatically queued when the drawable's scene node moves or changes scale
		void MarkForUpdate();

		const BoundingBox& GetBoundingBox() const { return boundingBox_; }
		const BoundingBox& GetWorldBoundingBox();

		unsigned char GetDrawableFlags() const { return drawableFlags_; }
		float GetDrawDistance() const  { return drawDistance_; }
		float GetShadowDistance() const { return shadowDistance_;}
		float GetLodBias() const { return lodBias_; }

		unsigned GetViewMask() const { return viewMask_; }
		unsigned GetLightMask() const { return lightMask_; }
		unsigned GetShadowMask() const { return shadowMask_; }
		unsigned GetZoneMask() const { return zoneMask_; }
		unsigned GetMaxLights() const { return maxLights_;}

		bool GetCastShadows() const { return castShadows_; }
		// todo 遮挡其他 ?
		bool IsOccluder() const { return occluder_;}
		// todo 被遮挡 ?
		bool IsOccludee() const { return occludee_; }

		bool IsInView() const;
		bool IsInView(Camera* camera) const;

		const Vector<SourceBatch>& GetBatches() const { batches_; }
		void SetZone(Zone* zone, bool temporary = false);
		void SetSortValue(float value);

		void SetMinMaxZ(float minZ, float maxZ)
		{
			minZ_ = minZ;
			maxZ_ = maxZ;
		}

		// Mark in viewl. Also clear the light list
		void MarkInView(const FrameInfo& frame);
		// Mark in view without specifying a camera. Used for shadow casters
		void MarkInView(unsigned frameNumber);
		// Sort and limit per-pixel lights to maximum allowed. Convert extra lights into vertex lights
		void LimitLights();
		// Sort and limit per-vertex lights to maximum allowed.
		void LimitVertexLights(bool removeConvertedLights);

		void SetBasePass(unsigned batchIndex)
		{
			basePassFlasg_ |= (1 << batchIndex);
		}

		Octant* GetOctant() const { return octant_; }
		Zone* GetZone() const { return zone_; }

		bool IsZoneDirty() const { return zoneDirty_; }

		float GetDistance() const { return distance_; }
		float GetLodDistance() const { return lodDistance_; }
		float GetSortValue() const { return sortValue_; }

		bool IsInView(const FrameInfo& frame, bool anyCamera = false) const;
		//todo

	protected:
		virtual void OnNodeSet(Node* node) override ;
		virtual void OnSceneSet(Scene* scene) override ;
		virtual void OnMarkedDirty(Node* node) override ;
		virtual void OnWorldBoundingBoxUpdate() = 0;

		virtual void OnRemoveFromOctree() {}

		void AddToOctree();
		void RemoveFromOctree();

		void SetOctant(Octant* octant)
		{
			octant_ = octant;
		}

		BoundingBox worldBoundingBox_;
		BoundingBox boundingBox_;
		Vector<SourceBatch> batches_;
		unsigned char drawableFlags_;
		bool worldBoundingboxDirty_;
		bool castShadows_;
		bool occluder_;
		bool occludee_;
		bool updateQueued_;
		bool zoneDirty_;
		Octant* octant_;
		Zone* zone_;
		unsigned viewMask_;
		unsigned lightMask_;
		unsigned shadowMask_;
		unsigned zoneMask_;
		unsigned viewFrameNumber_;
		float distance_;
		float lodDistance_;
		float drawDistance_;
		float shadowDistance_;
		float sortValue_;
		float minZ_;
		float maxZ_;
		float lodBias_;
		unsigned basePassFlasg_;
		// Max per-pixel lights
		unsigned maxLights_;
		PODVector<Camera*> viewCameras_;
		// First per-pixel light added this frame
		Light* firstLight_;
		// Per-pixel lights affecting this drawable
		PODVector<Light*> lights_;
		PODVector<Light*> vertexLights_;
	};

	inline bool CompareDrawables(Drawable* lhs, Drawable* rhs)
	{
		return lhs->GetSortValue() < rhs->GetSortValue();
	}
}



#endif //URHO3DCOPY_DRAWABLE_H
