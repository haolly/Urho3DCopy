//
// Created by LIUHAO on 2017/11/30.
//

#include "Variant.h"
#include "StringUtils.h"

namespace Urho3D
{

	//Note, need to maintain the same order with VariantType
	static const char * typeNames[] = {
		"None",
		"Int",
		"Bool",
		//todo ...
	};

	VariantType Variant::GetTypeFromName(const char *typeName)
	{
		return (VariantType)GetStringListIndex(typeName, typeNames, VAR_NONE);
	}

	VariantType Variant::GetTypeFromName(const String &typeName)
	{
		return GetTypeFromName(typeName.CString());
	}

	String Variant::GetTypeName(VariantType type)
	{
		return typeNames[type];
	}

	void Variant::FromString(const String &type, const String &value)
	{
		return FromString(type.CString(), value.CString());
	}

	void Variant::FromString(const char *type, const char *value)
	{
		return FromString(GetTypeFromName(type), value);
	}

	void Variant::FromString(VariantType type, const String &value)
	{
		return FromString(type, value.CString());
	}

	void Variant::FromString(VariantType type, const char *value)
	{
		switch (type)
		{
			case VAR_INT:
				*this = ToInt(value);
				break;
			case VAR_INT64:
				*this = ToInt64(value);
				break;
			case VAR_BOOL:
				*this = ToBool(value);
				break;
			case VAR_FLOAT:
				*this = ToFloat(value);
				break;
			case VAR_DOUBLE:
				*this = ToDouble(value);
				break;
			//todo ..
			default:
				SetType(VAR_NONE);
		}
	}

	void Variant::SetType(VariantType newType)
	{
		if(type_ == newType)
			return;

		//Note call destructor
		switch(type_)
		{
			case VAR_STRING:
				value_.string_.~String();
				break;
			case VAR_RESOURCEREF:
				value_.resourceRef_.~ResourceRef();
				break;
			case VAR_RESOURCEREFLIST:
				value_.resourceRef_.~ResourceRefList();
				break;
			//todo ..
			default:
				break;
		}
		type_ = newType;

		switch (type_)
		{
			case VAR_STRING:
				new(&value_.string_) String();
				break;
			case VAR_RESOURCEREF:
				new(&value_.resourceRef_) ResourceRef();
				break;
			case VAR_RESOURCEREFLIST:
				new(&value_.resourceRefList_) ResourceRefList();
				break;
			//todo
			default:
				break;
		}
	}
}
