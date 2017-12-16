//
// Created by LIUHAO on 2017/12/14.
//

#include "Color.h"
#include "MathDefs.h"

namespace Urho3D
{

	unsigned Color::ToUint() const
	{
		unsigned r = (unsigned)Clamp((int)(r_ * 255.0f), 0, 255);
		unsigned g = (unsigned)Clamp((int)(g_ * 255.0f), 0, 255);
		unsigned b = (unsigned)Clamp((int)(b_ * 255.0f), 0, 255);
		unsigned a = (unsigned)Clamp((int)(a_ * 255.0f), 0, 255);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}

	Vector3 Color::ToHSL() const
	{
		return nullptr;
	}

	Vector3 Color::ToHSV() const
	{
		return nullptr;
	}

	void Color::FromUInt(unsigned color)
	{
		a_ = ((color >> 24) & 0xff) / 255.0f;
		b_ = ((color >> 16) & 0xff) / 255.0f;
		g_ = ((color >> 8) & 0xff) / 255.0f;
		r_ = ((color >> 0) & 0xff) / 255.0f;
	}

	void Color::FromHSL(float h, float s, float l, float a)
	{
		float c;
		if (l < 0.5f)
			c = (1.0f + (2.0f * l - 1.0f)) * s;
		else
			c = (1.0f - (2.0f * l - 1.0f)) * s;

		float m = l - 0.5f * c;

		FromHCM(h, c, m);

		a_ = a;
	}

	void Color::FromHSV(float h, float s, float v, float a)
	{
		float c = v * s;
		float m = v - c;

		FromHCM(h, c, m);

		a_ = a;
	}

	float Color::Hue(float min, float max) const
	{
		float chroma = max - min;
		if(chroma <= M_EPSILON)
			return 0.0f;

		if(Urho3D::Equals(g_, max))
			return (b_ + 2.0f * chroma - r_) / (6.0f * chroma);
		else if(Urho3D::Equals(b_, max))
			return (4.0f * chroma - g_ + r_) / (6.0f * chroma);
		else
		{
			float r = (g_ - b_) / (6.0f * chroma);
			return (r < 0.0f) ? 1.0f + r : (r >= 1.0f ? r - 1.0f : r);
		}
	}

	float Color::SaturationHSV(float min, float max) const
	{
		if(max <= M_EPSILON)
			return 0.0f;
		return 1.0f - (min/max);
	}

	float Color::SaturationHSL(float min, float max) const
	{
		// Avoid div-by-zero: result undefined
		if (max <= M_EPSILON || min >= 1.0f - M_EPSILON)
			return 0.0f;

		// Chroma = max - min, lightness = (max + min) * 0.5
		float hl = (max + min);
		if (hl <= 1.0f)
			return (max - min) / hl;
		else
			return (min - max) / (hl - 2.0f);
	}

	void Color::FromHCM(float h, float c, float m)
	{
		if (h < 0.0f || h >= 1.0f)
			h -= floorf(h);

		float hs = h * 6.0f;
		float x = c * (1.0f - Urho3D::Abs(fmodf(hs, 2.0f) - 1.0f));

		// Reconstruct r', g', b' from hue
		if (hs < 2.0f)
		{
			b_ = 0.0f;
			if (hs < 1.0f)
			{
				g_ = x;
				r_ = c;
			}
			else
			{
				g_ = c;
				r_ = x;
			}
		}
		else if (hs < 4.0f)
		{
			r_ = 0.0f;
			if (hs < 3.0f)
			{
				g_ = c;
				b_ = x;
			}
			else
			{
				g_ = x;
				b_ = c;
			}
		}
		else
		{
			g_ = 0.0f;
			if (hs < 5.0f)
			{
				r_ = x;
				b_ = c;
			}
			else
			{
				r_ = c;
				b_ = x;
			}
		}

		r_ += m;
		g_ += m;
		b_ += m;
	}

	const Color Color::WHITE;
	const Color Color::GRAY(0.5f, 0.5f, 0.5f);
	const Color Color::BLACK(0.0f, 0.0f, 0.0f);
	const Color Color::RED(1.0f, 0.0f, 0.0f);
	const Color Color::GREEN(0.0f, 1.0f, 0.0f);
	const Color Color::BLUE(0.0f, 0.0f, 1.0f);
	const Color Color::CYAN(0.0f, 1.0f, 1.0f);
	const Color Color::MAGENTA(1.0f, 0.0f, 1.0f);
	const Color Color::YELLOW(1.0f, 1.0f, 0.0f);
	const Color Color::TRANSPARENT(0.0f, 0.0f, 0.0f, 0.0f);
}