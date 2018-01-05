//
// Created by liuhao1 on 2017/12/26.
//

#ifndef URHO3DCOPY_GEOMETRY_H
#define URHO3DCOPY_GEOMETRY_H

#include "../Core/Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Math/Ray.h"

namespace Urho3D
{
	//几何体
	class Geometry : public Object
	{
		URHO3D_OBJECT(Geometry, Object);
	public:
		Geometry(Context* context);
		virtual ~Geometry() override ;

		bool SetNumVertexBuffers(unsigned num);
		bool SetVertexBuffer(unsigned index, VertexBuffer* buffer);
		void SetIndexBuffer(IndexBuffer* buffer);
		bool SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, bool getUsedVertexRange = true);
		bool SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned vertexStart,
		                  unsigned vertexCount, bool checkIllegal = true);
		void SetLodDistance(float distance);
		void SetRawVertexData(SharedArrayPtr<unsigned char> data, const PODVector<VertexElement>& element);
		void SetRawVertexData(SharedArrayPtr<unsigned char> data, unsigned elementMask);
		void SetRawIndexData(SharedArrayPtr<unsigned char> data, unsigned indexSize);
		void Draw(Graphics* graphics);

		const Vector<SharedPtr<VertexBuffer> >& GetVertexBuffers() const { return vertexBuffers_; }
		unsigned GetNumVertexBuffers() const { return vertexBuffers_.Size(); }

		VertexBuffer* GetVertexBuffer(unsigned index) const;
		IndexBuffer* GetIndexBuffer() const { return indexBuffer_; }
		PrimitiveType GetPrimitiveType() const { return primitiveType_; }
		unsigned GetIndexStart() const { return indexStart_; }
		unsigned GetIndexCount() const { return indexCount_; }
		unsigned GetVertexStart() const { return vertexStart_; }
		unsigned GetVertexCount() const { return vertexCount_; }
		float GetLodDistance() const { return lodDistance_; }
		unsigned short GetbufferHash() const;
		void GetRawData(const unsigned char* vertexData, unsigned& vertexSize, const unsigned char*& indexData,
		                unsigned& indexSize, const PODVector<VertexElement>*& elements) const;

		void GetRawDataShared(SharedArrayPtr<unsigned char>& vertexData, unsigned& vertexSize,
		                      SharedArrayPtr<unsigned char>& indexData, unsigned& indexSize, const
		                      PODVector<VertexElement>*& elements) const;
		float GetHitDistance(const Ray& ray, Vector3* outNormal = nullptr, Vector2* outUV = nullptr) const;
		bool IsInside(const Ray& ray) const;
		bool IsEmpty() const { return indexCount_ == 0 && vertexCount_ == 0; }


	private:
		//todo, why need only one indexbuff but manay vertexbuffer ??
		Vector<SharedPtr<VertexBuffer> > vertexBuffers_;
		SharedPtr<IndexBuffer> indexBuffer_;
		//todo, only one primitiveType ??
		PrimitiveType primitiveType_;
		unsigned indexStart_;
		unsigned indexCount_;
		unsigned vertexStart_;
		unsigned vertexCount_;
		float lodDistance_;
		PODVector<VertexElement> rawElements_;
		//todo, Raw vertex data override
		SharedArrayPtr<unsigned char> rawVertexData_;
		SharedArrayPtr<unsigned char> rawIndexData_;
		unsigned rawVertexSize_;
		unsigned rawIndexSize_;
	};
}


#endif //URHO3DCOPY_GEOMETRY_H
