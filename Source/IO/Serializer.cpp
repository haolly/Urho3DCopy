//
// Created by LIUHAO on 2017/12/8.
//

#include "Serializer.h"
#include "../Math/Color.h"

namespace Urho3D
{
	static const float q = 32767.0f;

	Serializer::~Serializer()
	{
	}

	bool Serializer::WriteInt64(long long value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteInt(int value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteShort(short value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteByte(signed char value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteUInt64(unsigned long long value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteUInt(unsigned value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteUShort(unsigned short value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteUByte(unsigned char value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteBool(bool value)
	{
		return WriteUByte((unsigned char)(value ? 1 : 0)) == 1;
	}

	bool Serializer::WriteFloat(float value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteDouble(float value)
	{
		return Write(&value, sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteIntRect(const IntRect &value)
	{
		//todo
		return false;
	}

	bool Serializer::WriteIntVector2(const IntVector2 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteIntVector3(const IntVector3 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteRect(const Rect &value)
	{
		//todo
	}

	bool Serializer::WriteVector2(const Vector2 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteVector3(const Vector3 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteMatrix3(const Matrix3 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteMatrix3x4(const Matrix3x4 &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteColor(const Color &value)
	{
		return Write(value.Data(), sizeof(value)) == sizeof(value);
	}

	bool Serializer::WriteBoundingBox(const BoundingBox &value)
	{
		bool success = true;
		success &= WriteVector3(value.min_);
		success &= WriteVector3(value.max_);
	}

	bool Serializer::WriteString(const String &value)
	{
		const char* chars = value.CString();
		// Correspond to ReadString()
		unsigned length = String::CStringLength(chars);
		return Write(chars, length + 1) == length + 1;
	}

	bool Serializer::WriteFileID(const String &value)
	{
		bool success = true;
		unsigned length = Min(value.Length(), 4U);

		success &= Write(value.CString(), length) == length;
		for(unsigned i=value.Length(); i<4; ++i)
			success &= WriteByte(' ');
		return success;
	}

	bool Serializer::WriteStringHash(const StringHash &value)
	{
		return WriteUInt(value.Value());
	}

	bool Serializer::WriteBuffer(const PODVector<unsigned char> &buffer)
	{
		bool success = true;
		unsigned size = buffer.Size();

		success &= WriteVLE(size);
		if(size)
			success &= Write(&buffer[0], size) == size;
		return success;
	}

	bool Serializer::WriteResourceRef(const ResourceRef &value)
	{
		bool success = true;
		success &= WriteStringHash(value.type_);
		success &= WriteString(value.name_);
		return success;
	}

	bool Serializer::WriteResourceRefList(const ResourceRefList &value)
	{
		bool success = true;

		success &= WriteStringHash(value.type_);
		success &= WriteVLE(value.names_.Size());
		for(unsigned i=0; i<value.names_.Size(); ++i)
			success &= WriteString(value.names_[i]);
		return success;
	}

	bool Serializer::WriteVariant(const Variant &value)
	{
		bool success = true;
		VariantType type = value.GetType();

		success &= WriteUByte((unsigned char)type);
		success &= WriteVariantData(value);
		return success;
	}

	bool Serializer::WriteVariantData(const Variant &value)
	{
		switch(value.GetType())
		{
			case VAR_NONE:
				return true;
			case VAR_INT:
				return WriteInt(value.GetInt());
			case VAR_INT64:
				return WriteInt64(value.GetInt64());
			case VAR_BOOL:
				return WriteBool(value.GetBool());
			case VAR_FLOAT:
				return WriteFloat(value.GetFloat());
			case VAR_VECTOR2:
				return WriteVector2(value.GetVector2());
			case VAR_VECTOR3:
				return WriteVector3(value.GetVector3());
			//todo
		}
	}

	bool Serializer::WriteVariantVector(const VariantVector &value)
	{
		return false;
	}

	bool Serializer::WriteStringVector(const StringVector &value)
	{
		return false;
	}

	bool Serializer::WriteVariantMap(const VariantMap &value)
	{
		return false;
	}

	//Note, ref https://en.wikipedia.org/wiki/Variable-length_quantity
	bool Serializer::WriteVLE(unsigned value)
	{
		unsigned char data[4];

		if(value < 0x80)
			return WriteUByte((unsigned char)value);
		// 0x4000 is the max 14 bit value
		else if(value < 0x4000)
		{
			// make the 8th bit be 1, actually, save original 7 bits, the 8th bit is a flag indicate whether there are more bit
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)(value >> 7);
			return Write(data, 2) == 2;
		}
		// 0x20 00 00 is the max 21 bit value
		else if(value < 0x200000)
		{
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)((value >> 7) | 0x80);
			data[2] = (unsigned char)(value >> 14);
			return Write(data, 3) == 3;
		}
		else
		{
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)((value >> 7) | 0x80);
			data[2] = (unsigned char)((value >> 14) | 0x80);
			data[3] = (unsigned char)(value >> 21);
			return Write(data, 4) == 4;
		}
	}

	bool Serializer::WriteNetID(unsigned value)
	{
		return false;
	}

	bool Serializer::WriteLine(const String &value)
	{
		bool success = true;
		success &= Write(value.CString(), value.Length()) == value.Length();
		//crlf
		success &= WriteUByte(13);
		success &= WriteUByte(10);
		return success;
	}


}
