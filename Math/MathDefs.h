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
	static const float M_LARGE_EPSILON = 0.00005f;
	static const float M_MIN_NEARCLIP = 0.01f;
    static const float M_MAX_FOV = 160.0f;
	static const float M_LARGE_VALUE = 100000000.0f;
	static const float M_INFINITY = (float)HUGE_VAL;
	static const float M_DEGTORAD = M_PI / 180.0f;
	static const float M_DEGTORAD_2 = M_PI / 360.0f;
	static const float M_RADTODEG = 1.0f / M_DEGTORAD;

	enum Intersection
	{
		OUTSIDE,
		INTERSECTS,
		INSIDE
	};


	template <class T>
	inline bool Equals(T lhs, T rhs)
	{
		return lhs + std::numeric_limits<T>::epsilon() >= rhs && lhs - std::numeric_limits<T>::epsilon() <= rhs;
	}


	//todo



	template <class T>
	inline T Abs(T value)
	{
		return value >= 0.0 ? value : -value;
	}

	template <class T>
	inline T Sign(T value)
	{
		return value > 0.0 ? 1.0 : (value < 0.0 ? -1.0 : 0.0);
	}

	inline unsigned FloatToRawIntBits(float value)
	{
		unsigned u = *((unsigned*)&value);
		return u;
	}

#ifndef __GNUC__
	inline bool IsNaN(float value)
	{
		return value != value;
	}
#else
	//Note, see https://stackoverflow.com/questions/570669/checking-if-a-double-or-float-is-nan-in-c
	inline bool IsNaN(float value)
	{
		unsigned u = FloatToRawIntBits(value);
		return (u & 0x7fffffff) > 0x7f800000;
	}
#endif

	template <class T>
	inline T Clamp(T value, T min, T max)
	{
		if(value < min)
			return min;
		else if(value > max)
			return max;
		else
			return value;
	}

	template <class T>
	inline T Sin(T angle)
	{
		return sin(angle * M_DEGTORAD);
	}

	template <class T>
	inline T Cos(T angle)
	{
		return cos(angle * M_DEGTORAD);
	}

	template <class T>
	inline T Tan(T angle)
	{
		return tan(angle * M_DEGTORAD);
	}

	template <class T>
	inline T Asin(T x)
	{
		return M_RADTODEG * asin(Clamp(x, T(-1.0), T(1.0)));
	}

	template <class T>
	inline T Acos(T x)
	{
		return M_RADTODEG * acos(Clamp(x, T(-1.0), T(1.0)));
	}

	template <class T>
	inline T Atan(T x)
	{
		return M_RADTODEG * atan(x);
	}

	template <class T>
	inline T Atan2(T y, T x)
	{
		return M_RADTODEG * atan2(y, x);
	}

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
