//
// Created by LIUHAO on 2017/12/5.
//

#ifndef URHO3DCOPY_PAIR_H
#define URHO3DCOPY_PAIR_H

#include "../Container/Hash.h"

namespace Urho3D
{
	template <class T, class U>
	class Pair
	{
	public:
		Pair()
		{
		}

		Pair(const T& first, const U& second) :
				first_(first),
				second_(second)
		{
		}

		bool operator==(const Pair<T, U>& rhs) const
		{
			return first_ == rhs.first_ && second_ == rhs.second_;
		}

		bool operator!=(const Pair<T, U>& rhs) const
		{
			return first_ != rhs.first_ || second_ != rhs.second_;
		}

		bool operator <(const Pair<T, U>& rhs) const
		{
			if(first_ < rhs.first_)
				return true;
			if(first_ != rhs.first_)
				return false;
			return second_ < rhs.second_;
		}

		bool operator >(const Pair<T, U>& rhs) const
		{
			if(first_ > rhs.first_)
				return true;
			if(first_ < rhs.first_)
				return false;
			return second_ > rhs.second_;
		}

		unsigned ToHash() const
		{
			return (MakeHash(first_) & 0xffff) | (MakeHash(second_) << 16);
		}

		T first_;
		U second_;
	};

	template <class T, class U>
	Pair<T, U> MakePair(const T& first, const U& second)
	{
		return Pair<T, U>(first, second);
	};

	template <class T>
	T begin(Pair<T, T>& range) { return range.first_; }

	template <class T>
	T end(Pair<T, T>& range) { return range.second_; }

	template <class T>
	T begin(const Pair<T, T>& range) { return range.first_; }

	template <class T>
	T end(const Pair<T, T>& range) { return range.second_; }
}

#endif //URHO3DCOPY_PAIR_H
