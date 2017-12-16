//
// Created by LIUHAO on 2017/12/14.
//

#ifndef URHO3DCOPY_COLOR_H
#define URHO3DCOPY_COLOR_H

namespace Urho3D
{
	class String;

	//Note, it's a no-trivial class, because it has user defined constructor
	class Color
	{
	public:
		Color() :
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(1.0f)
		{
		}

		Color(const Color& color) :
				r_(color.r_),
				g_(color.g_),
				b_(color.b_),
				a_(color.a_)
		{
		}

		Color(const Color& color, float a) :
				r_(color.r_),
				g_(color.g_),
				b_(color.b_),
				a_(a)
		{
		}

		Color(float r, float g, float b) :
				r_(r),
				g_(g),
				b_(b),
				a_(1.0f)
		{
		}

		Color(float r, float g, float b, float a) :
				r_(r),
				g_(g),
				b_(b),
				a_(a)
		{
		}

		explicit Color(const float* data) :
				r_(data[0]),
				g_(data[1]),
				b_(data[2]),
				a_(data[3])
		{
		}

		Color&operator =(const Color& rhs)
		{
			r_ = rhs.r_;
			g_ = rhs.g_;
			b_ = rhs.b_;
			a_ = rhs.a_;
			return *this;
		}

		bool operator ==(const Color& rhs) const
		{
			return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_;
		}

		bool operator !=(const Color& rhs) const
		{
			return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_;
		}

		Color operator *(float rhs) const
		{
			return Color(r_ * rhs, g_ * rhs, b_ * rhs, a_ * rhs);
		}

		Color operator +(const Color& rhs) const
		{
			return Color(r_ + rhs.r_, g_ + rhs.g_, b_ + rhs.b_, a_ + rhs.a_);
		}

		Color operator -() const
		{
			return Color(-r_, -g_, -b_, -a_);
		}

		Color operator -(const Color& rhs) const
		{
			return Color(r_ - rhs.r_, g_ - rhs.g_, b_ - rhs.b_, a_- rhs.a_);
		}

		Color&operator +=(const Color& rhs)
		{
			r_ += rhs.r_;
			g_ += rhs.g_;
			b_ += rhs.b_;
			a_ += rhs.a_;
			return *this;
		}

		//todo, not trival class could do this ???
		const float* Data() const
		{
			return &r_;
		}

		unsigned ToUint() const ;

		Vector3 ToHSL() const;
		Vector3 ToHSV() const;

		void FromUInt(unsigned color);
		void FromHSL(float h, float s, float l, float a = 1.0f);
		void FromHSV(float h, float s, float v, float a = 1.0f);

		//todo

		float r_;
		float g_;
		float b_;
		float a_;

		static const Color WHITE;
		static const Color GRAY;
		static const Color BLACK;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color CYAN;
		static const Color MAGENTA;
		static const Color YELLOW;
		static const Color TRANSPARENT;

	protected:
		float Hue(float min, float max) const;
		float SaturationHSV(float min, float max) const;
		float SaturationHSL(float min, float max) const;
		void FromHCM(float h, float c, float m);
	};

	inline Color operator *(float lhs, const Color& rhs)
	{
		return rhs * lhs;
	}
}


#endif //URHO3DCOPY_COLOR_H
