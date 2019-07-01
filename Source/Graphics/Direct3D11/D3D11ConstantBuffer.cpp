//
// Created by liuhao1 on 2018/1/11.
//

#include "../ConstantBuffer.h"
#include "D3D11GraphicsImpl.h"

namespace Urho3D
{
	void ConstantBuffer::OnDeviceReset()
	{
		// No-op on Direct3D11
	}

	void ConstantBuffer::Release()
	{
		URHO3D_SAFE_RELEASE(object_.ptr_);
		shadowData_.Reset();
		size_ = 0;
	}

	bool ConstantBuffer::SetSize(unsigned size)
	{
		Release();

		if(!size)
		{
			URHO3D_LOGERROR("Can not create zero-sized constant buffer");
			return false;
		}

		// Round up to next 16 bytes
		size += 15;
		size &= 0xfffffff0;

		size_ = size;
		dirty_ = false;
		shadowData_ = new unsigned char[size_];
		memset(shadowData_.Get(), 0, size_);

		if(graphics_)
		{
			D3D11_BUFFER_DESC bufferDesc;
			memset(&bufferDesc, 0, sizeof(bufferDesc));

			bufferDesc.ByteWidth = size_;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;

			//todo
		}
	}

	void ConstantBuffer::Apply()
	{
		//todo
	}
}
