//
// Created by liuhao on 2017/11/28.
//

#include <cctype>
#include <cstdarg>
#include "Str.h"

namespace Urho3D
{
	int String::Compare(const char *str1, const char *str2, bool caseSenstive)
	{
		if (caseSenstive)
		{
			return strcmp(str1, str2);
		}
		if (!str1 || !str2)
		{
			return str1 ? 1 : (str2 ? -1 : 0);
		}
		int pos = 0;
		do
		{
			char a = static_cast<char>(tolower(str1[pos]));
			char b = static_cast<char>(tolower(str2[pos]));
			if (!a || !b)
			{
				return a ? 1 : (b ? -1 : 0);
			}
			if (a == b)
			{
				++pos;
			}
			if (a < b)
			{
				return -1;
			}
			if (a > b)
			{
				return 1;
			}
		}
		while (true);
	}

	void String::Replace(unsigned pos, unsigned length, const char *srcStart, unsigned srcLen)
	{
		int delta = srcLen - length;
		if(pos + length < length_)
		{
			if(delta < 0)
			{
				MoveRange(pos + srcLen, pos + length, length_ - pos - length);
				// shrink the space in the end
				Resize(length_ + delta);
			}
			else
			{
				Resize(length_ + delta);
				MoveRange(pos + srcLen, pos + length, length_ - pos - length - delta);
			}
		}
		else
		{
			assert(false);
			//Todo, 会出现这种情况吗？？ 如果会出现，那么这句执行后大小也不对呀, if length == srcLen
			Resize(length_ + delta);
		}
		CopyChars(buffer_ + pos, srcStart, srcLen);
	}

	int String::Compare(const String &str, bool caseSensitive) const
	{
		return Compare(CString(), str.CString(), caseSensitive);
	}

	int String::Compare(const char *str, bool caseSensitive) const
	{
		return Compare(CString(), str, caseSensitive);
	}

	String String::Replaced(const String &replaceThis, const String &replaceWith, bool caseSensitive) const
	{
		String ret(*this);
		ret.Replace(replaceThis, replaceWith, caseSensitive);
		return ret;
	}

	void String::Replace(const String &replaceThis, const String &replaceWith, bool caseSensitive)
	{
		unsigned nextPos = 0;
		while(nextPos < length_)
		{
			unsigned pos = Find(replaceThis, nextPos, caseSensitive);
			if(pos == NPOS)
				break;
			Replace(pos, replaceThis.length_, replaceWith);
			nextPos = pos + replaceWith.length_;
		}
	}

	unsigned String::Find(const String &string, unsigned int startPos, bool caseSensitive) const
	{
		if(!string.length_ || string.length_ > length_)
			return NPOS;

		for(unsigned i = startPos; i<= length_ - string.length_; ++i)
		{
			unsigned j = 0;
			for(j = 0; j< string.length_; ++j)
			{
				char other = string[j];
				char me = this->At(i + j);
				if(!caseSensitive)
				{
					other = (char)tolower(other);
					me = (char)tolower(me);
				}
				if(me != other)
				{
					break;
				}
			}
			if(j >= string.length_)
			{
				return i;
			}
		}
		return NPOS;
	}

	void String::Replace(unsigned pos, unsigned length, const String &replaceWith)
	{
		if(pos + length > length_)
			return;

		Replace(pos, length, replaceWith.buffer_, replaceWith.length_);
	}


	void String::Resize(unsigned newLength)
	{
		if(!capacity_)
		{
			if(!newLength)
				return;

			capacity_ = newLength + 1;
			if(capacity_ < MIN_CAPACITY)
				capacity_ = MIN_CAPACITY;

			buffer_ = new char[capacity_];
		}
		else
		{
			//Only resize capacity when newLength is bigger than old capacity
			if(newLength && capacity_ < newLength + 1)
			{
				while (capacity_ < newLength + 1)
					capacity_ += (capacity_ + 1) >> 1;
				char* newBuff = new char[capacity_];
				CopyChars(newBuff, buffer_, length_);
				delete [] buffer_;
				buffer_ = newBuff;
			}
		}
		// null terminated
		buffer_[newLength] = 0;
		length_ = newLength;
	}

	void String::Reserve(unsigned newCapacity)
	{
		if(newCapacity < length_ + 1)
			newCapacity = length_ + 1;
		if(newCapacity != capacity_)
		{
			char* newBuff = new char[newCapacity];
			//Note, need copy null in the end
			CopyChars(newBuff, buffer_, length_ + 1);
			delete[] buffer_;
			buffer_ = newBuff;
			capacity_ = newCapacity;
		}
	}

