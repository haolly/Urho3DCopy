//
// Created by liuhao1 on 2017/12/22.
//

#include "Model.h"
#include "../Core/Context.h"
#include "../IO/Deserializer.h"
#include "../IO/Log.h"

namespace Urho3D
{

	unsigned LookupVertexBuffer(VertexBuffer* buffer, const Vector<SharedPtr<VertexBuffer> >& buffers)
	{
		for(unsigned i=0; i< buffers.Size(); ++i)
		{
			if(buffer == buffers[i])
				return i;
		}
		return 0;
	}

	unsigned LookupIndexBuffers(IndexBuffer* buffer, const Vector<SharedPtr<IndexBuffer> >& buffers)
	{
		for(unsigned i=0; i<buffers.Size(); ++i)
		{
			if(buffer == buffers[i])
				return i;
		}
		return 0;
	}

	Model::Model(Context *context) :
			ResourceWithMetadata(context)
	{

	}

	Model::~Model()
	{

	}

	void Model::RegisterObject(Context *context)
	{
		context->RegisterFactory<Model>();
	}

	bool Model::BeginLoad(Deserializer &source)
	{
		String fileID = source.ReadFileID();
		if(fileID != "UMDL" && fileID != "UMD2")
		{
			URHO3D_LOGERROR(source.GetName() + " is not a valid model file");
			return false;
		}
		bool hasVertexDeclarations = (fileID == "UMD2");

		geometries_.Clear();
		geometryBoneMappings_.Clear();
		geometryCenters_.Clear();
		morphs_.Clear();
		vertexBuffers_.Clear();
		indexBuffers_.Clear();

		unsigned memoryUse = sizeof(Model);
		bool async = GetAsyncLoadState() == ASYNC_LOADING;

		// Read vertex buffer
		unsigned numVertexBuffers = source.ReadUInt();
		vertexBuffers_.Reserve(numVertexBuffers);
		morphRangeStarts_.Reserve(numVertexBuffers);
		morphRangeCounts_.Reserve(numVertexBuffers);
		loadVBData_.Resize(numVertexBuffers);
		for(unsigned i=0; i< numVertexBuffers; ++i)
		{
			VertexBufferDesc& desc = loadVBData_[i];

			desc.vertexCount_ = source.ReadUInt();
			if(!hasVertexDeclarations)
			{
				unsigned elementMask = source.ReadUInt();
				desc.vertexElements_ = VertexBuffer::GetElements(elementMask);
			}
			else
			{
				desc.vertexElements_.Clear();
				unsigned numElements = source.ReadUInt();
				for(unsigned j=0; j< numElements; ++j)
				{
					unsigned elementDesc = source.ReadUInt();
					VertexElementType type = (VertexElementType)(elementDesc & 0xff);
					VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
					unsigned char index = (unsigned char)((elementDesc >> 16) & 0xff);
					desc.vertexElements_.Push(VertexElement(type, semantic, index));
				}
			}

			morphRangeStarts_[i] = source.ReadUInt();
			morphRangeCounts_[i] = source.ReadUInt();

			SharedPtr<VertexBuffer> buffer(new VertexBuffer(context_));
			unsigned vertexSize = VertexBuffer::GetVertexSize(desc.vertexElements_);
			desc.dataSize_ = desc.vertexCount_ * vertexSize;

			// Prepare vertex buffer data to be uploaded during Endload()
			if(async)
			{
				desc.data_ = new unsigned char[desc.dataSize_];
				source.Read(desc.data_.Get(), desc.dataSize_);
			}
			else
			{
				// Make sure no previous data
				desc.data_.Reset();
				buffer->SetShadowed(true);
				buffer->SetSize(desc.vertexCount_, desc.vertexElements_);
				void* dest = buffer->Lock(0, desc.vertexCount_);
				source.Read(dest, desc.vertexCount_ * vertexSize);
				buffer->Unlock();
			}

			//Todo, why do add additional sizeof(VertexBuffer)
			memoryUse += sizeof(VertexBuffer) + desc.vertexCount_ * vertexSize;
			vertexBuffers_.Push(buffer);
		}

		// Read index buffers
		unsigned numIndexBuffers = source.ReadUInt();
		indexBuffers_.Reserve(numIndexBuffers);
		loadIBData_.Resize(numIndexBuffers);
		for(unsigned i=0; i< numIndexBuffers; ++i)
		{
			unsigned indexCount = source.ReadUInt();
			unsigned indexSize = source.ReadUInt();

			SharedPtr<IndexBuffer> buffer(new IndexBuffer(context_));

			if(async)
			{
				loadIBData_[i].indexCount_ = indexCount;
				loadIBData_[i].indexSize_ = indexSize;
				loadIBData_[i].dataSize_ = indexCount * indexSize;
				loadIBData_[i].data_ = new unsigned char[indexCount * indexSize];
				source.Read(loadIBData_[i].data_.Get(), loadIBData_[i].dataSize_);
			}
			else
			{
				loadIBData_[i].data_.Reset();
				buffer->SetShadowed(true);
				buffer->SetSize(indexCount, indexSize > sizeof(unsigned short));
				void* dest = buffer->Lock(0, indexCount);
				source.Read(dest, indexCount * indexSize);
				buffer->Unlock();
			}

			memoryUse += sizeof(IndexBuffer) + indexCount * indexSize;
			indexBuffers_.Push(buffer);
		}

		//Read geometries
		unsigned numGeometries = source.ReadUInt();
		geometries_.Reserve(numGeometries);
		geometryBoneMappings_.Reserve(numGeometries);
		geometryCenters_.Reserve(numGeometries);
		loadGeometries_.Resize(numGeometries);
		for(unsigned i=0; i< numGeometries; ++i)
		{
			unsigned boneMappingCount = source.ReadUInt();
			PODVector<unsigned> boneMapping(boneMappingCount);
			for(unsigned j=0; j<boneMappingCount; ++j)
				boneMapping[j] = source.ReadUInt();
			geometryBoneMappings_.Push(boneMapping);

			unsigned numLodLevels = source.ReadUInt();
			Vector<SharedPtr<Geometry> > geometryLodLevels;
			geometryLodLevels.Reserve(numLodLevels);
			loadGeometries_[i].Resize(numLodLevels);

			for(unsigned j=0; j<numLodLevels; ++j)
			{
				float distance = source.ReadFloat();
				PrimitiveType type = (PrimitiveType)source.ReadUInt();

				unsigned vbRef = source.ReadUInt();
				unsigned ibRef = source.ReadUInt();
				unsigned indexStart = source.ReadUInt();
				unsigned indexCount = source.ReadUInt();

				if(vbRef >= vertexBuffers_.Size())
				{
					URHO3D_LOGERROR("Vertex buffer index out of bounds");
					loadVBData_.Clear();
					loadIBData_.Clear();
					loadGeometries_.Clear();
					return false;
				}
				if(ibRef >= indexBuffers_.Size())
				{
					URHO3D_LOGERROR("Index buffer index out of bounds");
					loadVBData_.Clear();
					loadIBData_.Clear();
					loadGeometries_.Clear();
					return false;
				}

				SharedPtr<Geometry> geometry(new Geometry(context_));
				geometry->SetLodDistance(distance);

				// Prepare geometry to be defined during EndLoad()
				//todo , why do not check async ?? all data will be load asynchronously ??
				loadGeometries_[i][j].type_ = type;
				loadGeometries_[i][j].vbRef_ = vbRef;
				loadGeometries_[i][j].ibRef_ = ibRef;
				loadGeometries_[i][j].indexCount_ = indexCount;
				loadGeometries_[i][j].indexStart_ = indexStart;

				geometryLodLevels.Push(geometry);
				memoryUse += sizeof(Geometry);
			}

			geometries_.Push(geometryLodLevels);
		}

		// Read morphs
		unsigned numMorphs = source.ReadUInt();
		morphs_.Reserve(numMorphs);
		for(unsigned i=0; i< numMorphs; ++i)
		{
			ModelMorph newMorph;
			newMorph.name_ = source.ReadString();
			newMorph.nameHash_ = newMorph.name_;
			newMorph.weight_ = 0.0f;
			unsigned numBuffers = source.ReadUInt();

			for(unsigned j=0; j<numBuffers; ++j)
			{
				VertexBufferMorph newBuffer;
				unsigned bufferIndex = source.ReadUInt();

				newBuffer.elementMask_ = source.ReadUInt();
				newBuffer.vertexCount_ = source.ReadUInt();

				unsigned vertexSize = sizeof(unsigned);
				if(newBuffer.elementMask_ & MASK_POSITION)
					vertexSize += sizeof(Vector3);
				if(newBuffer.elementMask_ & MASK_NORMAL)
					vertexSize += sizeof(Vector3);
				if(newBuffer.elementMask_ & MASK_TANGENT)
					vertexSize += sizeof(Vector3);
				newBuffer.dataSize_ = newBuffer.vertexCount_ * vertexSize;
				newBuffer.morphData_ = new unsigned char[newBuffer.dataSize_];

				source.Read(&newBuffer.morphData_[0], newBuffer.vertexCount_ * vertexSize);

				newMorph.buffers_[bufferIndex] = newBuffer;
				//Todo, why add additional sizeof(VertexBufferMorph)
				memoryUse += sizeof(VertexBufferMorph) + newBuffer.vertexCount_ * vertexSize;
			}

			morphs_.Push(newMorph);
			memoryUse += sizeof(ModelMorph);
		}

		// Read skeleton , todo

		skeleton_.Load(source);
		memoryUse += skeleton_.GetNumBones() * sizeof(Bone);
	}

