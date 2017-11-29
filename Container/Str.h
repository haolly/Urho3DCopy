//
// Created by liuhao on 2017/11/28.
//

#ifndef URHO3DCOPY_STR_H
#define URHO3DCOPY_STR_H

#include <cstring>
#include "VectorBase.h"

namespace Urho3D
{
    //null terminated string
    class String
    {
    public:
        using Iterator = RandomAccessIterator<char>;
        using ConstIterator = RandomAccessConstIterator<char>;

        String() : length_(0), capacity_(0), buffer_(&endZero)
        {
        }

        String(const String& str) : length_(0), capacity_(0), buffer_(&endZero)
        {
            //call assignment operator
            *this = str;
        }

        String(const char* str) :
                length_(0),
                capacity_(0),
                buffer_(&endZero)
        {
            //call assignment operator
            *this = str;
        }

        String(char* str) : length_(0), capacity_(0), buffer_(&endZero)
        {
            *this = (const char*)str;
        }

        String(const char* str, unsigned length) :
                length_(0),
                capacity_(0),
                buffer_(&endZero)
        {
            Resize(length);
            CopyChars(buffer_, str, length);
        }

        String(const wchar_t* str) :
            length_(0),
            capacity_(0),
            buffer_(&endZero)
        {
            SetUTF8FromWChar(str);
        }

        String(wchar_t* str) :
            length_(0),
            capacity_(0),
            buffer_(&endZero)
        {
            SetUTF8FromWChar(str);
        }

        String(const WString& str);

        explicit String(int value);
        explicit String(short value);
        explicit String(long value);
        explicit String(long long value);
        explicit String(unsigned value);
        explicit String(unsigned short value);
        explicit String(unsigned long value);
        explicit String(unsigned long long value);
        explicit String(float value);
        explicit String(double value);
        explicit String(bool value);
        explicit String(char value);
        explicit String(char value, unsigned length);


        template <class T>
        explicit String(const T& value) :
                length_(0),
                capacity_(0),
                buffer_(&endZero)
        {
            *this = value.ToString();
        }

        // Note ,the big three
        ~String()
        {
            if(capacity_)
                delete[] buffer_;
        }

        String&operator =(const String& rhs)
        {
            Resize(rhs.length_);
            CopyChars(buffer_, rhs.buffer_, rhs.length_);
            return *this;
        }

        String&operator =(const char* str)
        {
            unsigned rhsLength = CStringLength(rhs);
            Resize(rhsLength);
            CopyChars(buffer_, str, rhsLength);
            return *this;
        }

        String&operator +=(const String& rhs)
        {
            unsigned oldLength = length_;
            Resize(length_ + rhs.length_);
            CopyChars(buffer_ + oldLength, rhs.buffer_, rhs.length_);
            return *this;
        }

        String&operator +=(const char* rhs)
        {
            unsigned rhsLength = CStringLength(rhs);
            unsigned oldLength = length_;
            Resize(length_ + rhsLength);
            CopyChars(buffer_ + oldLength, rhs, rhsLength);
            return *this;
        }

        String&operator +=(char rhs)
        {
            unsigned oldLength = length_;
            Resize(length_ + 1);
            buffer_[oldLength] = rhs;
            return *this;
        }

        String&operator +=(int rhs);
        String&operator +=(short rhs);
        String&operator +=(long rhs);
        String&operator +=(long long rhs);
        String&operator +=(unsigned rhs);
        String&operator +=(unsigned short rhs);
        String&operator +=(unsigned long rhs);
        String&operator +=(unsigned long long rhs);
        String&operator +=(float rhs);
        String&operator +=(bool  rhs);

        template <class T>
        String operator +=(const T& rhs)
        {
            return *this += rhs.ToString();
        }

        //Add a String, return a new string
        String operator+ (const String& rhs) const
        {
            String ret;
            ret.Resize(length_ + rhs.length_);
            CopyChars(ret.buffer_, buffer_, length_);
            CopyChars(ret.buffer_ + length_, rhs.buffer_, rhs.length_);
            return ret;
        }

        String operator+ (const char* rhs) const
        {
            unsigned rhsLenght = CStringLength(rhs);
            String ret;
            ret.Resize(length_ + rhsLenght);
            CopyChars(ret.buffer_, buffer_, length_);
            CopyChars(ret.buffer_ + length_, rhs, rhsLenght);
            return ret;
        }

        //todo


        //Note, this determinate whether the String is null terminated
        void Resize(unsigned newLenght);
        void Reserve(unsigned newCapacity);
        void Compact();
        void Clear();
        void Swap(String& str);


        void SetUTF8FromLatin1(const char* str);
        void SetUTF8FromWChar(const wchar_t* str);
        unsigned LengthUTF8() const;
        unsigned ByteOffsetUTF8(unsigned index) const;
        unsigned NextUTF8Char(unsigned& byteOffset) const;
        unsigned AtUTF8(unsigned index) const;
        void ReplaceUTF8(unsigned index, unsigned unicodeChar);
        String& AppendUTF8(unsigned unicodeChar);
        String SubstringUTF8(unsigned pos) const;
        String SubstringUTF8(unsigned pos, unsigned length) const;




        unsigned ToHash() const
        {
            unsigned hash = 0;
            const char* ptr = buffer_;
            while(*ptr)
            {
                hash = *ptr + (hash << 6) + (hash << 16) - hash;
                ++ptr;
            }
            return hash;
        }

        //todo

    private:
        void MoveRange(unsigned dest, unsigned src, unsigned count)
        {
            if(count)
                memmove(buffer_ + dest, buffer_ + src, count);
        }

        static void CopyChars(char* dest, const char* src, unsigned count)
        {
#ifdef _MSC_VER
            if(count)
                memcpy(dest, src, count);
#else
            char* end = dest + count;
            while (dest != end)
            {
                *dest = *src;
                ++dest;
                ++src;
            }
#endif
        }
        void Replace(unsigned pos, unsigned length, const char* srcStart, unsigned srcLen);
        unsigned length_;
        unsigned capacity_;
        char* buffer_;
        static char endZero;
    };
}

class WString
{
    //todo
};



#endif //URHO3DCOPY_STR_H
