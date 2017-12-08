//
// Created by LIUHAO on 2017/12/8.
//

#include "Serializer.h"

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
		return false;
	}
}
