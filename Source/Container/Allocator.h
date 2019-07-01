//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_ALLOCATOR_H
#define URHO3DCOPY_ALLOCATOR_H

namespace Urho3D
{
	struct AllocatorNode;

	struct AllocatorBlock
	{
		unsigned nodeSize_;
		unsigned capacity_;
		AllocatorNode* free_;
		AllocatorBlock* next_;
	};

	struct AllocatorNode
	{
		AllocatorNode* next_;
	};


	AllocatorBlock* AllocatorInitialize(unsigned nodeSize, unsigned initialCapacity = 1);
	void AllocatorUninitialize(AllocatorBlock* allocator);
	void* AllocatorReserve(AllocatorBlock* allocator);
	void AllocatorFree(AllocatorBlock* allocator, void* ptr);

	/// Allocator which can allocate Object T
	/// \tparam T
	template <class T>
	class Allocator
	{
	public:
		Allocator(unsigned initialCapacity = 0) :
			allocator_(nullptr)
		{
			if(initialCapacity)
				allocator_ = AllocatorInitialize((unsigned) sizeof(T), initialCapacity);
		}

		~Allocator()
		{
			AllocatorUninitialize(allocator_);
		}

		//todo, usagea
		T* Reserve(const T& object)
		{
			if(!allocator_)
			{
				allocator_ = AllocatorInitialize((unsigned)(sizeof(T)));
			}
			T* newObject = static_cast<T*>(AllocatorReserve(allocator_));
			new(newObject) T(object);
			return newObject;
		}

		void Free(T* object)
		{
			(object) ->~T();
			AllocatorFree(allocator_, object);
		}

	private:
		//prevent copy constructor
		Allocator(const Allocator<T>& rhs);
		Allocator<T>&operator =(const Allocator<T>& rhs);
		AllocatorBlock* allocator_;
	};
}


#endif //URHO3DCOPY_ALLOCATOR_H
