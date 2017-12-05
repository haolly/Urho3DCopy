//
// Created by LIUHAO on 2017/12/5.
//

#include "Allocator.h"
#include "VectorBase.h"

namespace Urho3D
{

	AllocatorBlock* AllocatorReserveBlock(AllocatorBlock* allocator, unsigned nodeSize, unsigned capacity)
	{
		if(!capacity)
			capacity = 1;
		//Note, one AllocatorBlock is actually a byte array!!!
		byte* blockPtr = new unsigned char[sizeof(AllocatorBlock) + capacity * (sizeof(AllocatorNode) + nodeSize)];
		AllocatorBlock* newBlock = reinterpret_cast<AllocatorBlock*>(blockPtr);
		newBlock->nodeSize_ = nodeSize;
		newBlock->capacity_ = capacity;
		newBlock->free_ = nullptr;
		newBlock->next_ = nullptr;

		if(!allocator)
			allocator = newBlock;
		else
		{
			newBlock->next_ = allocator->next_;
			allocator->next_ = newBlock;
		}

		//Initialize the nodes
		byte* nodePtr = blockPtr + sizeof(AllocatorBlock);
		AllocatorNode* firstNewNode = reinterpret_cast<AllocatorNode*>(nodePtr);

		for(unsigned i = 0; i < capacity -1; ++i)
		{
			AllocatorNode* newNode = reinterpret_cast<AllocatorNode*>(nodePtr);
			newNode->next_ = reinterpret_cast<AllocatorNode*>(nodePtr + sizeof(AllocatorNode) + nodeSize);
			nodePtr += sizeof(AllocatorNode) + nodeSize;
		}

		// i == capacity - 1, the last node
		{
			AllocatorNode* newNode = reinterpret_cast<AllocatorNode*>(nodePtr);
			newNode->next_ = nullptr;
		}
		//Note, free nodes are always chained to the first (parent) allocator
		allocator->free_ = firstNewNode;

		return newBlock;
	}

	AllocatorBlock* AllocatorInitialize(unsigned nodeSize, unsigned initialCapacity)
	{
		AllocatorBlock* block = AllocatorReserveBlock(nullptr, nodeSize, initialCapacity);
		return block;
	}

	void AllocatorUninitialize(AllocatorBlock* allocator)
	{
		while (allocator)
		{
			AllocatorBlock* next = allocator->next_;
			delete [] reinterpret_cast<byte*>(allocator);
			allocator = next;
		}
	}

	/// Allocate an object with the allocatorBlock
	/// \param allocator
	/// \return
	void* AllocatorReserve(AllocatorBlock* allocator)
	{
		if(!allocator)
			return nullptr;

		if(!allocator->free_)
		{
			//Free node have been exhausted, Allocate a new large block
			unsigned newCapacity = (allocator->capacity_ + 1) >> 1;
			AllocatorReserveBlock(allocator, allocator->nodeSize_, newCapacity);
			allocator->capacity_ += newCapacity;
		}

		AllocatorNode* freeNode = allocator->free_;
		//left size = nodeSize
		void* ptr = (reinterpret_cast<byte*>(freeNode)) + sizeof(AllocatorNode);
		allocator->free_ = freeNode->next_;
		freeNode->next_ = nullptr;
		return ptr;
	}

	void AllocatorFree(AllocatorBlock* allocator, void* ptr)
	{
		if(!allocator || !ptr)
			return;

		byte* dataPtr = static_cast<byte *>(ptr);
		AllocatorNode* node = reinterpret_cast<AllocatorNode*>(dataPtr - sizeof(AllocatorNode));

		// Chain the node back to free node
		node->next_ = allocator->free_;
		allocator->free_ = node;
	}
}