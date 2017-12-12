//
// Created by LIUHAO on 2017/11/30.
//

#include "Swap.h"
#include "Str.h"
#include "../Container/HashBase.h"
#include "../Container/ListBase.h"

namespace Urho3D
{
	template<>
	void Urho3D::Swap<String>(String &first, String &second)
	{
		first.Swap(second);
	}

	template<>
	void Urho3D::Swap<VectorBase>(VectorBase &first, VectorBase &second)
	{
		first.Swap(second);
	}

	template<>
	void Urho3D::Swap<ListBase>(ListBase &first, ListBase &second)
	{
		first.Swap(second);
	}

	template<>
	void Urho3D::Swap<HashBase>(HashBase &first, HashBase &second)
	{
		first.Swap(second);
	}
}