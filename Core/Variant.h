//
// Created by LIUHAO on 2017/11/30.
//

#ifndef URHO3DCOPY_VARIANT_H
#define URHO3DCOPY_VARIANT_H

#include "../Container/Vector.h"
#include "../Container/HashMap.h"
#include "../Math/StringHash.h"
#include "../Math/Vector2.h"
#include "../Container/Str.h"
#include "../Math/Vector4.h"
#include "../Math/Quaternion.h"
#include "../Math/Color.h"
#include "../Container/RefCounted.h"
#include "../Container/Ptr.h"
#include "../Math/Matrix3x4.h"

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
		VAR_QUATERNION,
		VAR_COLOR,
		VAR_STRING,
		VAR_BUFFER,
		VAR_VOIDPTR,
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


	class Variant;
	class VectorBuffer;


	using VariantVector = Vector<Variant>;
	using StringVector = Vector<String>;
	using VariantMap = HashMap<StringHash, Variant>;

	struct ResourceRef
	{
		ResourceRef() {}
		ResourceRef(StringHash type) :
			type_(type)
		{
		}

		ResourceRef(StringHash type, const String& name) :
			type_(type),
			name_(name)
		{
		}

		ResourceRef(const String& type, const String& name) :
			type_(type),
			name_(name)
		{
		}

		ResourceRef(const char* type, const char* name) :
			type_(type),
			name_(name)
		{

		}

		ResourceRef(const ResourceRef& rhs) :
			type_(rhs.type_),
			name_(rhs.name_)
		{

		}

		bool operator ==(const ResourceRef& rhs) const
		{
			return type_ == rhs.type_ && name_ == rhs.name_;
		}

		bool operator !=(const ResourceRef& rhs) const
		{
			return type_ != rhs.type_ || name_ != rhs.name_;
		}



		StringHash type_;
		String name_;
	};

	struct ResourceRefList
	{
		ResourceRefList()
		{

		}

		ResourceRefList(StringHash type) :
				type_(type)
		{
		}

		ResourceRefList(StringHash type, const StringVector& names) :
			type_(type),
			names_(names)
		{

		}

		bool operator==(const ResourceRefList& rhs) const
		{
			return type_ == rhs.type_ && names_ == rhs.names_;
		}

		bool operator!=(const ResourceRefList& rhs) const
		{
			return type_ != rhs.type_ || names_ != rhs.names_;
		}



		StringHash type_;
		StringVector names_;
	};

	//todo, why the total variantValue size is this ?
	//Size of variant value. 16 bytes on 32-bit platform, 32 bytes on 64-bit platform.
	static const unsigned VARIANT_VALUE_SIZE = sizeof(void*) * 4;

	//Union for the possible variant values, Objects exceeding the VARIANT_VALUE_SIZE are allocated on the heap
	union VariantValue
	{
		unsigned char storage_[VARIANT_VALUE_SIZE];
		int int_;
		bool bool_;
		float float_;
		Vector2 vector2_;
		Vector3 vector3_;
		Vector4 vector4_;
		Quaternion quaternion_;
		Color color_;
		String string_;
		PODVector<unsigned char> buffer_;
		void* voidPtr;
		ResourceRef resourceRef_;
		ResourceRefList resourceRefList_;
		VariantVector variantVector_;
		VariantMap variantMap_;
		//todo
		WeakPtr<RefCounted> weakPtr_;
		Matrix3* matrix3_;
		Matrix3x4* matrix3x4_;
		Matrix4* matrix4_;
		double double_;
		StringVector stringVector_;
		long long int64_;
		CustomVariantValue* customValueHeap_;
		//todo

		VariantValue() {}
		VariantValue(const VariantValue& value) = delete ;
		~VariantValue() {}
	};

	static_assert(sizeof(VariantValue) == VARIANT_VALUE_SIZE, "Unexpected size of VariantValue");

	//todo
	class Variant
	{
	public:
		Variant() {}
		Variant(int value)
		{
			*this = value;
		}

		Variant(long long value)
		{
			*this = value;
		}

		Variant(unsigned value)
		{
			*this = (int)value;
		}

		Variant(unsigned long long value)
		{
			*this = (long long)value;
		}

		Variant(const StringHash& value)
		{
			*this = (int)value.Value();
		}

		Variant(bool value)
		{
			*this = value;
		}

		Variant(float value)
		{
			*this = value;
		}

		Variant(double value)
		{
			*this = value;
		}

		Variant(const Vector2& value)
		{
			*this = value;
		}

		Variant(const Vector3& value)
		{
			*this = value;
		}

		Variant(const Vector4& value)
		{
			*this = value;
		}

		Variant(const Quaternion& value)
		{
			*this = value;
		}

		Variant(const Color& value)
		{
			*this = value;
		}

		Variant(const String& value)
		{
			*this = value;
		}

		Variant(const char* value)
		{
			*this = value;
		}

		Variant(const PODVector<unsigned char>& value)
		{
			*this = value;
		}

		Variant(const VectorBuffer& value)
		{
			*this = value;
		}

		Variant(void* value)
		{
			*this = value;
		}

		Variant(const ResourceRef& value)
		{
			*this = value;
		}

		Variant(const ResourceRefList& value)
		{
			*this = value;
		}

		Variant(const VariantVector& value)
		{
			*this = value;
		}

		Variant(const VariantMap& value)
		{
			*this = value;
		}

		Variant(const StringVector& value)
		{
			*this = value;
		}
		//todo

		Variant(const IntVector2& value)
		{
			*this = value;
		}

		Variant(const IntVector3& value)
		{
			*this = value;
		}

		Variant(RefCounted* value)
		{
			*this = value;
		}

		Variant(const Matrix3& value)
		{
			*this = value;
		}

		Variant(const Matrix3x4& value)
		{
			*this = value;
		}

		//todo


		Variant(const String& type, const String& value)
		{
			FromString(type, value);
		}

		Variant(VariantType type, const String& value)
		{
			FromString(type, value);
		}

		Variant(const char* type, const char* value)
		{
			FromString(type, value);
		}

		Variant(VariantType type, const char* value)
		{
			FromString(type, value);
		}

		Variant(const Variant& value)
		{
			*this = value;
		}

		~Variant()
		{
			SetType(VAR_NONE);
		}

		void Clear()
		{
			SetType(VAR_NONE);
		}

		Variant&operator =(const Variant& rhs);

		Variant&operator =(int rhs)
		{
			SetType(VAR_INT);
			value_.int_ = rhs;
			return *this;
		}

		Variant&operator =(long long rhs)
		{
			SetType(VAR_INT64);
			value_.int64_ = rhs;
			return *this;
		}

		Variant&operator =(unsigned long long rhs)
		{
			SetType(VAR_INT64);
			//Note, if another operand is unsigned, the signed will be converted to unsigned, so need cast
			value_.int64_ = static_cast<long long>(rhs);
			return *this;
		}

		Variant&operator =(unsigned rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs;
			return *this;
		}

		Variant&operator = (const StringHash& rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs.Value();
			return *this;
		}

		Variant&operator =(bool rhs)
		{
			SetType(VAR_BOOL);
			value_.bool_ = rhs;
			return *this;
		}

		Variant&operator =(float rhs)
		{
			SetType(VAR_FLOAT);
			value_.float_ = rhs;
			return *this;
		}

		Variant&operator =(double rhs)
		{
			SetType(VAR_DOUBLE);
			value_.double_ = rhs;
			return *this;
		}

		Variant&operator =(const Vector2& rhs)
		{
			SetType(VAR_VECTOR2);
			value_.vector2_ = rhs;
			return *this;
		}

		Variant&operator =(const Vector3& rhs)
		{
			SetType(VAR_VECTOR3);
			value_.vector3_ = rhs;
			return *this;
		}

		Variant&operator =(const Vector4& rhs)
		{
			SetType(VAR_VECTOR4);
			value_.vector4_ = rhs;
			return *this;
		}

		Variant& operator =(const Quaternion& rhs)
		{
			SetType(VAR_QUATERNION);
			value_.quaternion_ = rhs;
			return *this;
		}

		Variant&operator =(const Color& rhs)
		{
			SetType(VAR_COLOR);
			value_.color_ = rhs;
			return *this;
		}

		Variant&operator=(const String& rhs)
		{
			SetType(VAR_STRING);
			value_.string_ = rhs;
			return *this;
		}

		Variant&operator =(const char* rhs)
		{
			SetType(VAR_STRING);
			value_.string_ = rhs;
			return *this;
		}

		Variant&operator =(const PODVector<unsigned char>& rhs)
		{
			SetType(VAR_BUFFER);
			value_.buffer_ = rhs;
			return *this;
		}

		Variant&operator =(const VectorBuffer& rhs);

		Variant&operator =(void* rhs)
		{
			SetType(VAR_VOIDPTR);
			value_.voidPtr = rhs;
			return *this;
		}

		Variant&operator =(const ResourceRef& rhs)
		{
			SetType(VAR_RESOURCEREF);
			value_.resourceRef_ = rhs;
			return *this;
		}

		Variant&operator =(const ResourceRefList& rhs)
		{
			SetType(VAR_RESOURCEREFLIST);
			value_.resourceRefList_ = rhs;
			return *this;
		}

		Variant&operator =(const VariantVector& rhs)
		{
			SetType(VAR_VARIANTVECTOR);
			value_.variantVector_ = rhs;
			return *this;
		}

		Variant&operator =(const VariantMap& rhs)
		{
			SetType(VAR_VARIANTMAP);
			value_.variantMap_ = rhs;
			return *this;
		}

		//todo

		Variant&operator =(RefCounted* rhs)
		{
			SetType(VAR_PTR);
			value_.weakPtr_ = rhs;
			return *this;
		}

		Variant&operator =(const Matrix3& rhs)
		{
			SetType(VAR_MATRIX3);
			*value_.matrix3_ = rhs;
			return *this;
		}

		Variant&operator =(const Matrix3x4& rhs)
		{
			SetType(VAR_MATRIX3X4);
			*value_.matrix3x4_ = rhs;
			return *this;
		}


		//todo ..

		bool operator ==(const Variant& rhs) const;
		bool operator ==(int rhs) const
		{
			return type_ == VAR_INT ? value_.int_ == rhs : false;
		}

		bool operator ==(unsigned rhs) const
		{
			return type_ == VAR_INT ? value_.int_ == static_cast<int>(rhs) : false;
		}

		bool operator==(long long rhs) const
		{
			return type_ == VAR_INT64 ? value_.int64_ == rhs : false;
		}

		bool operator ==(unsigned long long rhs) const
		{
			return type_ == VAR_INT64 ? value_.int64_ == static_cast<long long>(rhs) : false;
		}

		bool operator ==(bool rhs) const
		{
			return type_ == VAR_BOOL ? value_.bool_ == rhs : false;
		}

		bool operator ==(float rhs) const
		{
			return type_ == VAR_FLOAT ? value_.float_ == rhs : false;
		}

		bool operator ==(double rhs) const
		{
			return type_ == VAR_DOUBLE ? value_.double_ == rhs : false;
		}


		//todo ...
		void FromString(const String& type, const String& value);
		void FromString(const char* type, const char* value);
		void FromString(VariantType type, const String& value);
		void FromString(VariantType type, const char* value);
		void SetBuffer(const void* data, unsigned size);


		//todo ,why not Var_int64???
		int GetInt() const
		{
			if(type_ == VAR_INT)
				return value_.int_;
			else if(type_ == VAR_FLOAT)
				return static_cast<int>(value_.float_);
			else if(type_ == VAR_DOUBLE)
				return static_cast<int>(value_.double_);
			else
				return 0;
		}

		unsigned GetUInt() const
		{
			if(type_ == VAR_INT)
				return static_cast<unsigned>(value_.int_);
			else if(type_ == VAR_FLOAT)
				return static_cast<unsigned>(value_.float_);
			else if(type_ == VAR_DOUBLE)
				return static_cast<unsigned>(value_.double_);
			else
				return 0;
		}

		long long GetInt64() const
		{
			if(type_ == VAR_INT64)
				return value_.int64_;
			else if(type_ == VAR_INT)
				return value_.int_;
			else if(type_ == VAR_FLOAT)
				return static_cast<long long>(value_.float_);
			else if(type_ == VAR_DOUBLE)
				return static_cast<long long>(value_.double_);
			else
				return 0;
		}

		unsigned long long GetUInt64() const
		{
			if(type_ == VAR_INT64)
				return static_cast<unsigned long long>(value_.int64_);
			else if(type_ == VAR_INT)
				return static_cast<unsigned long long>(value_.int_);
			else if(type_ == VAR_FLOAT)
				return static_cast<unsigned long long>(value_.float_);
			else if(type_ == VAR_DOUBLE)
				return static_cast<unsigned long long>(value_.double_);
			else
				return 0;
		}

		StringHash GetStringHash() const
		{
			return StringHash(GetUInt());
		}

		bool GetBool() const
		{
			return type_ == VAR_BOOL ? value_.bool_ : false;
		}

		float GetFloat() const
		{
			if(type_ == VAR_FLOAT)
				return value_.float_;
			if(type_ == VAR_DOUBLE)
				return static_cast<float>(value_.double_);
			if(type_ == VAR_INT)
				return static_cast<float>(value_.int_);
			if(type_ == VAR_INT64)
				return static_cast<float>(value_.int64_);
			return 0.0f;
		}

		double GetDouble() const
		{
			if(type_ == VAR_DOUBLE)
				return value_.double_;
			if(type_ == VAR_FLOAT)
				return static_cast<double>(value_.float_);
			if(type_ == VAR_INT)
				return static_cast<double>(value_.int_);
			if(type_ == VAR_INT64)
				return static_cast<double>(value_.int64_);
			return 0.0;
		}

		const Vector2& GetVector2() const
		{
			return type_ == VAR_VECTOR2 ? value_.vector2_ : Vector2::ZERO;
		}

		const Vector3& GetVector3() const
		{
			return type_ == VAR_VECTOR3 ? value_.vector3_ : Vector3::ZERO;
		}

		const Vector4& GetVector4() const
		{
			return type_ == VAR_VECTOR4 ? value_.vector4_ : Vector4::ZERO;
		}
		//todo

		const String& GetString() const
		{
			return (type_ == VAR_STRING ? value_.string_ : String::EMPTY);
		}

		const PODVector<unsigned char>& GetBuffer() const
		{
			return type_ == VAR_BUFFER ? value_.buffer_ : emptyBuffer;
		}

		//todo

		VariantType GetType() const { return type_; }
		String GetTypeName() const;
		String ToString() const;
		bool IsZero() const;
		bool IsEmpty() const
		{
			return type_ == VAR_NONE;
		}

		bool IsCustom() const
		{
			return type_ == VAR_CUSTOM_STACK || type_ == VAR_CUSTOM_HEAP;
		}

		template <class T> T Get() const;

		PODVector<unsigned char>* GetBufferPtr()
		{
			return type_ == VAR_BUFFER ? &value_.buffer_ : nullptr;
		}

		VariantVector* GetVariantVectorPtr()
		{
			return type_ == VAR_VARIANTVECTOR ? &value_.variantVector_ : nullptr;
		}

		VariantMap* GetVariantMapPtr()
		{
			return type_ == VAR_VARIANTMAP ? &value_.variantMap_ : nullptr;
		}
		//todo

		static String GetTypeName(VariantType type);
		static VariantType GetTypeFromName(const String& typeName);
		static VariantType GetTypeFromName(const char* typeName);

		static const Variant EMPTY;
		static const PODVector<unsigned char> emptyBuffer;
		static const ResourceRef emptyResourceRef;
		static const ResourceRefList emptyResourceRefList;
		static const VariantMap emptyVariantMap;
		static const VariantVector emptyVariantVector;
		static const StringVector emptyStringVector;



	private:
		void SetType(VariantType newType);
		VariantType  type_ = VAR_NONE;
		VariantValue value_;
	};

	template <typename T> VariantType GetVariantType();

	template <> inline VariantType GetVariantType<int>() { return VAR_INT; }
	template <> inline VariantType GetVariantType<unsigned>() { return VAR_INT; };
	template <> inline VariantType GetVariantType<long long>() { return VAR_INT64; }
	//todo
}



#endif //URHO3DCOPY_VARIANT_H
