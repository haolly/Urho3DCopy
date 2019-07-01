//
// Created by liuhao on 2019-04-14.
//

#include "../../Graphics/Graphics.h"
#include "../../Graphics/GraphicsImpl.hpp"
#include "../../Graphics/ConstantBuffer.h"
#include "../ConstantBuffer.h"


namespace Urho3D
{
	void ConstantBuffer::Release()
	{
		if(object_.name_)
		{
			if(!graphics_)
				return;

#ifndef GL_ES_VERSION_2_0
			graphics_->SetUBO(0);
			glDeleteBuffers(1, &object_.name_);
#endif
			object_.name_ = 0;
		}

		shadowData_.Reset();
		size_ = 0;
	}

	void ConstantBuffer::OnDeviceReset()
	{
		if(size_)
			SetSize(size_);
	}

	bool ConstantBuffer::SetSize(unsigned size)
	{
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
#ifndef GL_ES_VERSION_2_0
			if(!object_.name_)
				glGenBuffers(1, &object_.name_);
			graphics_->SetUBO(object_.name_);
			glBufferData(GL_UNIFORM_BUFFER, size_, shadowData_.Get(), GL_DYNAMIC_DRAW);
#endif
		}
		return true;
	}

	void ConstantBuffer::Apply()
	{
		if(dirty_ && object_.name_)
		{
#ifndef GL_ES_VERSION_2_0
			graphics_->SetUBO(object_.name_);
			glBufferData(GL_UNIFORM_BUFFER, size_, shadowData_.Get(), GL_DYNAMIC_DRAW);
#endif
			dirty_ = false;
		}
	}

}