	bool Model::EndLoad()
	{
		for(unsigned i=0; i<vertexBuffers_.Size(); ++i)
		{
			VertexBuffer* buffer = vertexBuffers_[i];
			VertexBufferDesc& desc = loadVBData_[i];
			if(desc.data_)
			{
				buffer->SetShadowed(true);
				buffer->SetSize(desc.vertexCount_, desc.vertexElements_);
				buffer->SetData(desc.data_.Get());
			}
		}

		for(unsigned i=0; i<indexBuffers_.Size(); ++i)
		{
			IndexBuffer* buffer = indexBuffers_[i];
			IndexBufferDesc& desc = loadIBData_[i];
			if(desc.data_)
			{
				buffer->SetShadowed(true);
				buffer->SetSize(desc.indexCount_, desc.indexSize_ > sizeof(unsigned short));
				buffer->SetData(desc.data_.Get());
			}
		}

		for(unsigned i=0; i< geometries_.Size(); ++i)
		{
			for(unsigned j=0; j< geometries_[i].Size(); ++j)
			{
				Geometry* geometry = geometries_[i][j];
				GeometryDesc& desc = loadGeometries_[i][j];
				geometry->SetVertexBuffer(0, vertexBuffers_[desc.vbRef_]);
				geometry->SetIndexBuffer(indexBuffers_[desc.ibRef_]);
				geometry->SetDrawRange(desc.type_, desc.indexStart_, desc.indexCount_);
			}
		}

		loadVBData_.Clear();
		loadIBData_.Clear();
		loadGeometries_.Clear();
		return true;
	}

