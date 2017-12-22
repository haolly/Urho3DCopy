//
// Created by LIUHAO on 2017/12/4.
//

#ifndef URHO3DCOPY_HASHSET_H
#define URHO3DCOPY_HASHSET_H

#include <initializer_list>
#include <cassert>
#include "HashBase.h"
#include "Hash.h"
#include "VectorBase.h"

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
			FreeNode(Tail());
			AllocatorUninitialize(allocator_);
			delete [] ptrs_;
		}

		HashSet&operator =(const HashSet<T>& rhs)
		{
			if(&rhs != this)
			{
				Clear();
				Insert(rhs);
			}
			return *this;
		}

		HashSet&operator +=(const T& rhs)
		{
			Insert(rhs);
			return *this;
		}

		HashSet&operator +=(const HashSet<T>& rhs)
		{
			Insert(rhs);
			return *this;
		}

		bool operator ==(const HashSet<T>& rhs) const
		{
			if(rhs.Size() != Size())
				return false;
			auto it = Begin();
			while (it != End())
			{
				if(!rhs.Contains(*it))
					return false;
			}
			return true;
		}

		bool operator != (const HashSet<T>& rhs) const
		{
			if(rhs.Size() == Size())
				return false;
			auto it = Begin();
			while(it != End())
			{
				if(!rhs.Contains(*it))
					return true;
			}
			return false;
		}

		Iterator Insert(const T& key)
		{
			if(!ptrs_)
			{
				AllocateBuckets(Size(), MIN_BUCKETS);
				Rehash();
			}

			unsigned hashKey = Hash(key);
			Node* existing = FindNode(key, hashKey);
			if(existing)
				return Iterator(existing);

			Node* newNode = InsertNode(Tail(), key);
			newNode->down_ = Ptrs()[hashKey];
			Ptrs()[hashKey] = newNode;

			if(Size() > NumBuckets() * MAX_LOAD_FACTOR)
			{
				AllocateBuckets(Size(), NumBuckets() << 1);
				Rehash();
			}
			return Iterator(newNode);
		}

		Iterator Insert(const T& key, bool& exists)
		{
			unsigned oldSize = Size();
			Iterator ret = Insert(key);
			exists = (Size() == oldSize);
			return ret;
		}

		void Insert(const HashSet<T>& set)
		{
			ConstIterator it = set.Begin();
			ConstIterator end = set.End();
			while (it != end)
				Insert(*it++);
		}

		Iterator Insert(const ConstIterator& it)
		{
			return Iterator(Insert(*it));
		}

		//Erase in the down_ chain-list, not the pre/next chain-list
		bool Erase(const T& key)
		{
			if(!ptrs_)
				return false;

			unsigned hashKey = Hash(key);
			Node* previous;
			Node* existing = FindNode(key, hashKey, previous);
			if(!existing)
				return false;
			if(previous)
				previous->down_ = existing->down_;
			else
				Ptrs()[hashKey] = existing->down_;

			EraseNode(existing);
			return true;
		}

		Iterator Erase(const Iterator& it)
		{
			if(!ptrs_ || !it.ptr_)
				return End();

			Node* node = static_cast<Node*>(it.ptr_);
			Node* next = node->Next();

			// Erase in down chain-list
			unsigned hashKey = Hash(node->key_);
			Node* previous = nullptr;
			Node* current = static_cast<Node *>(Ptrs()[hashKey]);
			while (current && current != node)
			{
				previous = current;
				current = current->Down();
			}

			assert(current == node);
			if(previous)
				previous->down_ = node->down_;
			else
				Ptrs()[hashKey] = node->down_;

			EraseNode(node);
			return Iterator(next);
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

		void Sort()
		{
			unsigned numKeys = Size();
			if(!numKeys)
				return;

			Node** ptrs = new Node*[numKeys];
			Node* ptr = Head();

			for(unsigned i=0; i< numKeys; ++i)
			{
				ptrs[i] = ptr;
				ptr = ptr->Next();
			}

			Urho3D::Sort(RandomAccessIterator<Node*>(ptrs), RandomAccessIterator<Node*>(ptrs + numKeys), CompareNodes);

			head_ = ptrs[0];
			ptrs[0]->prev_ = nullptr;
			for(unsigned i=1; i< numKeys; ++i)
			{
				ptrs[i - 1]->next_ = ptrs[i];
				ptrs[i]->prev_ = ptrs[i -1];
			}

			//Note tail node is special
			ptrs[numKeys - 1]->next_ = tail_;
			tail_->prev_ = ptrs[numKeys -1];
			delete [] ptrs;
		}

		bool ReHash(unsigned numBuckets)
		{
			if(numBuckets == NumBuckets())
				return true;
			if(!numBuckets || numBuckets < Size() / MAX_LOAD_FACTOR)
				return false;

			// Muste be a power of 2
			unsigned check = numBuckets;
			while (!(check & 1))
				check >> 1;

			if(check != 1)
				return false;

			AllocateBuckets(Size(), numBuckets);
			Rehash();
			return true;
		}


		Iterator Find(const T& key)
		{
			if(!ptrs_)
				return End();
			unsigned keyHash = Hash(key);
			Node* node = FindNode(key, keyHash);
			if(node)
				return Iterator(node);
			else
				return End();
		}

		ConstIterator Find(const T& key) const
		{
			if(!ptrs_)
				return End();
			unsigned keyHash = Hash(key);
			Node* node = FindNode(key, keyHash);
			if(node)
				return ConstIterator(node);
			else
				return End();
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

		//Note, find in the down chain-list
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

		//Note, Insert Node at Prev/Next chain-list, NOT down chain list
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

		//Note ,Erase node in the prev/next chain-list, not the down chain-list
		Node* EraseNode(Node* node)
		{
			//Note tail node is special
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

	//todo, what is the use of keyword typename??
	template <class T>
	typename HashSet<T>::ConstIterator begin(const HashSet<T>& v)
	{
		return v.Begin();
	}
}



#endif //URHO3DCOPY_HASHSET_H
