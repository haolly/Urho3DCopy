//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_HASHMAP_H
#define URHO3DCOPY_HASHMAP_H

#include <initializer_list>
#include "HashBase.h"
#include "Hash.h"
#include "Pair.h"
#include "Vector.h"

namespace Urho3D
{
	template <class T, class U>
	class HashMap : public HashBase
	{
	public:
		using KeyType = T;
		using ValueType = U;
		class KeyValue
		{
		public:
			KeyValue() :
				first_(T())
			{

			}

			KeyValue(const T& first, const U& second) :
				first_(first),
				second_(second)
			{
			}

			KeyValue(const KeyValue& value) :
				first_(value.first_),
				second_(value.second_)
			{
			}

			bool operator ==(const KeyValue& rhs) const
			{
				return first_ == rhs.first_ && second_ == rhs.second_;
			}

			bool operator !=(const KeyValue& rhs) const
			{
				return first_ != rhs.first_ || second_ != rhs.second_;
			}

			const T first_;
			U second_;
		private:
			// Todo why?
			// Prevent assignment
			KeyValue&operator =(const KeyValue& rhs);
		};

		struct Node : public HashNodeBase
		{
			Node()
			{
			}

			Node(const T& key, const U& value) :
				pair_(key, value)
			{
			}

			KeyValue pair_;

			Node* Next() const { return static_cast<Node*>(next_); }
			Node* Prev() const { return static_cast<Node*>(prev_); }
			Node* Down() const { return static_cast<Node*>(down_); }
		};

		struct Iterator : public HashIteratorBase
		{
			Iterator()
			{
			}

			Iterator(Node* ptr) :
				HashIteratorBase(ptr)
			{
			}

			Iterator&operator ++()
			{
				GotoNext();
				return *this;
			}

			// Postincrement
			Iterator operator++(int)
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

			KeyValue*operator ->() const
			{
				return &(static_cast<Node*>(ptr_))->pair_;
			}

			KeyValue&operator *() const
			{
				return (static_cast<Node*>(ptr_))->pair_;
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

			ConstIterator operator++(int)
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

			//NOTE, return value is CONST
			const KeyValue*operator ->() const
			{
				return &((static_cast<Node*>(ptr_))->pair_);
			}

			const KeyValue operator *() const
			{
				return ((static_cast<Node*>(ptr_))->pair_);
			}
		};

	public:

		HashMap()
		{
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node));
			head_ = tail_ = ReserveNode();
		}

		HashMap(const HashMap<T, U>& map)
		{
			allocator_ = AllocatorInitialize((unsigned) sizeof(Node), map.Size() + 1);
			head_ = tail_ = ReserveNode();
			//call assignment operator
			*this = map;
		}


		HashMap(const std::initializer_list<Pair<T, U> >& list) : HashMap()
		{
			for(auto it = list.begin(); it != list.end(); ++it)
			{
				Insert(*it);
			}
		}


		~HashMap()
		{
			Clear();
			FreeNode(Tail());
			AllocatorUninitialize(allocator_);
			delete[] ptrs_;
		}

		HashMap&operator =(const HashMap<T, U>& rhs)
		{
			if(&rhs != this)
			{
				Clear();
				Insert(rhs);
			}
			return *this;
		}

		HashMap&operator +=(const Pair<T, U>& rhs)
		{
			Insert(rhs);
			return *this;
		}

		HashMap&operator +=(const HashMap<T, U>& rhs)
		{
			Insert(rhs);
			return *this;
		}

		bool operator ==(const HashMap<T, U>& rhs) const
		{
			if(Size() != rhs.Size())
				return false;
			ConstIterator it = Begin();
			while (it != End())
			{
				ConstIterator i = rhs.Find(it->first_);
				if(i == rhs.End() || i->second_ != it->second_)
					return false;
				++it;
			}
			return true;
		}

		bool operator !=(const HashMap<T, U>& rhs) const
		{
			if(Size() != rhs.Size())
				return true;

			ConstIterator it = Begin();
			while (it != End())
			{
				ConstIterator i = rhs.Find(it->first_);
				if(i == rhs.End() || i->second_ != it->second_)
					return true;
				++it;
			}
			return false;
		}

		// If key not exists in map, create it
		U&operator [](const T& key)
		{
			if(!ptrs_)
				return InsertNode(key, U(), false)->pair_.second_;

			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			return node ? node->pair_.second_ : InsertNode(key, U(), false)->pair_.second_;
		}

		// Retrun null if key is not found
		U* operator [](const T& key) const
		{
			if(!ptrs_)
				return nullptr;

			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			return node ? node->pair_.second_ : nullptr;
		}

		HashMap& Populate(const T& key, const U& value)
		{
			this->operator[](key) = value;
			return *this;
		}

		template <typename... Args>
		HashMap& Populate(const T& key, const U& value, Args... args)
		{
			this->operator[](key) = value;
			this->Populate(args...);
		}

		Iterator Insert(const Pair<T, U>& pair)
		{
			return Iterator(InsertNode(pair.first_, pair.second_));
		}



		//todo


		Iterator Find(const T& key)
		{
			if(!ptrs_)
				return End();
			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			if(node)
				return Iterator(node);
			else
				return End();
		}

