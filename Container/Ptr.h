//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_PTR_H
#define URHO3DCOPY_PTR_H

#include <cstddef>
#include <cassert>
#include "RefCounted.h"

namespace Urho3D
{
    template <class T>
    class SharedPtr
    {
    public:
        SharedPtr() : ptr_(0)
        {
        }

        SharedPtr(std::nullptr_t) : ptr_(0)
        {
        }

        // Big Three
        SharedPtr(const SharedPtr<T>& rhs) : ptr_(rhs.ptr_)
        {
            AddRef();
        }

        template <class U> SharedPtr(const SharedPtr<U>& rhs) :
                ptr_(rhs.ptr_)
        {
            AddRef();
        }

        explicit SharedPtr(T* ptr) : ptr_(ptr)
        {
            AddRef();
        }

        ~SharedPtr()
        {
            ReleaseRef();
        }

        SharedPtr<T>&operator =(const SharedPtr<T>& rhs)
        {
            if(ptr_ == rhs.ptr_)
            {
                return *this;
            }
            SharedPtr<T> copy(rhs);
            Swap(copy);
            return *this;
        }

        template <class U> SharedPtr<T>&operator =(const SharedPtr<U>& rhs)
        {
            if(ptr_ == rhs.ptr_)
            {
                return *this;
            }
            SharedPtr<T> copy(rhs);
            Swap(copy);
            return *this;
        }

        SharedPtr<T>&operator =(T* ptr)
        {
            if(ptr == ptr_)
            {
                return *this;
            }
            SharedPtr<T> copy(ptr);
            Swap(copy);
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

        template <class U>
        bool operator <(const SharedPtr<U>& rhs) const
        {
            return ptr_ < rhs.ptr_;
        }

        template <class U>
        bool operator ==(const SharedPtr<U>& rhs) const
        {
            return ptr_ == rhs.ptr_;
        }

        template <class U>
        bool operator !=(const SharedPtr<U>& rhs) const
        {
            return ptr_ != rhs.ptr_;
        }

        operator T*() const
        {
            return ptr_;
        }

        void Reset()
        {
            ReleaseRef();
        }

        //todo
        T* Detach()
        {
            T* ptr = ptr_;
            if(ptr_)
            {
                RefCount* refCount = RefCountPtr();
                ++refCount->refs_;
                Reset();
                --refCount->refs_;
            }
            return ptr;
        }

        template <class U>
        void StaticCast(const SharedPtr<U>& rhs)
        {
            SharedPtr<T> copy(static_cast<T*>(rhs.Get()));
            Swap(copy);
        }

        template <class U>
        void DynamicCast(const SharedPtr<U>& rhs)
        {
            SharedPtr<T> copy(dynamic_cast<T*>(rhs.Get()));
            Swap(copy);
        }

        void Swap(SharedPtr& rhs)
        {
            //Todo
            Urho3D::Swap(ptr_, rhs.ptr_);
        }

        bool Null() const
        {
            return ptr_ == 0;
        }

        bool NotNull() const
        {
            return ptr_ != 0;
        }

        T* Get() const
        {
            return ptr_;
        }

        int Refs() const
        {
            return ptr_ ? ptr_ ->Refs() : 0;
        }

        int WeakRefs() const
        {
            return ptr_ ? ptr_->WeakRefs() : 0;
        }

        RefCount* RefCountPtr() const
        {
            return ptr_ ? ptr_->RefCountPtr() : 0;
        }

        unsigned ToHash() const
        {
            return (unsigned)((size_t)ptr_ / sizeof(T));
        }


    private:
        template <class U> friend class SharedPtr;

        void AddRef()
        {
            if(ptr_)
            {
                ptr_->AddRef();
            }
        }

        void ReleaseRef()
        {
            if(ptr_)
            {
                ptr_->ReleaseRef();
                ptr_ = 0;
            }
        }

        T* ptr_;
    };

    template <class T, class U>
    SharedPtr<T> StaticCast(const SharedPtr<U>& ptr)
    {
        //todo, do not need new ?
        //so all template do not need new?
        SharedPtr<T> ret;
        ret.StaticCast(ptr);
        return ret;
    };

    template <class T, class U>
    SharedPtr<T> DynamicCast(const SharedPtr<U>& ptr)
    {
        SharedPtr<T> ret;
        ret.DynamicCast(ptr);
        return ret;
    };

