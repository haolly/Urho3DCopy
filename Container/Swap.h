//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_SWAP_H
#define URHO3DCOPY_SWAP_H

namespace Urho3D
{
	class String;

	template <class T>
	inline void Swap(T& first, T& second)
	{
		T temp = first;
		first = second;
		second = temp;
	}
//todo
// http://en.cppreference.com/w/cpp/language/template_specialization
	template <>
	void Swap<String>(String& first, String& second);
}

#endif //URHO3DCOPY_SWAP_H
