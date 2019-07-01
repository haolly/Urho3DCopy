//
// Created by LIUHAO on 2017/12/11.
//

#ifndef URHO3DCOPY_LIST_H
#define URHO3DCOPY_LIST_H

#include "ListBase.h"

namespace Urho3D
{
	// Doubly linked list
	template <class T>
	class List : public ListBase
	{
	public:
		// Note, need to be POD types ?
		// ref https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special
		// Note, this is NOT a POD type, there is user defined constructor, and is not aggregates
		// ref http://en.cppreference.com/w/cpp/concept/PODType
		// ref http://en.cppreference.com/w/cpp/language/aggregate_initialization #class type (typically, struct or union), that has:
		struct Node : public ListNodeBase
		{
			Node()
			{
			}

			Node(const T& value) :
				value_(value)
			{

			}

			Node* Next() const { return static_cast<Node*>(next_); }
			Node* Prev() const { return static_cast<Node*>(prev_); }

			T value_;
		};

		struct Iterator : public ListIteratorBase
		{
			Iterator()
			{

			}

			explicit Iterator(Node* ptr) :
					ListIteratorBase(ptr)
			{
			}

			Iterator&operator ++()
			{
				GotoNext();
				return *this;
			}

			Iterator operator ++(int)
			{
				Iterator it = *this;
				GotoNext();
				return it;
			}

			Iterator&operator --()
			{
				GotoPrev();
				return *this;
			}

			Iterator operator --(int)
			{
				Iterator it = *this;
				GotoPrev();
				return it;
			}

			T*operator ->() const
			{
				return &(static_cast<Node*>(ptr_)->value_);
			}

			T& operator *() const
			{
				return (static_cast<Node*>(ptr_)->value_);
			}
		};

		struct ConstIterator : public ListIteratorBase
		{
			ConstIterator()
			{

			}

			explicit ConstIterator(Node* ptr) :
					ListIteratorBase(ptr)
			{
			}

			ConstIterator(const Iterator& rhs) :
				ListIteratorBase(rhs.ptr_)
			{
			}

			ConstIterator&operator =(const Iterator& rhs)
			{
				ptr_ = rhs.ptr_;
				return *this;
			}

			ConstIterator&operator ++()
			{
				GotoNext();
				return *this;
			}

			ConstIterator operator ++(int)
			{
				ConstIterator it = *this;
				GotoNext();
				return it;
			}

			ConstIterator&operator --()
			{
				GotoPrev();
				return *this;
			}

			ConstIterator operator --(int)
			{
				ConstIterator it = *this;
				GotoPrev();
				return it;
			}

			const T*operator ->() const
			{
				return &(static_cast<Node*>(ptr_)->value_);
			}

			const T& operator *() const
			{
				return (static_cast<Node *>(ptr_)->value_);
			}
		};

