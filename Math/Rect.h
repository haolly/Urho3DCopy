//
// Created by liuhao1 on 2018/1/22.
//

#ifndef URHO3DCOPY_RECT_H
#define URHO3DCOPY_RECT_H

#include "Vector2.h"

namespace Urho3D
{
	class Rect
	{
	public:
		//todo

		static const Rect FULL;
		static const Rect POSITIVE;
		static const Rect ZERO;

	};

	class IntRect
	{
	public:
		IntRect() :
			left_(0),
			top_(0),
			right_(0),
			bottom_(0)
		{
		}

		IntRect(const IntVector2& min, const IntVector2& max) :
			left_(min.x_),
			top_(min.y_),
			right_(max.x_),
			bottom_(max.y_)
		{
		}

		IntRect(int left, int top, int right, int bottom) :
			left_(left),
			top_(top),
			right_(right),
			bottom_(bottom)
		{
		}

		bool operator==(const IntRect& rhs) const
		{
			return left_ == rhs.left_ && top_ == rhs.top_ && right_ == rhs.right_ && bottom_ == rhs.bottom_;
		}

		bool operator !=(const IntRect& rhs) const
		{
			return !(*this == rhs);
		}

		IntRect&operator +=(const IntRect& rhs)
		{
			left_ += rhs.left_;
			top_ += rhs.top_;
			right_ += rhs.right_;
			bottom_ += rhs.bottom_;
			return *this;
		}

		IntRect&operator -=(const IntRect& rhs)
		{
			left_ -= rhs.left_;
			top_ -= rhs.top_;
			right_ -= rhs.right_;
			bottom_ -= rhs.bottom_;
			return *this;
		}

		IntRect&operator /=(float value)
		{
			left_ = static_cast<int>(left_ / value);
			top_ = static_cast<int>(top_ / value);
			right_ = static_cast<int>(right_ / value);
			bottom_ = static_cast<int>(bottom_ / value);
			return *this;
		}

		//todo

		IntVector2 Size() { return IntVector2(Width(), Height()); }

		int Width() const { return right_ - left_; }
		int Height() const { return bottom_ - top_; }

		int left_;
		int top_;
		int right_;
		int bottom_;

		static const IntRect ZERO;
	};

}


#endif //URHO3DCOPY_RECT_H
