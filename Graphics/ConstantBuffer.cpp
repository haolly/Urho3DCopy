//
// Created by liuhao1 on 2018/1/11.
//

#include "ConstantBuffer.h"

namespace Urho3D
{

	ConstantBuffer::ConstantBuffer(Context *context) :
			Object(context),
			GPUObject(GetSubsystem<Graphics>())
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		Release();
	}

	void ConstantBuffer::SetParameter(unsigned offset, unsigned size, const void *data)
	{
		if(offset + size > size_)
			return;

		memcpy(&shadowData_[offset], data, size);
		dirty_ = true;
	}

	void ConstantBuffer::SetVector3ArrayParameter(unsigned offset, unsigned rows, const void* data)
	{
		if(offset + rows * 4 * sizeof(float) > size_)
			return;

		float* dest = (float*)&shadowData_[offset];
		const float* src = (const float*)data;

		while(rows--)
		{
			*dest++ = *src++;
			*dest++ = *src++;
			*dest++ = *src++;
			// Skp over the w coordinate
			++dest;
		}
		dirty_ = true;
	}

}