		List()
		{
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node));
			head_ = tail_ = ReserveNode();
		}

		List(const List<T>& list)
		{
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node), list.Size() + 1);
			head_ = tail_ = ReserveNode();
			//call copy assignment
			*this = list;
		}

		List(const std::initializer_list<T>& list) : List()
		{
			for(auto it = list.begin(); it != list.end(); ++it)
			{
				Push(*it);
			}
		}

		~List()
		{
			Clear();
			FreeNode(Tail());
			AllocatorUninitialize(allocator_);
		}

		List&operator =(const List<T>& rhs)
		{
			if(&rhs != this)
			{
				Clear();
				Insert(End(), rhs);
			}
			return *this;
		}

		List&operator +=(const T& rhs)
		{
			Push(rhs);
			return *this;
		}

		List&operator +=(const List<T>& rhs)
		{
			Insert(End(), rhs);
			return *this;
		}

		bool operator ==(const List<T>& rhs) const
		{
			if(rhs.size_ != size_)
				return false;
			ConstIterator i = Begin();
			ConstIterator j = rhs.Begin();
			while(i != End())
			{
				if(*i != *j)
					return false;
				++i;
				++j;
			}
			return true;
		}

		bool operator !=(const List<T>& rhs) const
		{
			if(rhs.size_ != size_)
				return true;
			ConstIterator i = Begin();
			ConstIterator j = rhs.Begin();
			while(i != End())
			{
				if(*i != *j)
					return true;
				++i;
				++j;
			}
			return false;
		}


		void Push(const T& value)
		{
			InsertNode(Tail(), value);
		}

		void PushFront(const T& value)
		{
			InsertNode(Head(), value);
		}

		void Insert(const Iterator& dest, const T& value)
		{
			InsertNode(static_cast<Node*>(dest.ptr_), value);
		}

		void Insert(const Iterator& dest, const List<T>& list)
		{
			Node* destNode = static_cast<Node*>(dest.ptr_);
			ConstIterator it = list.Begin();
			ConstIterator end = list.End();
			while(it != end)
				InsertNode(destNode, *it++);
		}

		void Insert(const Iterator& dest, const ConstIterator& start, const ConstIterator& end)
		{
			Node* destNode = static_cast<Node*>(dest.ptr_);
			ConstIterator it = start;
			while(it != end)
				InsertNode(destNode, *it++);
		}

		void Insert(const Iterator& dest, const T* start, const T* end)
		{
			Node* destNode = static_cast<Node*>(dest.ptr_);
			const T* ptr = start;
			while(ptr != end)
				InsertNode(destNode, *ptr++);
		}

		void Pop()
		{
			if(size_)
				Erase(--End());
		}

		void PopFront()
		{
			if(size_)
				Erase(Begin());
		}

		Iterator Erase(Iterator it)
		{
			return Iterator(EraseNode(static_cast<Node*>(it.ptr_)));
		}


		void Clear()
		{
			if(Size())
			{
				for(Iterator it = Begin(); it != End())
				{
					FreeNode(static_cast<Node*>(it.ptr_));
					it.ptr_->prev_ = 0;
					++it;
				}
				head_ = tail_;
				size_ = 0;
			}
		}

		void Resize(unsigned newSize)
		{
			while(size_ > newSize)
				Pop();

			while(size_ < newSize)
				InsertNode(Tail(), T());
		}

		Iterator Find(const T& value)
		{
			Iterator it = Begin();
			while(it != End() && *it != value)
				++it;
			return it;
		}

		ConstIterator Find(const T& value) const
		{
			ConstIterator it = Begin();
			while(it != End() && *it != value)
				++it;
			return it;
		}

		bool Contains(const T& value) const
		{
			return Find(value) != End();
		}

		Iterator Begin() { return Iterator(Head()); }
		ConstIterator Begin() const { return ConstIterator(Head()); }
		Iterator End() { return Iterator(Tail()); }
		ConstIterator End() const { return ConstIterator(Tail()); }

		T& Front() { return *Begin(); }
		const T& Front() const { return *Begin(); }

		T& Back() { return *End(); }
		T& Back() const { return *End(); }

		unsigned Size() const { return size_; }

		bool Empty() const { return size_ == 0; }
	private:
		Node* Head() const { return static_cast<Node*>(head_); }
		Node* Tail() const { return static_cast<Node*>(tail_); }

		void InsertNode(Node* dest, const T& value)
		{
			if(!dest)
				return;
			Node* newNode = ReserveNode(value);
			Node* prev = dest->Prev();
			if(prev)
				prev->next_ = newNode;
			newNode->prev_ = prev;
			newNode->next_ = dest;
			dest->prev_ = newNode;

			if(dest == head_)
			{
				head_ = newNode;
			}
			++size_;
		}

		Node* EraseNode(Node* node)
		{
			if(!node || node == tail_)
				return Tail();
			Node* prev = node->Prev();
			Node* next = node->Next();
			if(prev)
				prev->next_ = next;
			if(next)
				next->prev_ = prev;

			FreeNode(node);
			if(node == head_)
				head_ = next;
			--size_;
			return next;
		}

		Node* ReserveNode()
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node();
			return newNode;
		}

		Node* ReserveNode(const T& value)
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node(value);
			return newNode;
		}

		void FreeNode(Node* node)
		{
			(node)->~Node();
			AllocatorFree(allocator_, node);
		}
	};
}

#endif //URHO3DCOPY_LIST_H
