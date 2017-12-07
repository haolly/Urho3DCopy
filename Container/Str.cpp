//
// Created by liuhao on 2017/11/28.
//

#include <cctype>
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

}