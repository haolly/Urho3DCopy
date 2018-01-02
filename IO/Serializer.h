//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_SERIALIZER_H
#define URHO3DCOPY_SERIALIZER_H

#include "../Container/Str.h"
#include "../Math/Matrix3x4.h"
#include "../Math/BoundingBox.h"
#include "../Math/StringHash.h"
#include "../Core/Variant.h"

namespace Urho3D
{
	class Color;
	class IntRect;
	class IntVector2;
	class IntVector3;
	class Quaternion;
	class Rect;
	class Vector2;
	class Vector3;
	class Vector4;

	class Serializer
	{
	public:
		virtual ~Serializer();

		virtual unsigned Write(const void* data, unsigned size) = 0;

		bool WriteInt64(long long value);
		bool WriteInt(int value);
		bool WriteShort(short value);
		//Todo, why byte is not unsigned char ??
		bool WriteByte(signed char value);
		bool WriteUInt64(unsigned long long value);
		bool WriteUInt(unsigned value);
		bool WriteUShort(unsigned short value);
		bool WriteUByte(unsigned char value);
		bool WriteBool(bool value);
		bool WriteFloat(float value);
		bool WriteDouble(float value);
		bool WriteIntRect(const IntRect& value);
		bool WriteIntVector2(const IntVector2& value);
		bool WriteIntVector3(const IntVector3& value);
		bool WriteRect(const Rect& value);
		bool WriteVector2(const Vector2& value);
		bool WriteVector3(const Vector3& value);
		//todo ..
		bool WriteMatrix3(const Matrix3& value);
		bool WriteMatrix3x4(const Matrix3x4& value);
		bool WriteColor(const Color& value);
		bool WriteBoundingBox(const BoundingBox& value);
		bool WriteString(const String& value);
		bool WriteFileID(const String& value);
		bool WriteStringHash(const StringHash& value);
		bool WriteBuffer(const PODVector<unsigned char>& buffer);
		bool WriteResourceRef(const ResourceRef& value);
		bool WriteResourceRefList(const ResourceRefList& value);
		bool WriteVariant(const Variant& value);
		bool WriteVariantData(const Variant& value);
		bool WriteVariantVector(const VariantVector& value);
		bool WriteStringVector(const StringVector& value);
		bool WriteVariantMap(const VariantMap& value);
		bool WriteVLE(unsigned value);
		bool WriteNetID(unsigned value);
		bool WriteLine(const String& value);
	};

}


#endif //URHO3DCOPY_SERIALIZER_H
