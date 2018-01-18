//
// Created by liuhao1 on 2017/12/25.
//

#include <d3d11.h>
#include "../VertexBuffer.h"
#include "../../IO/Log.h"
#include "../Graphics.h"

namespace Urho3D
{
	bool VertexBuffer::Create()
	{
		Release();

		if(!vertexCount_ || !elementMask_)
			return true;

		if(graphics_)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = dynamic_ ? D3D11_CPU_ACCESS_WRITE : 0;
			bufferDesc.Usage = dynamic_ ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = (UINT)(vertexCount_ * vertexSize_);

			//todo
		}
	}

	void VertexBuffer::OnDeviceLost()
	{
		//No-op on Direct3D11
	}

	void VertexBuffer::OnDeviceReset()
	{
		// No-op on Direct3D11
	}

	void VertexBuffer::Release()
	{
		Unlock();
		if(graphics_)
		{
			for(unsigned i=0; i<MAX_VERTEX_STREAMS; ++i)
			{
				if(graphics_ ->GetVertexBuffer(i) == this)
					//todo
			}
		}
	}

	bool VertexBuffer::UpdateToGPU()
	{
		if(object_.ptr_ && shadowData_)
			return SetData(shadowData_.Get());
		else
			return false;
	}

	void *VertexBuffer::MapBuffer(unsigned start, unsigned count, bool discard)
	{
		//todo
		return nullptr;
	}

	void VertexBuffer::UnmapBuffer()
	{
		if(object_.ptr_ && lockState_ == LOCK_NONE)
		{
			//todo
		}
	}

	void *VertexBuffer::Lock(unsigned start, unsigned count, bool discard)
	{
		if(lockState_ != LOCK_NONE)
		{
			URHO3D_LOGERROR("Vertex buffer already locked");
			return nullptr;
		}
		if(!vertexSize_)
		{
			URHO3D_LOGERROR("Vertex elements not defined, can not lock vertex buffer");
			return nullptr;
		}
		if(start + count > vertexCount_)
		{
			URHO3D_LOGERROR("Illegal range for locking vertex buffer");
			return nullptr;
		}

		if(!count)
			return nullptr;

		lockStart_ = start;
		lockCount_ = count;

		if(object_.ptr_ && !shadowData_ && dynamic_)
			return MapBuffer(start, count, discard);
		else if(shadowData_)
		{
			lockState_ = LOCK_SHADOW;
			return shadowData_.Get() + start * vertexSize_;
		}
		else if(graphics_)
		{
			lockState_ = LOCK_SCRATCH;
			lockScratchData_ = graphics_->ReserveScratchBuffer(count * vertexSize_);
			return lockScratchData_;
		}
		else
			return nullptr;
	}

	void VertexBuffer::Unlock()
	{
		switch (lockState_)
		{
			case LOCK_HARDWARE:
				UnmapBuffer();
				break;
			case LOCK_SHADOW:
				SetDataRange(shadowData_.Get() + lockStart_ * vertexSize_, lockStart_, lockCount_);
				break;
			case LOCK_SCRATCH:
				SetDataRange(lockScratchData_, lockStart_, lockCount_);
				if(graphics_)
					graphics_->FreeScratchBuffer(lockScratchData_);
				lockScratchData_ = nullptr;
				lockState_ = LOCK_NONE;
				break;
			default:
				break;
		}
	}
}
