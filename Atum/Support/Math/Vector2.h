
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

		float v[2];
	};

	Vector2()
	{
		Set(0.0f);
	}

	Vector2(float f)
	{
		Set(f);
	}

	Vector2(float x, float y)
	{
		Set(x, y);
	}

	void Set(float f)
	{
		x = y = f;
	}

	void Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Set(Vector2 v)
	{
		x = v.x;
		y = v.y;
	}

	Vector2 operator - () const
	{
		Vector2 v(*this);
		v.x = -v.x;
		v.y = -v.y;
		return v;
	}

	bool operator == (const Vector2& v)
	{
		return ((fabs(x - v.x) < 0.001f) && (fabs(y - v.y) < 0.001f));
	}

	Vector2& operator = (float f)
	{
		x = f;
		y = f;
		return *this;
	}

	Vector2& operator = (const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	Vector2& operator += (float f)
	{
		x += f;
		y += f;
		return *this;
	}

	Vector2& operator += (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2& operator -= (float f)
	{
		x -= f;
		y -= f;
		return *this;
	}

	Vector2& operator -= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2& operator *= (float f)
	{
		x *= f;
		y *= f;
		return *this;
	}

	Vector2& operator *= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2& operator /= (float f)
	{
		float d = 1.0f / f;
		x = float(x * d);
		y = float(y * d);
		return *this;
	}

	Vector2& operator /= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	friend Vector2 operator + (const Vector2& v, float f)
	{
		Vector2 tv(v);
		tv += f;
		return tv;
	}

	friend Vector2 operator + (float f, const Vector2& v)
	{
		Vector2 tv(f);
		tv += v;
		return tv;
	}

	friend Vector2 operator + (const Vector2& v1, const Vector2& v2)
	{
		Vector2 tv(v1);
		tv += v2;
		return tv;
	}

	friend Vector2 operator - (const Vector2& v, float f)
	{
		Vector2 tv(v);
		tv -= f;
		return tv;
	}

	friend Vector2 operator - (float f, const Vector2& v)
	{
		Vector2 tv(f);
		tv -= v;
		return tv;
	}

	friend Vector2 operator - (const Vector2& v1, const Vector2& v2)
	{
		Vector2 tv(v1);
		tv -= v2;
		return tv;
	}

	friend Vector2 operator * (const Vector2& v, float f)
	{
		Vector2 tv(v);
		tv *= f;
		return tv;
	}

	friend Vector2 operator * (float f, const Vector2& v)
	{
		Vector2 tv(v);
		tv *= f;
		return tv;
	}

	friend Vector2 operator * (const Vector2& v1, const Vector2& v2)
	{
		Vector2 tv(v1);
		tv *= v2;
		return tv;
	}

	friend Vector2 operator / (const Vector2& v, float f)
	{
		Vector2 tv(v);
		tv /= f;
		return tv;
	}

	friend Vector2 operator / (float f, const Vector2& v)
	{
		Vector2 tv;
		tv.Set(f);
		tv /= v;
		return tv;
	}

	friend Vector2 operator / (const Vector2& v1, const Vector2& v2)
	{
		Vector2 tv;
		tv.Set(v1);
		tv /= v2;
		return tv;
	}

	float Normalize()
	{
		float len = sqrtf(x * x + y * y);
		if (len > 0.0001f)
		{
			float k = 1.0f / len;
			x = float(x * k);
			y = float(y * k);
		}

		return float(len);
	}

	void Min(Vector2 v)
	{
		if (x > v.x) x = v.x;
		if (y > v.y) y = v.y;
	}

	void Max(Vector2 v)
	{
		if (x < v.x) x = v.x;
		if (y < v.y) y = v.y;
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	float Length(Vector2 v)
	{
		float dx = x - v.x;
		float dy = y - v.y;
		return sqrtf(dx * dx + dy * dy);
	}

	float Length2()
	{
		return x * x + y * y;
	}

	float Length2(Vector2 v)
	{
		float dx = x - v.x;
		float dy = y - v.y;
		return dx * dx + dy * dy;
	}

	void Lerp(Vector2 from, Vector2 to, float k)
	{
		x = from.x + (to.x - from.x) * k;
		y = from.y + (to.y - from.y) * k;
	}

	float Dot(Vector2 v)
	{
		return x * v.x + y * v.y;
	}

	float Cross(Vector2 v)
	{
		return x * v.y - y * v.x;
	}
};