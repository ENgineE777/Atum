
#pragma once

#include <math.h>

/**
\ingroup gr_code_common
*/

/**
\brief 2 component vector

Implementation of a vector with 2 components.

*/

struct Vector2
{
public:
	union
	{
		struct
		{
			float x;
			float y;
		};
		struct
		{
			float v[2];
		};
	};

public:

	Vector2();
	Vector2(float f);
	Vector2(double d);
	Vector2(float x, float y);
	Vector2(float f[2]);
	Vector2(double d[2]);

public:

	bool operator == (const Vector2& v);

	Vector2& operator = (float f);
	Vector2& operator = (double d);
	Vector2& operator = (const Vector2& v);

	Vector2& operator *= (float f);
	Vector2& operator *= (double d);
	Vector2& operator *= (const Vector2& v);

	Vector2& operator /= (float f);
	Vector2& operator /= (double d);
	Vector2& operator /= (const Vector2& v);

	Vector2& operator += (float f);
	Vector2& operator += (double d);
	Vector2& operator += (const Vector2& v);

	Vector2& operator -= (float f);
	Vector2& operator -= (double d);
	Vector2& operator -= (const Vector2& v);

public:

	float Normalize();
	float Length();
	float Length(Vector2 v);
	float Length2();
	float Length2(Vector2 v);

	bool InSquare(Vector2 min, Vector2 max);
	
	float Dot(Vector2 v);
	float Cross(Vector2 v);
	Vector2& Lerp(Vector2 from, Vector2 to, float kBlend);
};

inline Vector2::Vector2(){}

inline bool Vector2::operator == (const Vector2& v)
{
	if (fabs(x - v.x) < 0.01f && fabs(y - v.y) < 0.01f)
	{
		return true;
	}

	return false;
}

inline Vector2::Vector2(float f)
{
	x = y = f;
}

inline Vector2::Vector2(double d)
{
	x = y = float(d);
}

inline Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

inline Vector2::Vector2(float f[2])
{
	x = f[0];
	y = f[1];
}

inline Vector2::Vector2(double d[2])
{
	x = float(d[0]);
	y = float(d[1]);
}

inline Vector2& Vector2::operator = (float f)
{
	x = y = f;
	return *this;
}

inline Vector2& Vector2::operator = (double d)
{
	x = y = float(d);
	return *this;
}

inline Vector2& Vector2::operator = (const Vector2 & v)
{
	this->x = v.x;
	this->y = v.y;
	return *this;
}

inline float Vector2::Normalize()
{
	float len = sqrtf(x * x + y * y);
	float k = 1.0f / len;
	x = float(k*x);
	y = float(k*y);

	return len;
}

inline float Vector2::Length()
{
	return sqrtf(x*x + y*y);
}

inline float Vector2::Length(Vector2 v)
{
	float dx = x - v.x;
	float dy = y - v.y;
	return ::sqrtf(dx * dx + dy * dy);
}

inline float Vector2::Length2()
{
	return x * x + y * y;
}

inline float Vector2::Length2(Vector2 v)
{
	float dx = x - v.x;
	float dy = y - v.y;
	return dx * dx + dy * dy;
}


inline Vector2 & Vector2::operator *= (float f)
{
	x *= f;
	y *= f;
	return *this;
}

inline Vector2 & Vector2::operator *= (double d)
{
	x *= float(d);
	y *= float(d);
	return *this;
}

inline Vector2 & Vector2::operator *= (const Vector2 & v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

inline Vector2 & Vector2::operator /= (float f)
{
	x /= f;
	y /= f;
	return *this;
}

inline Vector2 & Vector2::operator /= (double d)
{
	x /= float(d);
	y /= float(d);
	return *this;
}

inline Vector2 & Vector2::operator /= (const Vector2 & v)
{
	x /= v.x;
	y /= v.y;
	return *this;
}

inline Vector2 & Vector2::operator += (float f)
{
	x += f;
	y += f;	
	return *this;
}

inline Vector2 & Vector2::operator += (double d)
{
	x += float(d);
	y += float(d);
	return *this;
}

inline Vector2 & Vector2::operator += (const Vector2 & v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline Vector2 & Vector2::operator -= (float f)
{
	x -= f;
	y -= f;	
	return *this;
}

inline Vector2 & Vector2::operator -= (double d)
{
	x -= float(d);
	y -= float(d);
	return *this;
}

inline Vector2& Vector2::operator -= (const Vector2 & v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline Vector2 operator * (const Vector2 & v, double d)
{
	Vector2 tv(v);
	tv *= d;
	return tv;
}

inline Vector2 operator * (double d, const Vector2 & v)
{
	Vector2 tv(v);
	tv *= d;
	return tv;
}

inline Vector2 operator * (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv *= v2;
	return tv;
}

inline Vector2 operator / (const Vector2 & v, double d)
{
	Vector2 tv(v);
	tv /= d;
	return tv;
}

inline Vector2 operator / (double d, const Vector2 & v)
{
	Vector2 tv(d);
	tv /= v;
	return tv;
}

inline Vector2 operator / (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv /= v2;
	return tv;
}

inline Vector2 operator + (const Vector2 & v, double d)
{
	Vector2 tv(v);
	tv += d;
	return tv;
}

inline Vector2 operator + (double d, const Vector2 & v)
{
	Vector2 tv(v);
	tv += d;
	return tv;
}

inline Vector2 operator + (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv += v2;
	return tv;
}

inline Vector2 operator - (const Vector2 & v, double d)
{
	Vector2 tv(v);
	tv -= d;
	return tv;
}

inline Vector2 operator - (double d, const Vector2 & v)
{
	Vector2 tv(d);
	tv -= v;
	return tv;
}

inline Vector2 operator - (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv -= v2;
	return tv;
}

inline bool Vector2::InSquare(Vector2 min, Vector2 max)
{
	if (y < min.y || y > max.y) return false;
	if (x < min.x || x > max.x) return false;
	return true;
}

inline float Vector2::Dot(Vector2 v)
{
	return x * v.x + y * v.y;
}

inline float Vector2::Cross(Vector2 v)
{
	return x * v.y - y * v.x;
}

inline Vector2 & Vector2::Lerp(Vector2 from, Vector2 to, float kBlend)
{
	x = from.x + (to.x - from.x) * kBlend;
	y = from.y + (to.y - from.y) * kBlend;
	return *this;
}