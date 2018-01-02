//
// Created by liuhao1 on 2017/12/25.
//

#include "IndexBuffer.h"
#include "../IO/Log.h"

namespace Urho3D
{

	IndexBuffer::IndexBuffer(Context *context, bool forceHeadless) :
		Object(context),
		GPUObject(forceHeadless ? nullptr : GetSubsystem<Graphics>()),
		indexCount_(0),
		indexSize_(0),
		lockState_(LOCK_NONE),
		lockStart_(0),
		lockCount_(0),
		lockScratchData_(nullptr),
		shadowed_(false),
		dynamic_(false),
		discardLock_(false)
	{
		if(!graphics_)
			shadowed_ = true;
	}

	IndexBuffer::~IndexBuffer()
	{
		Release();
	}


	void IndexBuffer::SetShadowed(bool enable)
	{
		if(!graphics_)
			enable = true;
		if(enable != shadowed_)
		{
			if(enable && indexSize_ && indexCount_)
				shadowData_ = new unsigned char[indexCount_ * indexSize_];
			else
				shadowData_.Reset();

			shadowed_ = enable;
		}
	}

	bool IndexBuffer::SetSize(unsigned indexCount, bool largeIndices, bool dynamic)
	{
		Unlock();
		//todo
	}

	bool IndexBuffer::SetData(const void *data)
	{
		return false;
	}

	bool IndexBuffer::SetDataRange(const void *data, unsigned start, unsigned count, bool discard)
	{
		return false;
	}

	void *IndexBuffer::Lock(unsigned start, unsigned count, bool discard)
	{
		return nullptr;
	}


	bool IndexBuffer::GetUsedVertexRange(unsigned start, unsigned count, unsigned &minVertex, unsigned &vertexCount)
	{
		if(!shadowData_)
		{
			//todo why?
			URHO3D_LOGERROR("Used vertex range can only be queried from an index buffer with shadow data");
			return false;
		}
		if(start + count > indexCount_)
		{
			URHO3D_LOGERROR("Illegal index range for querying used vertices");
			return false;
		}

		minVertex = M_MAX_UNSIGNED;
		unsigned maxVertex = 0;
		if(indexSize_ == sizeof(unsigned))
		{
			unsigned* indices = ((unsigned*)shadowData_.Get()) + start;
			for(unsigned i = 0; i<count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}
		else
		{
			unsigned short* indices = ((unsigned short*)shadowData_.Get()) + start;
			for(unsigned i=0; i<count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}
		vertexCount = maxVertex - minVertex;
		return true;
	}

	bool IndexBuffer::Create()
	{
		return false;
	}

	bool IndexBuffer::UpdateToGPU()
	{
		return false;
	}

	void *IndexBuffer::MapBuffer(unsigned start, unsigned count, bool discard)
	{
		return nullptr;
	}

	void IndexBuffer::UnmapBuffer()
	{

	}
}