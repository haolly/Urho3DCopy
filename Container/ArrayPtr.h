//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_ARRAYPTR_H
#define URHO3DCOPY_ARRAYPTR_H

#include <cassert>
#include "RefCounted.h"

namespace Urho3D
{
	template <class T>
	class SharedArrayPtr
	{
	public:
		SharedArrayPtr() :
				ptr_(0),
				refCount_(nullptr)
		{
		}

		SharedArrayPtr(const SharedArrayPtr<T> rhs) :
				ptr_(rhs.ptr_),
				refCount_(rhs.refCount_)
		{
			AddRef();
		}

		explicit SharedArrayPtr(T* ptr) :
				ptr_(ptr),
				refCount_(new RefCount())
		{
		}

		~SharedArrayPtr()
		{
			ReleaseRef();
		}

		SharedArrayPtr<T>&operator =(const SharedArrayPtr<T>& rhs)
		{
			if(ptr_ == rhs.ptr_)
				return *this;
			ReleaseRef();
			ptr_ = rhs.ptr_;
			refCount_ = rhs.refCount_;
			AddRef();
			return *this;
		}

		SharedArrayPtr<T>&operator =(T* ptr)
		{
			if(ptr_ == ptr)
				return *this;

			ReleaseRef();
			if(ptr)
			{
				ptr_ = ptr;
				refCount_ = new RefCount();
				AddRef();
			}
			return *this;
		}

		T*operator ->() const
		{
			assert(ptr_);
			return ptr_;
		}

		T&operator *() const
		{
			assert(ptr_);
			return *ptr_;
		}

		T&operator [](const int index)
		{
			assert(ptr_);
			return ptr_[index];
		}

		bool operator==(const SharedArrayPtr<T>& rhs) const
		{
			return ptr_ == rhs.ptr_;
		}

		bool operator !=(const SharedArrayPtr<T>& rhs) const
		{
			return ptr_ != rhs.ptr_;
		}

		bool operator <(const SharedArrayPtr<T>& rhs) const
		{
			return ptr_ < rhs.ptr_;
		}

		operator T*() const { return ptr_; }

		void Reset()
		{
			ReleaseRef();
		}

		template <class U>
		void StaticCast(const SharedArrayPtr<U>& rhs)
		{
			ReleaseRef();
			ptr_ = static_cast<T*>(rhs.Get());
			refCount_ = rhs.RefCountPtr();
			AddRef();
		}

		bool Null() const
		{
			return ptr_ == nullptr;
		}

		bool NotNull() const
		{
			return ptr_ != nullptr;
		}

		T* Get() const
		{
			return ptr_;
		}

		int Refs() const
		{
			return refCount_ ? refCount_->refs_ : 0;
		}

		int WeakRefs() const
		{
			return refCount_ ? refCount_->weakRefs_ : 0;
		}

		RefCount* RefCountPtr() const
		{
			return refCount_;
		}

		unsigned ToHash() const
		{
			return (unsigned)((size_t)ptr_ / sizeof(T));
		}


	private:
		template <class U> SharedArrayPtr<T>&operator =(const SharedArrayPtr<U>& rhs);

		void AddRef()
		{
			if(refCount_)
			{
				assert(refCount_->refs_ >= 0);
				++(refCount_->refs_);
			}
		}

		void ReleaseRef()
		{
			if(refCount_)
			{
				assert(refCount_->refs_ > 0);
				--(refCount_->refs_);
				if(!refCount_->refs_)
				{
					refCount_->refs_ = -1;
					delete [] ptr_;
				}
				if(refCount_->refs_ < 0 && !refCount_->weakRefs_)
					delete refCount_;
			}
			ptr_ = 0;
			refCount_ = 0;
		}

		T* ptr_;
		RefCount* refCount_;
	};
}

#endif //URHO3DCOPY_ARRAYPTR_H
