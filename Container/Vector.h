//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_VECTOR_H
#define URHO3DCOPY_VECTOR_H

#include <cstring>
#include <cassert>
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
		//todo

		T& At(unsigned index)
		{
			assert(index < size_);
			return Buffer()[index];
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
