//
// Created by LIUHAO on 2017/12/1.
//

#ifndef URHO3DCOPY_STRINGUTILS_H
#define URHO3DCOPY_STRINGUTILS_H

#include "../Container/Str.h"

namespace Urho3D
{
	static const String base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

	unsigned GetStringListIndex(const String& value, const String* strings, unsigned defaultIndex, bool caseSensitive = false);
	unsigned GetStringListIndex(const char* value, const String* strings, unsigned defaultIndex, bool caseSensitive = false);
	unsigned GetStringListIndex(const char* value, const char** strings, unsigned defaultIndex, bool caseSensitive = false);

	bool ToBool(const String& source);
	bool ToBool(const char* source);
	float ToFloat(const String& source);
	float ToFloat(const char* source);
	double ToDouble(const String& source);
	double ToDouble(const char* source);
	int ToInt(const String& source, int base = 10);
	int ToInt(const char* source, int base = 10);
	unsigned ToUInt(const String& source, int base = 10);
	unsigned ToUInt(const char* source, int base = 10);
	long long ToInt64(const String& source, int base = 10);
	long long ToInt64(const char* source, int base = 10);
	unsigned long long ToUInt64(const String& source, int base = 10);
	unsigned long long ToUInt64(const char* source, int base = 10);


}

#endif //URHO3DCOPY_STRINGUTILS_H
