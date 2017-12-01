//
// Created by liuhao on 2017/11/27.
//

#include <cctype>
#include "StringHash.h"
#include "MathDefs.h"

namespace Urho3D
{
    const StringHash StringHash::ZERO;

    StringHash::StringHash(const char *str):
        value_(Calculate(str))
    {
    }

    unsigned StringHash::Calculate(const char *str, unsigned int hash)
    {
        if(!str)
        {
            return hash;
        }

        while(*str)
        {
            char c = *str;
            hash = SDBMHash(hash, (unsigned char)tolower(c));
            ++str;
        }
        return hash;
    }

    StringHash::StringHash(const String &str) :
        value_(Calculate(str.CString()))
    {

    }
}