//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_LINKEDLIST_H
#define URHO3DCOPY_LINKEDLIST_H

#include <initializer_list>

namespace Urho3D
{
	struct LinkedListNode
	{
		LinkedListNode() :
			next_(nullptr)
		{
		}

		LinkedListNode* next_;
	};

	/// Singly-linked list, Elements must inhert from LinkedListNode
	/// \tparam T
	template <class T>
	class LinkedList
	{
	public:
		LinkedList() : head_(0)
		{
		}

		// Note, call default constructor first
		LinkedList(const std::initializer_list<T>& list) : LinkedList()
		{
			for (auto it = list.begin(); it != list.end(); it++)
			{
				Insert(*it);
			}
		}

		~LinkedList()
		{
			Clear();
		}

		void Clear()
		{
			T* element = head_;
			while(element)
			{
				T* next = Next(element);
				delete element;
				element = next;
			}
		}

		void InsertFront(T* element)
		{
			if(element)
			{
				element->next_ = head_;
				head_ = element;
			}
		}

		void Insert(T* element)
		{
			if(head_)
			{
				T* tail = Last();
				tail->next = element;
			}
			else
			{
				head_ = element;
			}
		}

		bool Erase(T* element)
		{
			if(element == head_)
			{
				head_ = Next(head_);
				delete element;
				return true;
			}
			T* tail = head_;
			while (tail && tail->next_ != element)
				tail = Next(tail);
			if(tail)
			{
				tail->next = element ->next_;
				delete element;
				return true;
			}
			return false;
		}

		//erase an element when the previous element is know (optimization),
		bool Erase(T* element, T* previous)
		{
			if(previous && previous->next_ == element)
			{
				previous->next_ = element->next_;
				delete element;
				return true;
			}
			else if(!previous)
			{
				if(element == head_)
				{
					head_ = Next(head_);
					delete element;
					return true;
				}
			}
			return false;
		}

		T* First() const {return head_;}
		T* Last() const
		{
			T* element = head_;
			if(element)
			{
				while(element->next_)
				{
					element = Next(element);
				}
			}
			return element;
		}

		T* Next(T* element) const { return element ? static_cast<T*>(element->next_) : 0;}
		bool Empty() const { return head_ == 0;}

	private:
		T* head_;
	};
}


#endif //URHO3DCOPY_LINKEDLIST_H
