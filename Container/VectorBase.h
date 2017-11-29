//
// Created by liuhao on 2017/11/28.
//

#ifndef URHO3DCOPY_VECTORBASE_H
#define URHO3DCOPY_VECTORBASE_H

namespace Urho3D
{
    template <class T>
    struct RandomAccessIterator
    {
        RandomAccessIterator() : ptr_(0)
        {
        }

        explicit RandomAccessIterator(T* ptr) : ptr_(ptr)
        {
        }

        //Note, the return value is not const !!
        T*operator ->() const
        {
            return ptr_;
        }

        T&operator *() const
        {
            return *ptr_;
        }

        RandomAccessIterator<T> operator ++()
        {
            ++ptr_;
            return *this;
        }

        //Postincrement the pointer
        RandomAccessIterator<T> operator ++(int)
        {
            RandomAccessIterator<T> it = *this;
            ++ptr_;
            return it;
        }

        RandomAccessIterator<T> operator --()
        {
            --ptr_;
            return *this;
        }

        RandomAccessIterator<T> operator --(int)
        {
            RandomAccessIterator<T> it = *this;
            --ptr_;
            return it;
        }

        RandomAccessIterator<T>&operator +=(int value)
        {
            ptr_ += value;
            return *this;
        }

        RandomAccessIterator<T>&operator -=(int value)
        {
            ptr_ -= value;
            return *this;
        }

        RandomAccessIterator<T> operator+(int value)
        {
            return RandomAccessIterator<T>(ptr_ + value);
        }

        RandomAccessIterator<T> operator-(int value)
        {
            return RandomAccessIterator<T>(ptr_ - value);
        }

        // Calculate offset to another iterator
        int operator -(const RandomAccessIterator& rhs) const
        {
            return (int)(ptr_ - rhs.ptr_);
        }

        bool operator==(const RandomAccessIterator& rhs) const
        {
            return ptr_ == rhs.ptr_;
        }

        bool operator !=(const RandomAccessIterator& rhs) const
        {
            return ptr_ != rhs.ptr_;
        }

        bool operator <(const RandomAccessIterator& rhs) const
        {
            return ptr_ < rhs.ptr_;
        }

        bool operator >(const RandomAccessIterator& rhs) const
        {
            return ptr_ > rhs.ptr_;
        }

        bool operator <=(const RandomAccessIterator& rhs) const
        {
            return ptr_ <= rhs.ptr_;
        }

        bool operator >=(const RandomAccessIterator& rhs) const
        {
            return ptr_ >= rhs.ptr_;
        }

        T* ptr_;
    };

    template <class T>
    struct RandomAccessConstIterator
    {
        RandomAccessConstIterator() : ptr_(0)
        {
        }

        explicit RandomAccessConstIterator(T* ptr) : ptr_(ptr)
        {
        }

        RandomAccessConstIterator(const RandomAccessIterator<T>& rhs) :
            ptr_(rhs.ptr_)
        {
        }

        RandomAccessConstIterator<T>&operator =(const RandomAccessIterator<T>& rhs)
        {
            ptr_ = rhs.ptr_;
            return *this;
        }


        //Note return value is const
        const T*operator ->() const
        {
            return ptr_;
        }

        const T&operator *() const
        {
            return *ptr_;
        }

        /// Preincrement the pointer.
        RandomAccessConstIterator<T>& operator ++()
        {
            ++ptr_;
            return *this;
        }

        /// Postincrement the pointer.
        RandomAccessConstIterator<T> operator ++(int)
        {
            RandomAccessConstIterator<T> it = *this;
            ++ptr_;
            return it;
        }

        /// Predecrement the pointer.
        RandomAccessConstIterator<T>& operator --()
        {
            --ptr_;
            return *this;
        }

        /// Postdecrement the pointer.
        RandomAccessConstIterator<T> operator --(int)
        {
            RandomAccessConstIterator<T> it = *this;
            --ptr_;
            return it;
        }

        /// Add an offset to the pointer.
        RandomAccessConstIterator<T>& operator +=(int value)
        {
            ptr_ += value;
            return *this;
        }

        /// Subtract an offset from the pointer.
        RandomAccessConstIterator<T>& operator -=(int value)
        {
            ptr_ -= value;
            return *this;
        }

        /// Add an offset to the pointer.
        RandomAccessConstIterator<T> operator +(int value) const { return RandomAccessConstIterator<T>(ptr_ + value); }

        /// Subtract an offset from the pointer.
        RandomAccessConstIterator<T> operator -(int value) const { return RandomAccessConstIterator<T>(ptr_ - value); }

        /// Calculate offset to another iterator.
        int operator -(const RandomAccessConstIterator& rhs) const { return (int)(ptr_ - rhs.ptr_); }

        /// Test for equality with another iterator.
        bool operator ==(const RandomAccessConstIterator& rhs) const { return ptr_ == rhs.ptr_; }

        /// Test for inequality with another iterator.
        bool operator !=(const RandomAccessConstIterator& rhs) const { return ptr_ != rhs.ptr_; }

        /// Test for less than with another iterator.
        bool operator <(const RandomAccessConstIterator& rhs) const { return ptr_ < rhs.ptr_; }

        /// Test for greater than with another iterator.
        bool operator >(const RandomAccessConstIterator& rhs) const { return ptr_ > rhs.ptr_; }

        /// Test for less than or equal with another iterator.
        bool operator <=(const RandomAccessConstIterator& rhs) const { return ptr_ <= rhs.ptr_; }

        /// Test for greater than or equal with another iterator.
        bool operator >=(const RandomAccessConstIterator& rhs) const { return ptr_ >= rhs.ptr_; }

        T* ptr_;
    };
}


#endif //URHO3DCOPY_VECTORBASE_H
