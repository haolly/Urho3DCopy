//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_HASHMAP_H
#define URHO3DCOPY_HASHMAP_H

#include "HashBase.h"

namespace Urho3D
{
	//todo
	template <class T, class U>
	class HashMap : public HashBase
	{
	public:
		using KeyType = T;
		using ValueType = U;
	};
}



#endif //URHO3DCOPY_HASHMAP_H