	bool Model::Save(Serializer &dest) const
	{
		if(!dest.WriteFileID("UMD2"))
			return false;
		// Write vretex buffers
		dest.WriteUInt(vertexBuffers_.Size());
		for(unsigned i=0; i<vertexBuffers_.Size(); ++i)
		{
			VertexBuffer* buffer = vertexBuffers_[i];
			dest.WriteUInt(buffer->GetVertexCount());
			const PODVector<VertexElement>& elements = buffer->GetElements();
			dest.WriteUInt(elements.Size());
			for(unsigned k = 0; k<elements.Size(); ++k)
			{
				unsigned elementDesc = ((unsigned)elements[k].type_) | ((unsigned)elements[k].semantic_) << 8 |
						((unsigned)elements[k].index_) << 16;
				dest.WriteUInt(elementDesc);
			}
			dest.WriteUInt(morphRangeStarts_[i]);
			dest.WriteUInt(morphRangeCounts_[i]);
			dest.Write(buffer->GetShadowData(), buffer->GetVertexCount() * buffer->GetVertexSize());
		}

		// Write index buffers
		dest.WriteUInt(indexBuffers_.Size());
		for(unsigned i=0; i<indexBuffers_.Size(); ++i)
		{
			IndexBuffer* buffer = indexBuffers_[i];
			dest.WriteUInt(buffer->GetIndexCount());
			dest.WriteUInt(buffer->GetIndexSize());
			dest.Write(buffer->GetShadowData(), buffer->GetIndexCount() * buffer->GetIndexSize());
		}
		// Write geometries
		dest.WriteUInt(geometries_.Size());
		for(unsigned i=0; i<geometries_.Size(); ++i)
		{
			//Write bone mapping
			dest.WriteUInt(geometryBoneMappings_[i].Size());
			for(unsigned k=0; k<geometryBoneMappings_.Size(); ++k)
				dest.WriteUInt(geometryBoneMappings_[i][k]);


			//Write the LOD levels
			dest.WriteUInt(geometries_[i].Size());
			for(unsigned k=0; k<geometries_[i].Size(); ++i)
			{
				Geometry* geometry = geometries_[i][k];
				dest.WriteFloat(geometry->GetLodDistance());
				dest.WriteUInt(geometry->GetPrimitiveType());
				dest.WriteUInt(LookupVertexBuffer(geometry->GetVertexBuffer(0), vertexBuffers_));
				dest.WriteUInt(LookupIndexBuffers(geometry->GetIndexBuffer(), indexBuffers_));
				dest.WriteUInt(geometry->GetIndexStart());
				dest.WriteUInt(geometry->GetIndexCount());
			}
		}

		// Write morphs, todo
	}

