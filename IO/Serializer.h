//
// Created by LIUHAO on 2017/12/8.
//

#ifndef URHO3DCOPY_SERIALIZER_H
#define URHO3DCOPY_SERIALIZER_H

#include "../Container/Str.h"

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
		//todo ..
		bool WriteLine(const String& value);
	};

}


#endif //URHO3DCOPY_SERIALIZER_H
