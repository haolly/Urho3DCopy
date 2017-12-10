//
// Created by liuhao on 2017/12/9.
//

#ifndef URHO3DCOPY_MEMORYBUFFER_H
#define URHO3DCOPY_MEMORYBUFFER_H

#include "AbstractFile.h"

namespace Urho3D
{
	class MemoryBuffer : public AbstractFile
	{
	public:
		MemoryBuffer(void* data, unsigned size);
		MemoryBuffer(const void* data, unsigned size);
		MemoryBuffer(PODVector<unsigned char>& data);
		MemoryBuffer(const PODVector<unsigned char>& data);

		virtual unsigned Read(void* dest, unsigned size) override ;
		virtual unsigned Seek(unsigned position) override ;
		virtual unsigned Write(const void* data, unsigned size) override ;

		unsigned char* GetData() { return buffer_; }
		bool IsReadOnly() { return readOnly_; }

	private:
		unsigned char* buffer_;
		bool readOnly_;
	};

}



#endif //URHO3DCOPY_MEMORYBUFFER_H