	void Model::SetBoundingBox(const BoundingBox &box)
	{
		boundingBox_ = box;
	}

	bool
	Model::SetVertexBuffers(const Vector<SharedPtr<VertexBuffer>> &buffers, const PODVector<unsigned> &morphRangeStarts,
	                        const PODVector<unsigned> &morphRangeCounts)
	{
		for(unsigned i=0; i<buffers.Size(); ++i)
		{
			if(!buffers[i])
			{
				URHO3D_LOGERROR("Null model vertex buffers specified");
				return false;
			}
			if(!buffers[i]->IsShadowed())
			{
				URHO3D_LOGERROR("Model vertex buffers must be shadowed");
				return false;
			}
		}

		vertexBuffers_ = buffers;
		morphRangeStarts_.Resize(buffers.Size());
		morphRangeCounts_.Resize(buffers.Size());

		for(unsigned i=0; i<buffers.Size(); ++i)
		{
			morphRangeStarts_[i] = i < morphRangeStarts.Size() ? morphRangeStarts[i] : 0;
			morphRangeCounts_[i] = i < morphRangeCounts.Size() ? morphRangeCounts[i] : 0;
		}

		return true;
	}

	bool Model::SetIndexBuffers(const Vector<SharedPtr<IndexBuffer>> &buffers)
	{
		for(unsigned i=0; i<buffers.Size(); ++i)
		{
			if(!buffers[i])
			{
				URHO3D_LOGERROR("Null model index buffers specified");
				return false;
			}
			if(!buffers[i]->IsShadowed())
			{
				URHO3D_LOGERROR("Model index buffers must be shadowed");
				return false;
			}
		}
		indexBuffers_ = buffers;
		return true;
	}

	void Model::SetNumGeometries(unsigned num)
	{
		geometries_.Resize(num);
		geometryBoneMappings_.Resize(num);
		geometryCenters_.Resize(num);

		// For easier creation of from-scratch geometry, ensure that all geometries start with at least 1 LOD level(0 makes no sense)
		for(unsigned i=0; i<geometries_.Size(); ++i)
		{
			if(geometries_[i].Empty())
				geometries_[i].Resize(1);
		}
	}

	bool Model::SetNumGeometryLodLevels(unsigned index, unsigned num)
	{
		if(index >= geometries_.Size())
		{
			URHO3D_LOGERROR("Geometry index out of bounds");
			return false;
		}
		if(!num)
		{
			URHO3D_LOGERROR("Zero LOD levels not allowed");
			return false;
		}
		geometries_[index].Resize(num);
		return true;
	}

