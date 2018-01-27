//
// Created by liuhao1 on 2018/1/5.
//

#ifndef URHO3DCOPY_VECTORBUFFER_H
#define URHO3DCOPY_VECTORBUFFER_H

#include "AbstractFile.h"

namespace Urho3D
{
	/***
	 * Dynamically sized buffer that can be read and written to as a steam
	 * Compare with VectorBuffer
	 */
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
		void SetData(Deserializer& data, unsigned size);

		void Clear();
		void Resize(unsigned size);

		const unsigned char* GetData() const
		{
			//Note, 可以这样写？？ so the PODVector must have a standard-layout ??
			// Yes, it is
			return size_ ? &buffer_[0] : nullptr;
		}

		unsigned char* GetModifiableData()
		{
			return size_ ? &buffer_[0] : nullptr;
		}

		const PODVector<unsigned char>& GetBuffer() const
		{
			return buffer_;
		}

	private:
		PODVector<unsigned char> buffer_;
	};

}



#endif //URHO3DCOPY_VECTORBUFFER_H
