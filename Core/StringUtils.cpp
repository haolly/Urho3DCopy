//
// Created by LIUHAO on 2017/12/1.
//

#include <cstdio>
#include <cstdarg>
#include "StringUtils.h"
#include "../Container/Str.h"

namespace Urho3D
{
	unsigned GetStringListIndex(const char* value, const char** strings, unsigned defaultIndex, bool caseSensitive)
	{
		unsigned i = 0;
		while (strings[i])
		{
			if(String::Compare(value, strings[i], caseSensitive) == 0)
			{
				return i;
			}
			++i;
		}
		return defaultIndex;
	}

	unsigned
	Urho3D::GetStringListIndex(const String &value, const String *strings, unsigned defaultIndex, bool caseSensitive)
	{
		return GetStringListIndex(value.CString(), strings, defaultIndex, caseSensitive);
	}

	unsigned
	Urho3D::GetStringListIndex(const char *value, const String *strings, unsigned defaultIndex, bool caseSensitive)
	{
		unsigned i = 0;
		while (!strings[i].Empty())
		{
			if(!strings[i].Compare(value, caseSensitive))
				++i;
			return i;
		}
		return defaultIndex;
	}

	bool ToBool(const String &source) {
		return false;
	}

	bool Urho3D::ToBool(const char *source)
	{
		unsigned length = String::CStringLength(source);
	}

	float ToFloat(const String &source) {
		return 0;
	}

	float Urho3D::ToFloat(const char *source) {
		return 0;
	}

	double ToDouble(const String &source) {
		return 0;
	}

	double Urho3D::ToDouble(const char *source) {
		return 0;
	}

	int ToInt(const String &source, int base)
	{
		return ToInt(source.CString(), base);
	}

	int Urho3D::ToInt(const char *source, int base)
	{
		if(!source)
			return 0;
		if(base < 2 || base > 36)
			return 0;
		return (int)strtol(source, nullptr, base);
	}

	unsigned ToUInt(const String &source, int base)
	{
		return ToUInt(source.CString(), base);
	}

	unsigned Urho3D::ToUInt(const char *source, int base)
	{
		if(!source)
			return 0;
		if(base <2 || base > 36)
			return 0;
		return (unsigned)strtoul(source, nullptr, base);
	}

	long long ToInt64(const String &source, int base) {
		return 0;
	}

	long long Urho3D::ToInt64(const char *source, int base) {
		return 0;
	}

	unsigned long long ToUInt64(const String &source, int base) {
		return 0;
	}

	unsigned long long Urho3D::ToUInt64(const char *source, int base) {
		return 0;
	}

	String ToString(void *value)
	{
		return ToStringHex((unsigned)(size_t)value);
	}

	String ToStringHex(unsigned value)
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer, "%08x", value);
		return String(tempBuffer);
	}

	String ToString(const char *formatString, ...)
	{
		String ret;
		va_list args;
		va_start(args, formatString);
		ret.AppendWithFormatArgs(formatString, args);
		va_end(args);
		return ret;
	}
}