//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_HASHBASE_H
#define URHO3DCOPY_HASHBASE_H

#include "Swap.h"
#include "Allocator.h"

namespace Urho3D
{
	struct HashNodeBase
	{
		HashNodeBase() :
			down_(nullptr),
			prev_(nullptr),
			next_(nullptr)
		{
		}

		// List data which has the same hash value
		HashNodeBase* down_;
		// Maybe the hash value data
		HashNodeBase* prev_;
		HashNodeBase* next_;
	};

	struct HashIteratorBase
	{
		HashIteratorBase() :
			ptr_(nullptr)
		{
		}

		explicit HashIteratorBase(HashNodeBase* ptr) :
			ptr_(ptr)
		{
		}

		bool operator ==(const HashIteratorBase& rhs) const
		{
			return ptr_ == rhs.ptr_ ;
		}

		bool operator !=(const HashIteratorBase& rhs) const
		{
			return ptr_ != rhs.ptr_;
		}

		void GotoNext()
		{
			if(ptr_)
				ptr_ = ptr_ ->next_;
		}

		void GotoPrev()
		{
			if(ptr_)
				ptr_ = ptr_->prev_;
		}


		HashNodeBase* ptr_;
	};

	class HashBase
	{
	public:
		static const unsigned MIN_BUCKETS = 8;
		static const unsigned MAX_LOAD_FACTOR = 4;

		HashBase():
				head_(nullptr),
				tail_(nullptr),
				ptrs_(nullptr),
				allocator_(nullptr)
		{
		}

		void Swap(HashBase& rhs)
		{
			Urho3D::Swap(head_, rhs.head_);
			Urho3D::Swap(tail_, rhs.tail_);
			Urho3D::Swap(ptrs_, rhs.ptrs_);
			Urho3D::Swap(allocator_, rhs.allocator_);
		}

		unsigned Size() const
		{
			//todo, why cast to unsigned?
			return ptrs_ ? (reinterpret_cast<unsigned *>(ptrs_))[0] : 0;
		}

		unsigned NumBuckets() const
		{
			return ptrs_ ? (reinterpret_cast<unsigned*>(ptrs_))[1] : 0;
		}

		bool Empty() const
		{
			return Size() == 0;
		}

	protected:
		void AllocateBuckets(unsigned size, unsigned numBuckets);
		void ResetPtrs();
		void SetSize(unsigned size)
		{
			if(ptrs_)
			{
				(reinterpret_cast<unsigned *>(ptrs_))[0] = size;
			}
		}

		//note, why plus 2
		//index 0 is size, index 1 is buckets size
		HashNodeBase** Ptrs() const { return ptrs_ ? ptrs_ + 2 : nullptr; }

		HashNodeBase* head_;
		HashNodeBase* tail_;
		HashNodeBase** ptrs_;
		AllocatorBlock* allocator_;
	};
}


#endif //URHO3DCOPY_HASHBASE_H
