//
// Created by liuhao1 on 2017/12/26.
//

#include "Geometry.h"
#include "../IO/Log.h"
#include "../Math/Ray.h"

namespace Urho3D
{

	Geometry::Geometry(Context *context) :
			Object(context),
			primitiveType_(TRIANGLE_LIST),
			indexStart_(0),
			indexCount_(0),
			vertexStart_(0),
			vertexCount_(0),
			rawVertexSize_(0),
			rawIndexSize_(0),
			rawVertexSize_(0),
			lodDistance_(0.0f)
	{
		SetNumVertexBuffers(1);
	}

	Geometry::~Geometry()
	{

	}

	bool Geometry::SetNumVertexBuffers(unsigned num)
	{
		//Todo, what??
		if(num > MAX_VERTEX_STREAMS)
		{
			URHO3D_LOGERROR("Too many vertex streams");
			return false;
		}
		unsigned oldSize = vertexBuffers_.Size();
		vertexBuffers_.Resize(num);
		return true;
	}

	bool Geometry::SetVertexBuffer(unsigned index, VertexBuffer *buffer)
	{
		if(index >= vertexBuffers_.Size())
		{
			URHO3D_LOGERROR("Stream index out of bounds");
			return false;
		}
		vertexBuffers_[index] = buffer;
		return true;
	}

	void Geometry::SetIndexBuffer(IndexBuffer *buffer)
	{
		indexBuffer_ = buffer;
	}