	void String::Compact()
	{
		if(capacity_)
			Reserve(length_ + 1);
	}

	void String::Clear()
	{
		Resize(0);
	}

	//Note, used in swap.h
	void String::Swap(String &str)
	{
		Urho3D::Swap(length_, str.length_);
		Urho3D::Swap(capacity_, str.capacity_);
		Urho3D::Swap(buffer_, str.buffer_);
	}

	String String::Replaced(char replaceThis, char replaceWith, bool caseSensitive) const
	{
		String ret(*this);
		ret.Replace(replaceThis, replaceWith, caseSensitive);
		return ret;
	}

	void String::Replace(char replaceThis, char replaceWith, bool caseSensitive)
	{
		unsigned pos = 0;
		while (pos < length_)
		{
			char origin = this->At(pos);
			if(!caseSensitive)
			{
				origin = (char)tolower(origin);
				replaceThis = (char)tolower(replaceThis);
			}

			if(origin == replaceThis)
			{
				buffer_[pos] = replaceWith;
			}
			++pos;
		}
	}

	void String::Replace(unsigned pos, unsigned length, const char *replaceWith)
	{
		if(pos + length > length_)
			return;
		Replace(pos, length, replaceWith, CStringLength(replaceWith));
	}

	String::Iterator
	String::Replace(const String::Iterator &start, const String::Iterator &end, const String &replaceWith)
	{
		unsigned pos = (unsigned)(start - Begin());
		if(pos >= length_)
			return End();

		unsigned length = (unsigned)(end - start);
		Replace(pos, length, replaceWith);
		return Begin() + pos;
	}

	unsigned String::NextUTF8Char(unsigned &byteOffset) const
	{
		return 0;
	}

	unsigned String::LengthUTF8() const
	{
		return 0;
	}

	//Todo, read
	void String::EncodeUTF8(char *&dest, unsigned unicodeChar)
	{
		if (unicodeChar < 0x80)
			*dest++ = unicodeChar;
		else if (unicodeChar < 0x800)
		{
			dest[0] = (char)(0xc0 | ((unicodeChar >> 6) & 0x1f));
			dest[1] = (char)(0x80 | (unicodeChar & 0x3f));
			dest += 2;
		}
		else if (unicodeChar < 0x10000)
		{
			dest[0] = (char)(0xe0 | ((unicodeChar >> 12) & 0xf));
			dest[1] = (char)(0x80 | ((unicodeChar >> 6) & 0x3f));
			dest[2] = (char)(0x80 | (unicodeChar & 0x3f));
			dest += 3;
		}
		else if (unicodeChar < 0x200000)
		{
			dest[0] = (char)(0xf0 | ((unicodeChar >> 18) & 0x7));
			dest[1] = (char)(0x80 | ((unicodeChar >> 12) & 0x3f));
			dest[2] = (char)(0x80 | ((unicodeChar >> 6) & 0x3f));
			dest[3] = (char)(0x80 | (unicodeChar & 0x3f));
			dest += 4;
		}
		else if (unicodeChar < 0x4000000)
		{
			dest[0] = (char)(0xf8 | ((unicodeChar >> 24) & 0x3));
			dest[1] = (char)(0x80 | ((unicodeChar >> 18) & 0x3f));
			dest[2] = (char)(0x80 | ((unicodeChar >> 12) & 0x3f));
			dest[3] = (char)(0x80 | ((unicodeChar >> 6) & 0x3f));
			dest[4] = (char)(0x80 | (unicodeChar & 0x3f));
			dest += 5;
		}
		else
		{
			dest[0] = (char)(0xfc | ((unicodeChar >> 30) & 0x1));
			dest[1] = (char)(0x80 | ((unicodeChar >> 24) & 0x3f));
			dest[2] = (char)(0x80 | ((unicodeChar >> 18) & 0x3f));
			dest[3] = (char)(0x80 | ((unicodeChar >> 12) & 0x3f));
			dest[4] = (char)(0x80 | ((unicodeChar >> 6) & 0x3f));
			dest[5] = (char)(0x80 | (unicodeChar & 0x3f));
			dest += 6;
		}
	}

#define GET_NEXT_CONTINUATION_BYTE(ptr) *ptr; if ((unsigned char)*ptr < 0x80 || (unsigned char)*ptr >= 0xc0) return '?'; else ++ptr;

