//
// Created by LIUHAO on 2017/12/11.
//

#ifndef URHO3DCOPY_LISTBASE_H
#define URHO3DCOPY_LISTBASE_H
namespace Urho3D
{
	struct ListNodeBase
	{
		ListNodeBase() :
			prev_(nullptr),
			next_(nullptr)
		{
		}
		ListNodeBase* prev_;
		ListNodeBase* next_;
	};

	struct ListIteratorBase
	{
		ListIteratorBase() :
			ptr_(nullptr)
		{

		}

		explicit ListIteratorBase(ListNodeBase* ptr) :
			ptr_(ptr)
		{
		}

		bool operator ==(const ListIteratorBase& rhs) { return ptr_ == rhs.ptr_; }
		bool operator !=(const ListIteratorBase& rhs) { return ptr_ != rhs.ptr_; }

		void GotoNext()
		{
			if(ptr_)
				ptr_ = ptr_->next_;
		}

		void GotoPrev()
		{
			if(ptr_)
				ptr_ = ptr_->prev_;
		}

		ListNodeBase* ptr_;
	};

	class ListBase
	{
	public:
		ListBase() :
			head_(nullptr),
			tail_(nullptr),
			allocator_(nullptr),
			size_(0)
		{

		}

		void Swap(ListBase& rhs)
		{
			// Note, specialized template function
			Urho3D::Swap(head_, rhs.head_);
			Urho3D::Swap(tail_, rhs.tail_);
			Urho3D::Swap(allocator_, rhs.allocator_);
			Urho3D::Swap(size_, rhs.size_);
		}

	protected:
		ListNodeBase* head_;
		ListNodeBase* tail_;
		AllocatorBlock* allocator_;
		unsigned size_;
	};
}

#endif //URHO3DCOPY_LISTBASE_H