	//todo usage
	bool Geometry::SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, bool getUsedVertexRange)
	{
		if(!indexBuffer_ && !rawIndexData_)
		{
			URHO3D_LOGERROR("Null index buffer and no raw index data, can not define indexed draw range");
			return false;
		}
		if(indexBuffer_ && indexStart + indexCount > indexBuffer_->GetIndexCount())
		{
			URHO3D_LOGERROR("Illegal draw range " + String(indexStart) + " to " + String(indexStart + indexCount -1) +
							", index buffer has " + String(indexBuffer_->GetIndexCount()) + " indices");
			return false;
		}

		primitiveType_ = type;
		indexStart_ = indexStart;
		indexCount_ = indexCount;

		// Get min vertex index and num of vertices from index buffer, if it fails, use full range as fallback
		if(indexCount)
		{
			vertexStart_ = 0;
			//todo
			vertexCount_ = vertexBuffers_[0] ? vertexBuffers_[0]->GetVertexCount() : 0;
			if(getUsedVertexRange && indexBuffer_)
				indexBuffer_->GetUsedVertexRange(indexStart_, indexCount_, vertexStart_, vertexCount_);
		}
		else
		{
			vertexStart_ = 0;
			vertexCount_ = 0;
		}
		return true;
	}

	//todo usage
	bool Geometry::SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex,
	                            unsigned vertexCount, bool checkIllegal)
	{
		if(indexBuffer_)
		{
			if(checkIllegal && indexStart + indexCount > indexBuffer_->GetIndexCount())
			{
				URHO3D_LOGERROR("Illegal draw range " + String(indexStart) + " to " + String(indexStart + indexCount -1)
				                + ", index buffer has " + String(indexBuffer_->GetIndexCount()) + " indices");
				return false;
			}
		}
		else if(!rawIndexData_)
		{
			indexStart = 0;
			indexCount = 0;
		}

		primitiveType_ = type;
		indexStart_ = indexStart;
		indexCount_ = indexCount;
		vertexStart_ = minVertex;
		vertexCount_  = vertexCount;
		return true;
	}

	void Geometry::SetLodDistance(float distance)
	{
		if(distance < 0.0f)
			distance = 0.0f;
		lodDistance_ = distance;
	}

	void Geometry::SetRawVertexData(SharedArrayPtr<unsigned char> data, const PODVector<VertexElement> &element)
	{
		rawVertexData_ = data;
		rawVertexSize_ = VertexBuffer::GetVertexSize(element);
		rawElements_ = element;
	}

	void Geometry::SetRawVertexData(SharedArrayPtr<unsigned char> data, unsigned elementMask)
	{
		rawVertexData_ = data;
		rawVertexSize_ = VertexBuffer::GetVertexSize(elementMask);
		rawElements_ = VertexBuffer::GetElements(elementMask);
	}

	void Geometry::SetRawIndexData(SharedArrayPtr<unsigned char> data, unsigned indexSize)
	{
		rawIndexData_ = data;
		rawIndexSize_ = indexSize;
	}

	void Geometry::Draw(Graphics *graphics)
	{
		//todo
	}

	VertexBuffer *Geometry::GetVertexBuffer(unsigned index) const
	{
		return index < vertexBuffers_.Size() ? vertexBuffers_[index] : nullptr;
	}

	unsigned short Geometry::GetbufferHash() const
	{
		unsigned short hash = 0;
		for(unsigned i=0; i<vertexBuffers_.Size(); ++i)
		{
			VertexBuffer* vBuf = vertexBuffers_[i];
			hash += *((unsigned short*)&vBuf);
		}

		IndexBuffer* iBuf = indexBuffer_;
		hash += *((unsigned short*)&iBuf);

		return hash;
	}

	void
	Geometry::GetRawData(const unsigned char *vertexData, unsigned &vertexSize, const unsigned char *&indexData,
	unsigned& indexSize, const PODVector<VertexElement> *&elements)
	{
		if(rawVertexData_)
		{
			vertexData = rawVertexData_;
			vertexSize = rawVertexSize_;
			elements = &rawElements_;
		}
		else if(vertexBuffers_.Size() && vertexBuffers_[0])
		{
			vertexData = vertexBuffers_[0]->GetShadowData();
			vertexSize = vertexBuffers_[0]->GetVertexSize();
			elements = &vertexBuffers_[0]->GetElements();
		}
		else
		{
			vertexData = nullptr;
			vertexSize = 0;
			elements = nullptr;
		}

		if(rawIndexData_)
		{
			indexData = rawIndexData_;
			indexSize = rawIndexSize_;
		}
		else
		{
			if(indexBuffer_)
			{
				indexData = indexBuffer_->GetShadowData();
				if(indexData)
					indexSize = indexBuffer_->GetIndexSize();
				else
					indexSize = 0;
			}
			else
			{
				indexData = nullptr;
				indexSize = 0;
			}
		}
	}

	void Geometry::GetRawDataShared(SharedArrayPtr<unsigned char> &vertexData, unsigned &vertexSize,
	                                SharedArrayPtr<unsigned char> &indexData, unsigned &indexSize, const
	                                PODVector<VertexElement> *&elements) const
	{
		if(rawVertexData_)
		{
			vertexData = rawVertexData_;
			vertexSize = rawVertexSize_;
			elements = &rawElements_;
		}
		else if(vertexBuffers_.Size() && vertexBuffers_[0])
		{
			//todo, why store in shadowData ??
			//todo, why check vertexBuffers_[0] ??
			vertexData = vertexBuffers_[0]->GetShadowData();
			vertexSize = vertexBuffers_[0]->GetVertexSize();
			elements = &vertexBuffers_[0]->GetElements();
		}
		else
		{
			vertexData = nullptr;
			vertexSize = 0;
			elements = nullptr;
		}

		if(rawIndexData_)
		{
			indexData = rawIndexData_;
			indexSize = rawIndexSize_;
		}
		else
		{
			if(indexBuffer_)
			{
				indexData = indexBuffer_->GetShadowData();
				if(indexData)
					indexSize = indexBuffer_->GetIndexSize();
				else
					indexSize = 0;
			}
			else
			{
				indexData = nullptr;
				indexSize = 0;
			}
		}
	}

	float Geometry::GetHitDistance(const Ray &ray, Vector3 *outNormal, Vector2 *outUV) const
	{
		return 0;
	}

	bool Geometry::IsInside(const Ray &ray) const
	{
		return false;
	}
}