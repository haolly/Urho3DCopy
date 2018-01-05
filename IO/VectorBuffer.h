//
// Created by liuhao1 on 2018/1/5.
//

#ifndef URHO3DCOPY_VECTORBUFFER_H
#define URHO3DCOPY_VECTORBUFFER_H

#include "AbstractFile.h"

namespace Urho3D
{
	class VectorBuffer : public AbstractFile
	{
	public:
		VectorBuffer();
		VectorBuffer(const PODVector<unsigned char>& data);
		VectorBuffer(const void* data, unsigned size);
		VectorBuffer(Deserializer& source, unsigned size);

		virtual unsigned Read(void* dest, unsigned size) override ;
		virtual unsigned Seek(unsigned position) override ;
		virtual unsigned Write(const void* data, unsigned size) override ;

		void SetData(const PODVector<unsigned char>& data);

		void SetData(const void* data, unsigned size);
		//todo

		const unsigned char* GetData() const
		{
			//todo, 可以这样写？？ so the PODVector must have a standard-layout ??
			return size_ ? &buffer_[0] : nullptr;
		}

	private:
		PODVector<unsigned char> buffer_;
	};

}



#endif //URHO3DCOPY_VECTORBUFFER_H