	bool Model::SetGeometry(unsigned index, unsigned lodLevel, Geometry *geometry)
	{
		if(index >= geometries_.Size())
		{
			URHO3D_LOGERROR("Geometry index out of bounds");
			return false;
		}
		if(lodLevel >= geometries_[index].Size())
		{
			URHO3D_LOGERROR("LOD level index out of bounds");
			return false;
		}
		//Todo, a lod level is a Geometry??
		geometries_[index][lodLevel] = geometry;
		return true;
	}

	bool Model::SetGeometryCenter(unsigned index, const Vector &center)
	{
		if(index >= geometries_.Size())
		{
			URHO3D_LOGERROR("Geometry index out of bounds");
			return false;
		}
		geometries_[index] = center;
		return true;
	}

	void Model::SetSkeleton(const Skeleton &skeleton)
	{
		//todo
	}

	void Model::SetGeometryBoneMappings(const Vector<PODVector<unsigned int>> &mappings)
	{
		geometryBoneMappings_ = mappings;
	}

	void Model::SetMorphs(const Vector<ModelMorph> &morphs)
	{
		morphs_ = morphs;
	}

	SharedPtr<Model> Model::Clone(const String &cloneName) const
	{
		SharedPtr<Model> ret(new Model(context_));

		ret->SetName(cloneName);
		ret->boundingBox_ = boundingBox_;
		ret->skeleton_ = skeleton_;
		ret->geometryBoneMappings_ = geometryBoneMappings_;
		ret->geometryCenters_ = geometryCenters_;
		ret->morphs_ = morphs_;
		ret->morphRangeStarts_ = morphRangeStarts_;
		ret->morphRangeCounts_ = morphRangeCounts_;

		//Deep copy
		HashMap<VertexBuffer*, VertexBuffer*> vbMapping;
		for(auto it = vertexBuffers_.Begin(); it != vertexBuffers_.End(); ++it)
		{
			VertexBuffer* origBuffer = *it;
			SharedPtr<VertexBuffer> cloneBuffer;
			if(origBuffer)
			{
				cloneBuffer = new VertexBuffer(context_);
				cloneBuffer->SetSize(origBuffer->GetVertexSize(), origBuffer->GetElementMask(), origBuffer->IsDynamic());
				cloneBuffer->SetShadowed(origBuffer->IsShadowed());
				if(origBuffer->IsShadowed())
					cloneBuffer->SetData(origBuffer->GetShadowData());
				else
				{
					void* origData = origBuffer->Lock(0, origBuffer->GetVertexSize());
					if(origData)
					{
						cloneBuffer->SetData(origData);
					}
					else
						URHO3D_LOGERROR("Failed to lock original vertex buffer for copying");
				}
				vbMapping[origBuffer] = cloneBuffer;
			}
			ret->vertexBuffers_.Push(cloneBuffer);
		}
		//todo
	}

	unsigned Model::GetNumGeometryLodLevels(unsigned index) const
	{
		return index < geometries_.Size() ? geometries_[index].Size() : 0;
	}

	Geometry *Model::GetGeometry(unsigned index, unsigned lodLevel) const
	{
		if(index >= geometries_.Size() || geometries_[index].Empty())
			return nullptr;
		if(lodLevel >= geometries_[index].Size())
			lodLevel = geometries_[index].Size() - 1;
		return geometries_[index][lodLevel];
	}

	const ModelMorph *Model::GetMorph(unsigned index) const
	{
		return index < morphs_.Size() ? &morphs_[index] : nullptr;
	}

	const ModelMorph *Model::GetMorph(const String &name) const
	{
		return GetMorph(StringHash(name));
	}

	const ModelMorph *Model::GetMorph(StringHash nameHash) const
	{
		for(auto it = morphs_.Begin(); it != morphs_.End(); ++it)
		{
			if(it->nameHash_ == nameHash)
				return &(*it);
		}
		return nullptr;
	}

	unsigned Model::GetMorphRangeStart(unsigned bufferIndex) const
	{
		return bufferIndex < vertexBuffers_.Size() ? morphRangeCounts_[bufferIndex] : 0;
	}

	unsigned Model::GetMorphRangeCount(unsigned bufferIndex) const
	{
		return bufferIndex < vertexBuffers_.Size() ? morphRangeCounts_[bufferIndex] : 0;
	}
}