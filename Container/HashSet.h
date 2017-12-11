//
// Created by LIUHAO on 2017/12/4.
//

#ifndef URHO3DCOPY_HASHSET_H
#define URHO3DCOPY_HASHSET_H

#include <initializer_list>
#include "HashBase.h"
#include "Hash.h"

namespace Urho3D
{
	template <class T>
	class HashSet : public HashBase
	{
	public:
		struct Node : public HashNodeBase
		{
			Node()
			{
			}

			Node(const T& key) :
				key_(key)
			{

			}

			T key_;

			Node* Next() const { return static_cast<Node*>(next_); }
			Node* Prev() const { return static_cast<Node*>(prev_); }
			Node* Down() const { return static_cast<Node*>(down_); }
		};

		struct Iterator : public HashIteratorBase
		{
			Iterator()
			{}

			Iterator(Node* ptr) :
				HashIteratorBase(ptr)
			{

			}

			Iterator&operator ++()
			{
				GotoNext();
				return *this;
			}

			//Note, postincrement DO NOT return REFERENCE
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

			const T*operator ->() const
			{
				return &(static_cast<Node*>(ptr_))->key_;
			}

			const T&operator *() const
			{
				return (static_cast<Node*>(ptr_))->key_;
			}
		};

		struct ConstIterator : public HashIteratorBase
		{
			ConstIterator()
			{
			}

			ConstIterator(Node* ptr) :
				HashIteratorBase(ptr)
			{
			}

			ConstIterator(const Iterator& rhs) :
				HashIteratorBase(rhs.ptr_)
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

			ConstIterator& operator --()
			{
				GotoPrev();
				return *this;
			}

			ConstIterator&operator --(int)
			{
				ConstIterator it = *this;
				GotoPrev();
				return it;
			}

			const T*operator ->() const
			{
				return &(static_cast<Node*>(ptr_) ->key_);
			}

			const T&operator *() const
			{
				return static_cast<Node*>(ptr_) ->key_;
			}
		};

		HashSet()
		{
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node));
			head_ = tail_ = ReserveNode();
		}

		HashSet(const HashSet<T>& set)
		{
			// Reserve the tail node + initial capacity according to the set's size
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node), set.Size() + 1);
			head_ = tail_ = ReserveNode();
			*this = set;
		}

		//Note , call default constructor first
		HashSet(const std::initializer_list<T>& list) : HashSet()
		{
			for(auto it = list.begin(); it != list.end(); ++it)
			{
				Insert(*it);
			}
		}

		~HashSet()
		{
			Clear();
			//todo
		}

		void Clear()
		{
			ResetPtrs();
			if(Size())
			{
				for(Iterator i = Begin(); i != End())
				{
					FreeNode(static_cast<Node*>(i.ptr_));
					++i;
					i.ptr_->prev_ = 0;
				}
				head_ = tail_;
				SetSize(0);
			}
		}

		bool Contains(const T& key) const
		{
			if(!ptrs_)
				return false;
			unsigned hashKey = Hash(key);
			return FindNode(key, hashKey) != nullptr;
		}

		Iterator Begin() { return Iterator(Head()); }
		ConstIterator Begin() const { return ConstIterator(Head()); }

		Iterator End() { return Iterator(End()); }
		ConstIterator End() const { return ConstIterator(End()); }

		const T& Front() const { return *Begin(); }
		const T& Back() const { return *(--End()); }

	private:
		Node* Head() const { return static_cast<Node*>(head_); }
		Node* Tail() const { return static_cast<Node*>(tail_); }

		Node* FindNode(const T& key, unsigned hashKey) const
		{
			Node* node = static_cast<Node*>(Ptrs()[hashKey]);
			while (node)
			{
				if(node->key_ == key)
					return node;
				node = node->Down();
			}
			return 0;
		}

		Node* FindNode(const T& key, unsigned hashKey, Node*& previous) const
		{
			previous = 0;

			Node* node = static_cast<Node*>(Ptrs()[hashKey]);
			while (node)
			{
				if(node->key_ == key)
					return node;
				previous = node;
				node = node->Down();
			}
			return 0;
		}

		Node* InsertNode(Node* dest, const T& key)
		{
			if(!dest)
				return 0;

			Node* newNode = ReserveNode(key);
			Node* prev = dest->Prev();
			if(prev)
				prev->next_ = newNode;
			newNode->prev_ = prev;
			newNode->next_ = dest;
			dest->prev_ = newNode;

			if(dest == Head())
				head_ = newNode;

			SetSize(Size() + 1);
			return newNode;
		}

		Node* EraseNode(Node* node)
		{
			if(!node || node == tail_)
				return Tail();

			Node* prev = node->Prev();
			Node* next = node->Next();

			if(prev)
				prev->next_ = node->next_;
			if(next)
				next->prev_ = prev;

			if(node == Head())
				head_ = next;

			FreeNode(node);
			SetSize(Size() - 1);
			return next;
		}

		Node* ReserveNode()
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node();
			return newNode;
		}

		Node* ReserveNode(const T& key)
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node(key);
			return newNode;
		}

		void FreeNode(Node* node)
		{
			(node)->~Node();
			AllocatorFree(allocator_, node);
		}

		void Rehash()
		{
			for(Iterator it = Begin(); it != End(); ++it)
			{
				Node* node = static_cast<Node*>(it.ptr_);
				unsigned hashKey = Hash(*it);
				node->down_ = Ptrs()[hashKey];
				Ptrs()[hashKey] = node;
			}
		}

		static bool CompareNodes(Node*& lhs, Node*& rhs)
		{
			return lhs->key_ < rhs->key_;
		}

		unsigned Hash(const T& key) const
		{
			return MakeHash(key) & (NumBuckets() - 1);
		}
	};

	//todo

}



#endif //URHO3DCOPY_HASHSET_H
