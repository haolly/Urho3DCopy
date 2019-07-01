//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_STRINGHASH_H
#define URHO3DCOPY_STRINGHASH_H

#include "../Container/Str.h"

namespace Urho3D
{
	/**
	 *
	 * Note, StringHash's semantic is **value type**
	 */
    class StringHash
    {
    public:
        StringHash() :
                value_(0)
        {

        }

        StringHash(const StringHash& rhs) :
                value_(rhs.value_)
        {

        }

        explicit StringHash(unsigned value) :
                value_(value)
        {

        }

	    explicit StringHash(const char* str);

        explicit StringHash(const String& str);

        StringHash&operator=(const StringHash& rhs)
        {
            value_ = rhs.value_;
            return *this;
        }



        StringHash operator+(const StringHash& rhs) const
        {
            StringHash ret;
            ret.value_ = value_ + rhs.value_;
            return ret;
        }

        StringHash&operator+=(const StringHash& rhs)
        {
            value_ += rhs.value_;
            return *this;
        }

        bool operator ==(const StringHash& rhs) const
        {
            return value_ == rhs.value_;
        }

        bool  operator !=(const StringHash& rhs) const
        {
            return value_ != rhs.value_;
        }

        bool  operator <(const StringHash& rhs) const
        {
            return value_ < rhs.value_;
        }

        bool operator >(const StringHash& rhs) const
        {
            return value_ > rhs.value_;
        }

        operator bool() const
        {
            return value_ != 0;
        }

        unsigned Value() const {return value_;}

        String ToString() const;

        unsigned ToHash() const {return value_;}

        static unsigned Calculate(const char* str, unsigned hash=0);

        static const StringHash ZERO;

    private:
        unsigned value_;
    };
}


#endif //URHO3DCOPY_STRINGHASH_H
