//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_SWAP_H
#define URHO3DCOPY_SWAP_H

namespace Urho3D
{
	class HashBase;
	class ListBase;
	class String;
	class VectorBase;

	template <class T>
	inline void Swap(T& first, T& second)
	{
		T temp = first;
		first = second;
		second = temp;
	}

	template <>
	void Swap<String>(String& first, String& second);

	template <>
	void Swap<VectorBase>(VectorBase& first, VectorBase& second);

	template <>
	void Swap<ListBase>(ListBase& first, ListBase& second);

	template <>
	void Swap<HashBase>(HashBase& first, HashBase& second);
}

#endif //URHO3DCOPY_SWAP_H
