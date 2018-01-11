//
// Created by liuhao1 on 2017/12/25.
//

#ifndef URHO3DCOPY_INDEXBUFFER_H
#define URHO3DCOPY_INDEXBUFFER_H

#include "GPUObject.h"
#include "../Core/Object.h"
#include "../Container/ArrayPtr.h"
#include "GraphicsDefs.h"

namespace Urho3D
{
	/**
	 * Todo, what is difference with VertexBuffer ??
	 * ref https://msdn.microsoft.com/en-us/library/windows/desktop/ff476898(v=vs.85).aspx
	 */
	class IndexBuffer : public Object, public GPUObject
	{
		URHO3D_OBJECT(IndexBuffer, Object);
	public:
		IndexBuffer(Context* context, bool forceHeadless = false);
		virtual ~IndexBuffer() override ;

		virtual void OnDeviceLost() override ;
		virtual void OnDeviceReset() override ;
		virtual void Release() override ;

		void SetShadowed(bool enable);
		bool SetSize(unsigned indexCount, bool largeIndices, bool dynamic = false);
		bool SetData(const void* data);
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);
		void* Lock(unsigned start, unsigned count, bool discard = false);
		void Unlock();

		bool IsShadowed() const
		{
			return shadowed_;
		}

		bool IsDynamic() const { return dynamic_; }
		bool IsLocked() const { return lockState_ == LOCK_NONE; }
		unsigned GetIndexCount() const { return indexCount_; }
		unsigned GetIndexSize() const { return indexSize_; }
		bool GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount);
		unsigned char* GetShadowData() const { return shadowData_.Get(); }
		SharedArrayPtr<unsigned char> GetShadowDataShared() const { return shadowData_; }


	private:
		bool Create();
		bool UpdateToGPU();
		void* MapBuffer(unsigned start, unsigned count, bool discard);
		void UnmapBuffer();

		//todo, when and how ??
		SharedArrayPtr<unsigned char> shadowData_;
		//Number of indices
		unsigned indexCount_;
		//Todo, the size of each index ??
		unsigned indexSize_;
		LockState lockState_;
		unsigned lockStart_;
		unsigned lockCount_;
		void* lockScratchData_;
		bool dynamic_;
		bool shadowed_;
		bool discardLock_;
	};
}


#endif //URHO3DCOPY_INDEXBUFFER_H
