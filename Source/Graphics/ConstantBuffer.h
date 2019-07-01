//
// Created by liuhao1 on 2018/1/11.
//

#ifndef URHO3DCOPY_CONSTANTBUFFER_H
#define URHO3DCOPY_CONSTANTBUFFER_H

#include "GPUObject.h"
#include "../Core/Context.h"
#include "../Container/ArrayPtr.h"

namespace Urho3D
{
	class ConstantBuffer : public Object, public GPUObject
	{
		URHO3D_OBJECT(ConstantBuffer, Object);
	public:
		explicit ConstantBuffer(Context* context);

		~ConstantBuffer() override ;

		void OnDeviceReset() override ;

		void Release() override ;

		bool SetSize(unsigned size);
		void SetParameter(unsigned offset, unsigned size, const void* data);
		void SetVector3ArrayParameter(unsigned offset, unsigned rows, const void* data);
		void Apply();

		unsigned GetSize() const { return size_; }
		bool IsDirty() const { return dirty_; }

	private:
		SharedArrayPtr<unsigned char> shadowData_;
		unsigned size_;
		bool dirty_;
	};
}



#endif //URHO3DCOPY_CONSTANTBUFFER_H
