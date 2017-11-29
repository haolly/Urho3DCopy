//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_STRINGHASH_H
#define URHO3DCOPY_STRINGHASH_H

namespace Urho3D
{
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

        StringHash&operator=(const StringHash& rhs)
        {
            value_ = rhs.value_;
            return *this;
        }

        explicit StringHash(unsigned value) :
                value_(value)
        {

        }

        StringHash(const char* str);

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

        unsigned ToHash() const {return value_;}

        static unsigned Calculate(const char* str, unsigned hash=0);

        static const StringHash ZERO;

    private:
        unsigned value_;
    };
}


#endif //URHO3DCOPY_STRINGHASH_H
