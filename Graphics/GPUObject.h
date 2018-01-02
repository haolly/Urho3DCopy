//
// Created by liuhao1 on 2017/12/22.
//

#ifndef URHO3DCOPY_GPUOBJECT_H
#define URHO3DCOPY_GPUOBJECT_H

#include "../Container/Ptr.h"

namespace Urho3D
{
	class Graphics;

	union GPUObjectHandle
	{
		//Object pointer (DirectX)
		void* ptr_;
		//Object name (OpenGL)
		unsigned name_;
	};

	class GPUObject
	{
	public:
		GPUObject(Graphics* graphics);
		virtual ~GPUObject();

		virtual void OnDeviceLost();
		virtual void OnDeviceReset();
		virtual void Release();

		void ClearDataLost();

		Graphics* GetGraphics() const;
		void* GetGPUObject() const
		{
			return object_.ptr_;
		}

		unsigned GetGPUObjectName() const
		{
			return object_.name_;
		}

		bool IsDataLost() const
		{
			return dataLost_;
		}

		bool HasPendingData() const
		{
			return dataPending_;
		}


	protected:
		WeakPtr<Graphics> graphics_;
		GPUObjectHandle object_;
		//Data lost flag
		//Todo,
		bool dataLost_;
		bool dataPending_;
	};
}



#endif //URHO3DCOPY_GPUOBJECT_H
