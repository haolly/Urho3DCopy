//
// Created by liuhao1 on 2017/12/22.
//

#include "GPUObject.h"

namespace Urho3D
{
	GPUObject::GPUObject(Graphics *graphics) :
		graphics_(graphics),
		dataLost_(false),
		dataPending_(false)
	{
#ifdef URHO3D_OPENGL
		object_.name_ = 0;
#else
		object_.ptr_ = nullptr;
#endif
		if(graphics_)
			graphics_->AddGPUObject(this);
	}

	GPUObject::~GPUObject()
	{
		if(graphics_)
			graphics_->RemoveGPUObject(this);
	}

	void GPUObject::OnDeviceLost()
	{
#ifdef URHO3D_OPENGL
		object_.name_ = 0;
#endif
	}

	void GPUObject::OnDeviceReset()
	{

	}

	void GPUObject::Release()
	{

	}

	void GPUObject::ClearDataLost()
	{
		dataLost_ = false;
	}

	Graphics *GPUObject::GetGraphics() const
	{
		return graphics_;
	}
}