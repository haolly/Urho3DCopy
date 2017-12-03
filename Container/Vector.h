//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_VECTOR_H
#define URHO3DCOPY_VECTOR_H

#include <cstring>
#include <cassert>
#include <initializer_list>
#include "VectorBase.h"

namespace Urho3D
{
	template <class T>
	class Vector : public VectorBase
	{
		//todo
	};


	/**
	 * Todo, the origin comments has notice: Does not call constructors or destructors and use block move.
	 * Is intentionally (for performance reasons) unsafe for self-insertion
	 * @tparam T
	 */
	template <class T>
	class PODVector : public VectorBase
	{
	public:
		using ValueType = T;
		using Iterator = RandomAccessIterator<T>;
		using ConstIterator = RandomAccessConstIterator<T>;

		PODVector()
		{
		}

		explicit PODVector(unsigned size)
		{
			Resize(size);
		}

		PODVector(unsigned size, const T& value)
		{
			Resize(size);
			for (int i = 0; i < size; ++i) {
				At(i) = value;
			}
		}

		PODVector(const T* data, unsigned size)
		{
			Resize(size);
			CopyElements(Buffer(), data, size);
		}

		PODVector(const PODVector<T>& vector)
		{
			*this = vector;
		}

		//Todo, : PODVector() is what ?
		PODVector(const std::initializer_list<T>& list) : PODVector()
		{
			for (auto it = list.begin(); it != list.end(); ++it)
			{
				Push(*it);
			}
		}

		~PODVector()
		{
			delete [] buffer_;
		}

		PODVector<T>&operator =(const PODVector<T>& rhs)
		{
			if(&rhs != this)
			{
				Resize(rhs.size_);
				CopyElements(buffer_, rhs.buffer_, rhs.size_);
			}
			return *this;
		}

		PODVector<T>&operator +=(const T& rhs)
		{
			Push(rhs);
			return *this;
		}

		PODVector<T> operator +(const T& rhs) const
		{
			PODVector<T> ret(*this);
			ret.Push(rhs);
			return ret;
		}

		PODVector<T> operator+(const PODVector<T>& rhs) const
		{
			PODVector<T> ret(*this);
			ret.Push(rhs);
			return ret;
		}
		
		bool operator ==(const PODVector<T>& rhs) const 
		{
			if(rhs.size_ != size_)
				return false;
			T* buffer = Buffer();
			T* rhsBuffer = rhs.Buffer();
			for (int i = 0; i < size_; ++i) {
				if(buffer[i] != rhsBuffer[i])
					return false;
			}
			return true;
		}

		bool operator !=(const PODVector<T>& rhs) const
		{
			if(rhs.size_ != size_)
				return true;

			T* buffer = Buffer();
			T* rhsBuffer = rhs.Buffer();
			for (int i = 0; i < size_; ++i) {
				if(buffer[i] != rhsBuffer[i])
					return true;
			}
			return false;
		}

		T&operator [](unsigned index)
		{
			assert(index < size_);
			return Buffer()[index];
		}

		const T&operator [](unsigned index) const
		{
			assert(index < size_);
			return Buffer()[index];
		}

		T& At(unsigned index)
		{
			assert(index < size_);
			return Buffer()[index];
		}

		const T& At(unsigned index) const
		{
			assert(index < size_);
			return Buffer()[index];
		}

		void Push(const T& value)
		{
			if(size_ < capacity_)
				size_++;
			else
				Resize(size_ + 1);
			Back() = value;
		}

		void Push(const PODVector<T>& rhs)
		{
			unsigned oldSize = size_;
			Resize(size_ + rhs.size_);
			CopyElements(buffer_ + oldSize, rhs.buffer_, rhs.size_);
		}

		void Pop()
		{
			if(size_)
				Resize(size_ - 1);
		}

		void Insert(unsigned pos, const T& value)
		{
			if(pos > size_)
				pos = size_;
			unsigned oldSize = size_;
			Resize(size_ + 1);
			MoveRange(pos + 1, pos, oldSize - pos);
			Buffer()[pos] = value;
		}

		void Insert(unsigned pos, const PODVector<T>& vector)
		{
			if(pos > size_)
				pos = size_;
			unsigned oldSize = size_;
			Resize(size_ + vector.size_);
			MoveRange(pos + vector.size_, pos, oldSize - pos);
			CopyElements(buffer_ + pos, vector.buffer_, vector.size_);
		}

		Iterator Insert(const Iterator& dest, const T& value)
		{
			unsigned pos =(unsigned)(Begin() - dest);
			if(pos > size_)
				pos = size_;
			Insert(pos, value);
			return Begin() + pos;
		}

		Iterator Insert(const Iterator& dest, const ConstIterator& start, const ConstIterator& end)
		{
			unsigned pos = (unsigned) (dest - Begin());
			if(pos > size_)
				pos = size_;
			unsigned length = (unsigned)(end - start);
			Resize(size_ + length);
			//todo
		}


		void Clear()
		{
			Resize(0);
		}

		void Resize(unsigned newSize)
		{
			if(newSize > capacity_)
			{
				if(!capacity_)
					capacity_ = newSize;
				else
				{
					while(capacity_ < newSize)
					{
						capacity_ += (capacity_ + 1) >> 1;
					}
					byte* newBuffer = AllocateBuffer((unsigned)(capacity_ * sizeof(T)));
					if(buffer_)
					{
						CopyElements(reinterpret_cast<T*>(newBuffer), Buffer(), size_);
						//Note, AllocateBuffer use new[], so use delete[] here
						delete [] buffer_;
					}
					buffer_ = newBuffer;
				}
			}
			size_ = newSize;
		}

		void Reserve(unsigned newCapacity)
		{
			if(newCapacity < size_)
				newCapacity = size_;

			if(newCapacity != capacity_)
			{
				byte* newBuffer = nullptr;
				capacity_ = newCapacity;
				if(capacity_)
				{
					newBuffer = AllocateBuffer((unsigned)capacity_ * sizeof(T));
					CopyElements(newBuffer, buffer_, size_);
				}
			}
		}


		Iterator Begin() { return Iterator(Buffer()); }

		ConstIterator Begin() const
		{
			return ConstIterator(Buffer());
		}

		Iterator End() { return Iterator(Buffer() + size_); }

		ConstIterator End() const
		{
			return ConstIterator(Buffer() + size_);
		}

		T& Front() { return Buffer()[0]; }
		const T& Front() const { return Buffer()[0]; }

		T& Back()
		{
			assert(size_);
			return Buffer()[size_ -1];
		}

		const T& Back() const
		{
			assert(size_);
			return Buffer()[size_ - 1];
		}

		unsigned Size() const { return size_; }

		unsigned Capacity() const { return capacity_; }

		bool Empty() const { return size_ == 0; }

		T* Buffer() const
		{
			return reinterpret_cast<T*>(buffer_);
		}

	private:
		//Note , dest and src CAN OVERLAP in memmove
		void MoveRange(unsigned dest, unsigned src, unsigned count)
		{
			if(count)
				memmove(Buffer() + dest, Buffer() + src, count * sizeof(T));
		}

		//Note , dest and src CAN NOT OVERLAP in memcpy, otherwise the behavior is undefined !!
		static void CopyElements(T* dest, const T* src, unsigned count)
		{
			if(count)
				memcpy(dest, src, count * sizeof(T));
		}
	};

}



#endif //URHO3DCOPY_VECTOR_H
