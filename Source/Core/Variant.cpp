//
// Created by LIUHAO on 2017/11/30.
//

#include "Variant.h"
#include "StringUtils.h"

namespace Urho3D
{

	const Variant Variant::EMPTY;
	const PODVector<unsigned char> Variant::emptyBuffer;
	const ResourceRef Variant::emptyResourceRef;
	const ResourceRefList Variant::emptyResourceRefList;
	const VariantMap Variant::emptyVariantMap;
	const VariantVector Variant::emptyVariantVector;
	const StringVector Variant::emptyStringVector;


	//Note, need to maintain the same order with VariantType
	static const char * typeNames[] = {
		"None",
		"Int",
		"Bool",
		"Float",
		"Vector2",
		"Vector3",
		"Vector4",
		"Quaternion",
		"Color",
		"String",
		"Buffer",
		"VoidPtr",
		"ResourceRef",
		"ResourceRefList",
		"VariantVector",
		"VariantMap",
		"IntRect",
		"IntVector2",
		"Ptr",
		"Matrix3",
		"Matrix3x4",
		"Matrix4",
		"Double",
		"StringVector",
		"Rect",
		"IntVector3",
		"Int64",
		"CustomHeap",
		"CustomStack",
		nullptr
	};

	static_assert(sizeof(typeNames) / sizeof(const char*) == MAX_VAR_TYPES + 1,
	              "Variant type name array is out-of-data");

	Variant &Variant::operator=(const Variant &rhs)
	{
		if(rhs.IsCustom())
		{
			//todo
			//SetCustomVariantValue(*rhs.GetCustomVariantValuePtr());
			return *this;
		}
		SetType(rhs.GetType());
		switch(type_)
		{
			case VAR_STRING:
				value_.string_ = rhs.value_.string_;
				break;
			case VAR_BUFFER:
				value_.buffer_ = rhs.value_.buffer_;
				break;
			case VAR_RESOURCEREF:
				value_.resourceRef_ = rhs.value_.resourceRef_;
				break;
			case VAR_RESOURCEREFLIST:
				value_.resourceRefList_ = rhs.value_.resourceRefList_;
				break;
			case VAR_VARIANTVECTOR:
				value_.variantVector_ = rhs.value_.variantVector_;
				break;
			case VAR_STRINGVECTOR:
				//todo
		}
	}

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


	bool Variant::operator==(const Variant &rhs) const
	{
		return false;
	}

	void Variant::SetBuffer(const void *data, unsigned size)
	{

	}
}
