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

	// template specialization
	// http://en.cppreference.com/w/cpp/language/template_specialization
	// Void pointer hash function
	template <> inline unsigned MakeHash(const void* value)
	{
		return (unsigned)(size_t)value;
	}


	//todo ...
}

#endif //URHO3DCOPY_HASH_H
