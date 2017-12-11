//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_MATHDEFS_H
#define URHO3DCOPY_MATHDEFS_H

#include <cstdlib>
#include <cmath>
#include <limits>

namespace Urho3D
{
#undef M_PI
    static const float M_PI = 3.14159265358979323846264338327950288f;
    static const float M_HALF_PI = M_PI * 0.5f;
    static const int M_MIN_INT = 0x80000000;
    static const int M_MAX_INT = 0x7fffffff;

    static const unsigned M_MIN_UNSIGNED = 0x00000000;
	static const unsigned M_MAX_UNSIGNED = 0xffffffff;

    static const float M_EPSILON = 0.000001f;
    static const float M_MAX_FOV = 160.0f;

    inline unsigned SDBMHash(unsigned hash, unsigned char c)
    {
        return c+(hash << 6) + (hash << 16) - hash;
    }

	//todo
	template <class T, class U>
	inline T Min(T lhs, U rhs)
	{
		return lhs < rhs ? lhs : rhs;
	};

	template <class T, class U>
	inline T Max(T lhs, U rhs)
	{
		return lhs > rhs ? lhs : rhs;
	};
}

#endif //URHO3DCOPY_MATHDEFS_H