	//Todo read
	unsigned String::DecodeUTF8(const char *&src)
	{
		if (src == nullptr)
			return 0;

		unsigned char char1 = *src++;

		// Check if we are in the middle of a UTF8 character
		if (char1 >= 0x80 && char1 < 0xc0)
		{
			while ((unsigned char)*src >= 0x80 && (unsigned char)*src < 0xc0)
				++src;
			return '?';
		}

		if (char1 < 0x80)
			return char1;
		else if (char1 < 0xe0)
		{
			unsigned char char2 = GET_NEXT_CONTINUATION_BYTE(src);
			return (unsigned)((char2 & 0x3f) | ((char1 & 0x1f) << 6));
		}
		else if (char1 < 0xf0)
		{
			unsigned char char2 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char3 = GET_NEXT_CONTINUATION_BYTE(src);
			return (unsigned)((char3 & 0x3f) | ((char2 & 0x3f) << 6) | ((char1 & 0xf) << 12));
		}
		else if (char1 < 0xf8)
		{
			unsigned char char2 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char3 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char4 = GET_NEXT_CONTINUATION_BYTE(src);
			return (unsigned)((char4 & 0x3f) | ((char3 & 0x3f) << 6) | ((char2 & 0x3f) << 12) | ((char1 & 0x7) << 18));
		}
		else if (char1 < 0xfc)
		{
			unsigned char char2 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char3 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char4 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char5 = GET_NEXT_CONTINUATION_BYTE(src);
			return (unsigned)((char5 & 0x3f) | ((char4 & 0x3f) << 6) | ((char3 & 0x3f) << 12) | ((char2 & 0x3f) << 18) |
							  ((char1 & 0x3) << 24));
		}
		else
		{
			unsigned char char2 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char3 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char4 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char5 = GET_NEXT_CONTINUATION_BYTE(src);
			unsigned char char6 = GET_NEXT_CONTINUATION_BYTE(src);
			return (unsigned)((char6 & 0x3f) | ((char5 & 0x3f) << 6) | ((char4 & 0x3f) << 12) | ((char3 & 0x3f) << 18) |
							  ((char2 & 0x3f) << 24) | ((char1 & 0x1) << 30));
		}
	}

	String &String::AppendWithFormat(const char *formatString, ...)
	{
		va_list args;
		va_start(args, formatString);
		AppendWithFormatArgs(formatString, args);
		va_end(args);
		return *this;
	}

	String &String::AppendWithFormatArgs(const char *formatString, va_list args)
	{
		int pos = 0, lastPos = 0;
		int length = (int)strlen(formatString);
		while(true)
		{
			while(pos < length && formatString[pos] != '%')
				pos++;

			Append(formatString + lastPos, (unsigned) (pos - lastPos));
			//todo
		}
	}


	WString::WString() :
		length_(0),
		buffer_(nullptr)
	{
	}

	WString::WString(const String &str) :
		length_(0),
		buffer_(nullptr)
	{
#ifdef _WIN32
		unsigned neededSize = 0;
		wchar_t temp[3];
		unsigned byteOffset = 0;
		while(byteOffset < str.Length())
		{
			wchar_t* dest = temp;
			String::EncodeUTF16(dest, str.NextUTF8Char(byteOffset));
			neededSize += dest - temp;
		}

		Resize(neededSize);

		byteOffset = 0;
		wchar_t* dest = buffer_;
		while(byteOffset < str.Length())
		{
			String::EncodeUTF16(dest, str.NextUTF8Char(byteOffset));
		}
#else
		Resize(str.LengthUTF8());

		unsigned byteOffset = 0;
		wchar_t* dest = buffer_;
		while(byteOffset < str.Length())
		{
			*dest++ = (wchar_t)str.NextUTF8Char(byteOffset);
		}

#endif
	}

	WString::~WString()
	{
		delete [] buffer_;
	}

	void WString::Resize(unsigned newLength)
	{
		if (!newLength)
		{
			delete[] buffer_;
			buffer_ = nullptr;
			length_ = 0;
		}
		else
		{
			//Note ,Null terminated
			wchar_t* newBuffer = new wchar_t[newLength + 1];
			if(buffer_)
			{
				unsigned copyLength = length_ < newLength ? length_ : newLength;
				memcpy(newBuffer, buffer_, copyLength * sizeof(wchar_t));
				delete[] buffer_;
			}
			newBuffer[newLength] = 0;
			buffer_ = newBuffer;
			length_ = newLength;
		}
	}
}