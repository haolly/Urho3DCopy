//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_VARIANT_H
#define URHO3DCOPY_VARIANT_H

#include "../Container/Vector.h"
#include "../Container/HashMap.h"
#include "../Math/StringHash.h"
#include "../Math/Vector2.h"

namespace Urho3D
{
	enum VariantType
	{
		VAR_NONE = 0,
		VAR_INT,
		VAR_BOOL,
		VAR_FLOAT,
		VAR_VECTOR2,
		VAR_VECTOR3,
		VAR_VECTOR4,
		VAR_COLOR,
		VAR_STRING,
		VAR_BUFFER,
		VAR_RESOURCEREF,
		VAR_RESOURCEREFLIST,
		VAR_VARIANTVECTOR,
		VAR_VARIANTMAP,
		VAR_INTRECT,
		VAR_INTVECTOR2,
		VAR_PTR,
		VAR_MATRIX3,
		VAR_MATRIX3X4,
		VAR_MATRIX4,
		VAR_DOUBLE,
		VAR_STRINGVECTOR,
		VAR_RECT,
		VAR_INTVECTOR3,
		VAR_INT64,
		//ADD NEW TYPES HERE
		VAR_CUSTOM_HEAP,
		VAR_CUSTOM_STACK,
		MAX_VAR_TYPES
	};

	static const unsigned VARIANT_VALUE_SIZE = sizeof(void*) * 4;

	union VariantValue
	{
		unsigned char storage_[VARIANT_VALUE_SIZE];
		int int_;
		bool bool_;
		float float_;
		Vector2 vector2;
		//todo ..

		VariantValue() {}
		VariantValue(const VariantValue& value) = delete ;
		~VariantValue() {}
	};


	using VariantVector = Vector<Variant>;
	using StringVector = Vector<String>;
	using VariantMap = HashMap<StringHash, Variant>;

	//todo
	class Variant
	{

	};
}



#endif //URHO3DCOPY_VARIANT_H
