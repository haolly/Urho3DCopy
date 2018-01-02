//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_MODEL_H
#define URHO3DCOPY_MODEL_H

#include "../Resource/Resource.h"
#include "../Math/BoundingBox.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Geometry.h"

namespace Urho3D
{
	//Todo, ref https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch03.html
	struct VertexBufferMorph
	{
		unsigned elementMask_;
		unsigned vertexCount_;
		unsigned dataSize_;
		SharedArrayPtr<unsigned char> morphData_;
	};

	struct ModelMorph
	{
		String name_;
		StringHash nameHash_;
		float weight_;
		HashMap<unsigned, VertexBufferMorph> buffers_;
	};

	// Description of vertex buffer data fro asynchronous loading
	struct VertexBufferDesc
	{
		unsigned vertexCount_;
		PODVector<VertexElement> vertexElements_;
		unsigned dataSize_;
		SharedArrayPtr<unsigned char> data_;
	};

	struct IndexBufferDesc
	{
		unsigned indexCount_;
		unsigned indexSize_;
		unsigned dataSize_;
		SharedArrayPtr<unsigned char> data_;
	};

	struct GeometryDesc
	{
		PrimitiveType type_;
		unsigned vbRef_;
		unsigned ibRef_;
		unsigned indexStart_;
		unsigned indexCount_;
	};

	class Model : public ResourceWithMetadata
	{
		URHO3D_OBJECT(Model, ResourceWithMetadata);
	public:
		Model(Context* context);
		virtual ~Model() override ;

		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;
		virtual bool EndLoad() override ;
		virtual bool Save(Serializer& dest) const override ;

		void SetBoundingBox(const BoundingBox& box);

		bool SetVertexBuffers(const Vector<SharedPtr<VertexBuffer> >& buffers, const PODVector<unsigned>&
							morphRangeStarts, const PODVector<unsigned>& morphRangeCounts);

		bool SetIndexBuffers(const Vector<SharedPtr<IndexBuffer> >& buffers);

		void SetNumGeometries(unsigned num);
		bool SetNumGeometryLodLevels(unsigned index, unsigned num);
		bool SetGeometry(unsigned index, unsigned lodLevel, Geometry* geometry);
		bool SetGeometryCenter(unsigned index, const Vector& center);

		void SetSkeleton(const Skeleton& skeleton);

		void SetGeometryBoneMappings(const Vector<PODVector<unsigned> >& mappings);
		void SetMorphs(const Vector<ModelMorph>& morphs);
		SharedPtr<Model> Clone(const String& cloneName = String::EMPTY) const;

		const BoundingBox& GetBoundingBox() const
		{
			return boundingBox_;
		}

		//todo
		const Vector<SharedPtr<VertexBuffer> >& GetVertexBuffers() const
		{
			return vertexBuffers_;
		}

		const Vector<SharedPtr<IndexBuffer> >& GetIndexBuffers() const
		{
			return indexBuffers_;
		}

		unsigned GetNumGeometries() const { return geometries_.Size(); }

		unsigned GetNumGeometryLodLevels(unsigned index) const;

		const Vector<Vector<SharedPtr<Geometry> > >& GetGeometries() const { return geometries_; }

		const PODVector<Vector3>& GetGeometryCenters() const { return geometryCenters_; }

		Geometry* GetGeometry(unsigned index, unsigned lodLevel) const;

		const Vector3& GetGeometryCenter(unsigned index) const
		{
			return index < geometryCenters_.Size() ? geometryCenters_[index] : Vector3::ZERO;
		}

		const Vector<PODVector<unsigned> >& GetGeometryBoneMappings() const
		{
			return geometryBoneMappings_;
		}

		const Vector<ModelMorph>& GetMorphs() const
		{
			return morphs_;
		}

		unsigned GetNumMorphs() const { return morphs_.Size(); }

		const ModelMorph* GetMorph(unsigned index) const;
		const ModelMorph* GetMorph(const String& name) const;
		const ModelMorph* GetMorph(StringHash nameHash) const;

		unsigned GetMorphRangeStart(unsigned bufferIndex) const;
		unsigned GetMorphRangeCount(unsigned bufferIndex) const;

	private:
		BoundingBox boundingBox_;
		Skeleton skeleton_;

		//todo, why there will be many vertexbuffer and indexbuffer geometry ??
		Vector<SharedPtr<VertexBuffer> > vertexBuffers_;
		Vector<SharedPtr<IndexBuffer> > indexBuffers_;
		Vector<Vector<SharedPtr<Geometry> > > geometries_;

		Vector<PODVector<unsigned> > geometryBoneMappings_;
		PODVector<Vector3> geometryCenters_;
		Vector<ModelMorph> morphs_;
		// Vertex buffer morph range start
		// todo, how to use this ??
		// todo, the same size with vertexBuffers?? ref SetVertexBuffer
		PODVector<unsigned> morphRangeStarts_;
		PODVector<unsigned> morphRangeCounts_;

		Vector<VertexBufferDesc> loadVBData_;
		Vector<IndexBufferDesc> loadIBData_;
		Vector<PODVector<GeometryDesc> > loadGeometries_;
	};
}



#endif //URHO3DCOPY_MODEL_H
