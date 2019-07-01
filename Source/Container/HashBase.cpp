//
// Created by LIUHAO on 2017/11/30.
//

#include "HashBase.h"

namespace Urho3D
{

	void HashBase::ResetPtrs()
	{
		if(!ptrs_)
			return;

		unsigned numBuckets = NumBuckets();
		HashNodeBase** ptrs = Ptrs();
		for (unsigned i = 0; i< numBuckets; ++i)
			ptrs[i] = nullptr;
	}

	void HashBase::AllocateBuckets(unsigned size, unsigned numBuckets)
	{
		if(ptrs_)
			delete[] ptrs_;

		HashNodeBase** ptrs = new HashNodeBase* [numBuckets + 2];
		//todo, wtf ??
		unsigned * data = reinterpret_cast<unsigned *>(ptrs);
		data[0] = size;
		data[1] = numBuckets;
		ptrs_ = ptrs;
		ResetPtrs();
	}
}