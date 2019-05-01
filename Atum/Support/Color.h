
#pragma once

#include <stdint.h>

/**
\ingroup gr_code_common
*/


class Color
{	
public:

	float r, g, b, a;

	Color()
	{
		r = g = b = a = 1.0f;
	};

	Color(float R, float G, float B)
	{
		Set(R,G,B,1.0f);
	};

	Color(float c)
	{
		Set(c, c, c, 1.0f);
	};

	Color(const Color& other)
	{
		Set(other.r,other.g,other.b,other.a);
	}

	Color(uint32_t color)
	{
		Set(color);
	};

	Color(float R, float G, float B, float A)
	{
		Set(R,G,B,A);
	};

	void Set(float R, float G, float B, float A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	};

	void Set(uint32_t color)
	{
		a = (color >> 24) / 255.0f;
		b = ((color >> 16) & 0xff) / 255.0f;
		g = ((color >> 8) & 0xff) / 255.0f;
		r = ((color) & 0xff) / 255.0f;	
	};

	uint32_t Get(bool revert = false)
	{
		return (((uint8_t)(a*255))<<24) | (((uint8_t)(b*255))<<16) | (((uint8_t)(g*255))<<8) | ((uint8_t)(r*255));
	};

	void Lerp(Color& from, Color& to, float blend)
	{
		r = from.r + (to.r - from.r) * blend;
		g = from.g + (to.g - from.g) * blend;
		b = from.b + (to.b - from.b) * blend;
		a = from.a + (to.a - from.a) * blend;
	}

	Color& operator *= (const Color& color)
	{
		r *= color.r;
		g *= color.g;
		b *= color.b;
		a *= color.a;

		return *this;
	};

	Color& operator = (const Color& other)
	{
		Set(other.r,other.g,other.b,other.a);
		return *this;
	};

	Color& operator = (uint32_t color)
	{
		Set(color);
		return *this;
	};
};

inline Color operator * (const Color& c1, const Color& c2)
{
	Color c(c1);
	c *= c2;
	return c;
}

#define COLOR_WHITE Color()
#define COLOR_WHITE_A(alpha) Color(1.0, 1.0f, 1.0f, alpha)

#define COLOR_BLACK Color(0.0f, 0.0f, 0.0f)
#define COLOR_BLACK_A(alpha) Color(0.0f, 0.0f, 0.0f, alpha)

#define COLOR_GRAY Color(0.4f, 0.4f, 0.4f)
#define COLOR_GRAY_A(alpha) Color(0.4f, 0.4f, 0.4f, alpha)

#define COLOR_LIGHT_GRAY Color(0.65f, 0.65f, 0.65f)
#define COLOR_LIGHT_GRAY_A(alpha) Color(0.65f, 0.65f, 0.65f, alpha)

#define COLOR_RED Color(1.0f, 0.0f, 0.0f)
#define COLOR_RED_A(alpha) Color(1.0f, 0.0f, 0.0f, alpha)

#define COLOR_GREEN Color(0.0f, 1.0f, 0.0f)
#define COLOR_GREEN_A(alpha) Color(0.0f, 1.0f, 0.0f, alpha)

#define COLOR_BLUE Color(0.0f, 0.0f, 1.0f)
#define COLOR_BLUE_A(alpha) Color(0.0f, 0.0f, 1.0f, alpha)


#define COLOR_YELLOW Color(1.0f, 1.0f, 0.0f)
#define COLOR_YELLOW_A(alpha) Color(1.0f, 1.0f, 0.0f, alpha)

#define COLOR_MAGNETA Color(1.0f, 0.0f, 1.0f)
#define COLOR_MAGNETA_A(alpha) Color(1.0f, 0.0f, 1.0f, alpha)

#define COLOR_CYAN Color(0.0f, 1.0f, 1.0f)
#define COLOR_CYAN_A(alpha) Color(0.0f, 1.0f, 1.0f, alpha)
