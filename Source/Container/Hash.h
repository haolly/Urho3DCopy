//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_HASH_H
#define URHO3DCOPY_HASH_H

#include <cstddef>

namespace Urho3D
{
	template <class T>
	unsigned MakeHash(T* value)
	{
		return (unsigned)((size_t)value / sizeof(T));
	}

	template <class T> unsigned MakeHash(const T* value)
	{
		return (unsigned)((size_t)value / sizeof(T));
	}

	template <class T> unsigned MakeHash(const T& value)
	{
		return value.ToHash();
	}


	// template specialization, 特化允许针对不同的类型，内部实现不同
	// Void pointer hash function
	template <> inline unsigned MakeHash(void* value)
	{
		return (unsigned)(size_t)value;
	}

	template <> inline unsigned MakeHash(const void* value)
	{
		return (unsigned)(size_t)value;
	}

	template <> inline unsigned MakeHash(const long long& value)
	{
		return (unsigned)((value >> 32) | (value & 0xffffffff));
	}

	template <> inline unsigned MakeHash(const unsigned long long& value)
	{
		return (unsigned)((value >> 32) | (value & 0xffffffff));
	}

	template <> inline unsigned MakeHash(const int& value)
	{
		return (unsigned)value;
	}

	template <> inline unsigned MakeHash(const unsigned& value)
	{
		return value;
	}

	template <> inline unsigned MakeHash(const short& value)
	{
		return (unsigned)value;
	}

	template <> inline unsigned MakeHash(const unsigned short& value)
	{
		return value;
	}

	template <> inline unsigned MakeHash(const char& value)
	{
		return (unsigned)value;
	}

	template <> inline unsigned MakeHash(const unsigned char& value)
	{
		return value;
	}

}

#endif //URHO3DCOPY_HASH_H
