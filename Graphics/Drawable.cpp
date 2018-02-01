//
// Created by liuhao1 on 2018/1/27.
//

#include "Drawable.h"
#include "../Graphics/Octree.h"

namespace Urho3D
{
	SourceBatch::SourceBatch() :
		distance_(0.0f),
		geometry_(nullptr),
		worldTransform_(&Matrix3x4::IDENTITY),
		numWorldTransforms_(1),
		instancingData_(nullptr),
		geometryType_(GEOM_STATIC)
	{
	}

	SourceBatch::SourceBatch(const SourceBatch &batch)
	{
		*this = batch;
	}

	SourceBatch::~SourceBatch()
	{
	}

	SourceBatch &SourceBatch::operator=(const SourceBatch &rhs)
	{
		distance_ = rhs.distance_;
		geometry_ = rhs.geometry_;
		material_ = rhs.material_;
		worldTransform_ = rhs.worldTransform_;
		numWorldTransforms_ = rhs.numWorldTransforms_;
		instancingData_ = rhs.instancingData_;
		geometryType_ = rhs.geometryType_;
		return *this;
	}

	Drawable::Drawable(Context *context, unsigned char drawableFlags) :
		Component(context),
		boundingBox_(0.0f, 0.0f),
		drawableFlags_(drawableFlags),
		worldBoundingboxDirty_(true),
		castShadows_(false),
		occluder_(false),
		occludee_(true),
		updateQueued_(false),
		zoneDirty_(false),
		octant_(nullptr),
		zone_(nullptr),
		viewMask_(DEFAULT_VIEWMASK),
		lightMask_(DEFAULT_LIGHTMASK),
		shadowMask_(DEFAULT_SHADOWMASK),
		zoneMask_(DEFAULT_ZONEMASK),
		viewFrameNumber_(0),
		distance_(0.0f),
		lodDistance_(0.0f),
		drawDistance_(0.0f),
		shadowDistance_(0.0f),
		sortValue_(0.0f),
		minZ_(0.0f),
		maxZ_(0.0f),
		lodBias_(1.0f),
		maxLights_(0),
		firstLight_(nullptr)
	{
		if(drawableFlags == DRAWABLE_UNDEFINED || drawableFlags > DRAWABLE_ANY)
		{
			URHO3D_LOGERROR("Drawable with undefined drawableFlags");
		}
	}

	Drawable::~Drawable()
	{
		RemoveFromOctree();
	}

	void Drawable::RegisterObject(Context *context)
	{
		URHO3D_ATTRIBUTE("Max Lights", int, maxLights_, 0, AM_DEFAULT);
		URHO3D_ATTRIBUTE("View Mask", int, viewMask_, DEFAULT_VIEWMASK, AM_DEFAULT);
		URHO3D_ATTRIBUTE("Light Mask", int, lightMask_, DEFAULT_LIGHTMASK, AM_DEFAULT);
		URHO3D_ATTRIBUTE("Shadow Mask", int, shadowMask_, DEFAULT_SHADOWMASK, AM_DEFAULT);
		URHO3D_ACCESSOR_ATTRIBUTE("Zone Mask", GetZoneMask, SetZoneMask, unsigned, DEFAULT_ZONEMASK, AM_DEFAULT);
	}

	void Drawable::OnSetEnabled()
	{
		//todo
	}

	void Drawable::ProcessRayQuery(const RayOctreeQuery &query, PODVector<RayQueryResult> &results)
	{

	}

	void Drawable::UpdateBatches(const FrameInfo &frame)
	{

	}

	Geometry *Drawable::GetLodGeometry(unsigned batchIndex, unsigned level)
	{
		return nullptr;
	}

	bool Drawable::DrawOcclusion(OcclusionBuffer *buffer)
	{
		return false;
	}

	void Drawable::DrawDebugGeometry(DebugRenderer *debug, bool depthTest)
	{
		Component::DrawDebugGeometry(debug, depthTest);
	}

	void Drawable::SetDrawDistance(float distance)
	{

	}

	void Drawable::SetShadowDistance(float distance)
	{

	}

	void Drawable::SetLodBias(float bias)
	{

	}

	void Drawable::SetViewMask(unsigned mask)
	{

	}

	void Drawable::SetLightMask(unsigned mask)
	{

	}

	void Drawable::SetShadowMask(unsigned mask)
	{

	}

	void Drawable::SetZoneMask(unsigned mask)
	{

	}

	void Drawable::SetMaxLights(unsigned num)
	{

	}

	void Drawable::SetCastShadows(bool enable)
	{

	}

	void Drawable::SetOccluder(bool enable)
	{

	}

	void Drawable::SetOccludee(bool enable)
	{

	}

	void Drawable::MarkForUpdate()
	{

	}

	const BoundingBox &Drawable::GetWorldBoundingBox()
	{
		return <#initializer#>;
	}

	bool Drawable::IsInView() const
	{
		return false;
	}

	bool Drawable::IsInView(Camera *camera) const
	{
		return false;
	}

	void Drawable::SetZone(Zone *zone, bool temporary)
	{

	}

	void Drawable::SetSortValue(float value)
	{
		sortValue_ = value;
	}

	void Drawable::MarkInView(const FrameInfo &frame)
	{

	}

	void Drawable::MarkInView(unsigned frameNumber)
	{

	}

	void Drawable::LimitLights()
	{

	}

	void Drawable::LimitVertexLights(bool removeConvertedLights)
	{

	}

	bool Drawable::IsInView(const FrameInfo &frame, bool anyCamera) const
	{
		return false;
	}

	void Drawable::OnNodeSet(Node *node)
	{
		Component::OnNodeSet(node);
	}

	void Drawable::OnSceneSet(Scene *scene)
	{
		Component::OnSceneSet(scene);
	}

	void Drawable::OnMarkedDirty(Node *node)
	{
		Component::OnMarkedDirty(node);
	}

	void Drawable::AddToOctree()
	{

	}

	void Drawable::RemoveFromOctree()
	{
		if(octant_)
		{
			Octree* octree = octant_->GetRoot();
			if(updateQueued_)
				octree->CancelUpdate(this);

			OnRemoveFromOctree();
			octant_->RemoveDrawable(this);
		}
	}


}