    template <class T> class WeakPtr
    {
    public:
        WeakPtr() :
            ptr_(0),
			refCount_(nullptr)
        {
        }

	    WeakPtr(std::nullptr_t) :
	        ptr_(0),
			refCount_(nullptr)
	    {
	    }

	    WeakPtr(const WeakPtr<T>& rhs) : ptr_(rhs.ptr_), refCount_(rhs.refCount_)
	    {
		    AddRef();
	    }

	    template<class U>
	    WeakPtr(const WeakPtr<U>& rhs) :
			    ptr_(rhs.ptr_),
			    refCount_(rhs.refCount_)
	    {
		    AddRef();
	    }

	    WeakPtr(const SharedPtr<T>& rhs) : ptr_(rhs.Get()), refCount_(rhs.RefCountPtr())
	    {
		    AddRef();
	    }

	    explicit WeakPtr(T* ptr) : ptr_(ptr), refCount_(ptr ? ptr->RefCountPtr() : 0)
	    {
			AddRef();
	    }

	    ~WeakPtr()
	    {
		    ReleaseRef();
	    }

	    WeakPtr<T>&operator =(const SharedPtr<T>& rhs)
	    {
		    if(ptr_ == rhs.Get() && refCount_ == rhs.RefCountPtr())
			    return *this;
		    ReleaseRef();
		    ptr_ = rhs.Get();
		    refCount_ = rhs.RefCountPtr();
		    AddRef();
		    return *this;
	    }

	    WeakPtr<T>&operator =(const WeakPtr<T>& rhs)
	    {
		    if(ptr_ == rhs.ptr_ && refCount_ == rhs.refCount_)
			    return *this;

		    ReleaseRef();
		    ptr_ = rhs.ptr_;
		    refCount_ = rhs.refCount_;
		    AddRef();
		    return *this;
	    }

	    template <class U> WeakPtr<T>&operator =(const WeakPtr<U>& rhs)
	    {
		    if(ptr_ == rhs.ptr_ && refCount_ == rhs.refCount_)
			    return *this;
		    ReleaseRef();
		    ptr_ = rhs.ptr_;
		    refCount_ = rhs.refCount_;
		    AddRef();
			return *this;
	    }

	    WeakPtr<T>&operator =(T* ptr)
	    {
		    RefCount* refCount = ptr ? ptr->RefCountPtr() : 0;
		    if(ptr_ == ptr && refCount_ == refCount)
			    return *this;
		    ReleaseRef();
		    ptr_ = ptr;
		    refCount_ = refCount;
		    AddRef();
		    return *this;
	    }

	    SharedPtr<T> Lock() const
	    {
		    if(Expired())
			    return SharedPtr<T>();
		    else
			    return SharedPtr<T>(ptr_);
	    }

	    T* Get() const
	    {
		    if(Expired())
			    return 0;
		    else
			    return ptr_;
	    }

	    T*operator ->() const
	    {
		    T* rawPtr = Get();
		    assert(rawPtr);
		    return rawPtr;
	    }

	    T&operator *() const
	    {
		    T* rawPtr = Get();
		    assert(rawPtr);
		    return *rawPtr;
	    }

	    T&operator [](const int index)
	    {
		    T* rawPtr = Get();
		    assert(rawPtr);
		    return (*rawPtr)[index];
	    }

	    template <class U>
	    bool operator ==(const WeakPtr<U>& rhs) const
	    {
		    return ptr_ == rhs.ptr_ && refCount_ == rhs.refCount_;
	    }

	    template <class U>
	    bool operator !=(const WeakPtr<U>& rhs) const
	    {
		    return ptr_ != rhs.ptr_ || refCount_ != rhs.refCount_;
	    }

	    template <class U>
	    bool operator <(const WeakPtr<U>& rhs) const
	    {
		    return ptr_ < rhs.ptr_;
	    }

	    operator T*()
	    {
		    return Get();
	    }

	    void Reset()
	    {
		    ReleaseRef();
	    }



        //todo
        bool Expired() const
        {
            return refCount_ ? refCount_->refs_ < 0 : true;
        }
    private:
        //Add a weak reference to the object pointed to
        void AddRef()
        {
            if(refCount_)
            {
                assert(refCount_->weakRefs_ >= 0);
                ++(refCount_->weakRefs_);
            }
        }
        void ReleaseRef()
        {
            if(refCount_)
            {
                assert(refCount_->weakRefs_ >0);
                --(refCount_->weakRefs_);

	            //todo, may be memory leaks when this conditions are met
                if(Expired() && !refCount_->weakRefs_)
                {
                    delete refCount_;
                }
            }
            ptr_ = 0;
            refCount_ = 0;
        }
	    //note, make WeakPtr<U> is a friend of WeakPtr<T>, regardless what T and U is
	    // ref https://stackoverflow.com/questions/8967521/class-template-with-template-class-friend-whats-really-going-on-here
        template <class U> friend class WeakPtr;
        T* ptr_;
        RefCount* refCount_;
    };
}//end namespace

#endif //URHO3DCOPY_PTR_H