		ConstIterator Find(const T& key) const
		{
			if(!ptrs_)
				return End();
			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			if(node)
				return ConstIterator(node);
			else
				return End();
		}

		bool Contains(const T& key) const
		{
			if(ptrs_)
				return false;
			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			return node != nullptr;
		}

		bool TryGetValue(const T& key, U& out) const
		{
			if(ptrs_)
				return false;
			unsigned hashKey = Hash(key);
			Node* node = FindNode(key, hashKey);
			if(node)
			{
				out = node->pair_.second_;
				return true;
			}
			return false;
		}

		Vector<T> Keys() const
		{
			Vector<T> ret;
			ret.Reserve(Size());
			for(ConstIterator it = Begin(); it != End(); ++it)
				ret.Push(it->first_);
			return ret;
		}

		Vector<U> Values() const
		{
			Vector<U> ret;
			//todo
		}

		void Clear()
		{
			ResetPtrs();
			if(Size())
			{
				for(Iterator it = Begin(); it != End(); ++it)
				{
					FreeNode(static_cast<Node*>(it.ptr_));
					it.ptr_->prev_ = nullptr;
				}
				head_ = tail_;
				SetSize(0);
			}
		}


		Iterator Begin() { return Iterator(Head()); }
		ConstIterator Begin() const { return ConstIterator(Head()); }
		Iterator End() { return Iterator(End()); }
		ConstIterator End() const { return ConstIterator(End()); }


	private:
		Node* Head() const
		{
			return static_cast<Node*>(head_);
		}

		Node* Tail() const
		{
			return static_cast<Node*>(tail_);
		}

		Node* FindNode(const T& key, unsigned hashKey) const
		{
			Node* node = static_cast<Node*>(Ptrs()[hashKey]);
			while (node)
			{
				if(node->pair_.first_ == key)
					return node;
				node = node->Down();
			}
			return nullptr;
		}

		Node* FindNode(const T& key, unsigned hashKey, Node*& previous) const
		{
			previous = nullptr;
			Node* node = static_cast<Node*>(Ptrs()[hashKey]);
			while (node)
			{
				if(node->pair_.first_ == key)
					return node;
				previous = node;
				node = node->Down();
			}
		}

		// Insert a key-value and return either the new or existing node
		Node* InsertNode(const T& key, const U& value, bool findExisting = true)
		{
			if(!ptrs_)
			{
				AllocateBuckets(Size(), MIN_BUCKETS);
				Rehash();
			}

			unsigned hashKey = Hash(key);

			if(findExisting)
			{
				// If exists, just change the value
				Node* existing = FindNode(key, hashKey);
				if(existing)
				{
					existing->pair_.second_ = value;
					return existing;
				}
			}

			// Chain the node with same hashValue into a list
			Node* newNode = InsertNode(Tail(), key, value);
			newNode->down_ = Ptrs()[hashKey];
			//put newNode in the head of the chain
			Ptrs()[hashKey] = newNode;

			if(Size() > NumBuckets() * MAX_LOAD_FACTOR)
			{
				AllocateBuckets(Size(), NumBuckets() << 1);
				Rehash();
			}
			return newNode;
		}

		Node* InsertNode(Node* dest, const T& key, const U& value)
		{
			if(!dest)
				return nullptr;

			Node* newNode = ReserveNode(key, value);
			Node* prev = dest->Prev();
			newNode->next_ = dest;
			newNode->prev_ = prev;
			if(prev)
				prev->next_ = newNode;
			dest->prev_ = newNode;

			if(dest == Head())
				head_ = newNode;

			SetSize(Size() + 1);
			return newNode;
		}

		Node* EraseNode(Node* node)
		{
			// Note, the tail node CAN NOT be removed
			if(!node || node == tail_)
				return Tail();

			Node* prev = node->Prev();
			Node* next = node->Next();
			if (prev)
				prev->next_ = next;
			if (next)
				next->prev_ = prev;

			if(node == Head())
				head_ = next;

			//todo, why do not need adjust down_ pointer ??
			// if node is the first node at the same hash value (say hashValue) list,
			// the Ptrs()[hashValue] will pointer to invalid address
			FreeNode(node);
			SetSize(Size() -1);
			return next;
		}

		Node* ReserveNode()
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node();
			return newNode;
		}

		Node* ReserveNode(const T& key, const U& value)
		{
			Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
			new(newNode) Node(key, value);
			return newNode;
		}

		void FreeNode(Node* node)
		{
			(node)->~Node();
			AllocatorFree(allocator_, node);
		}

		void Rehash()
		{
			for (Iterator i = Begin(); i != End(); ++i)
			{
				Node* node = static_cast<Node*>(i.ptr_);
				unsigned hashKey = Hash(i->first_);
				node->down_ = Ptrs()[hashKey];
				Ptrs()[hashKey] = node;
			}
		}

		static bool CompareNodes(Node* lhs, Node* rhs) const
		{
			return lhs->pair_ < rhs->pair_.first_;
		}

		unsigned Hash(const T& key) const
		{
			//Note, hash value will NERVE exceed the buckets num
			return MakeHash(key) & (NumBuckets() -1);
		}
	};
}



#endif //URHO3DCOPY_HASHMAP_H
