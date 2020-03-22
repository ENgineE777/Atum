
#pragma once

#include "Vector3.h"

/**
\ingroup gr_code_common
*/

/**
\brief 4 component vector

Implementation of a vector with 4 components.

*/

struct Vector4
{
public:

	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		float v[4];
	};

	Vector4()
	{
		Set(0.0f);
	}

	Vector4(float f)
	{
		Set(f);
	}

	Vector4(float x, float y, float z, float w)
	{
		Set(x, y, z, w);
	}

	Vector4(Vector3 v)
	{
		Set(v.x, v.y, v.z, 1.0f);
	}

	void Set(float f)
	{
		x = y = z = w = f;
	}

	void Set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Set(Vector4 v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	Vector4 operator - () const
	{
		Vector4 v(*this);
		v.x = -v.x;
		v.y = -v.y;
		v.z = -v.z;
		v.w = -v.w;
		return v;
	}

	bool operator == (const Vector4& v)
	{
		return (fabs(x - v.x) < 0.001f) && (fabs(y - v.y) < 0.001f) && (fabs(z - v.z) < 0.001f) && (fabs(w - v.w) < 0.001f);
	}

	Vector4& operator = (float f)
	{
		x = f;
		y = f;
		z = f;
		w = f;
		return *this;
	}

	Vector4& operator = (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	Vector4& operator += (float f)
	{
		x += f;
		y += f;
		z += f;
		w += f;
		return *this;
	}

	Vector4& operator += (const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	Vector4& operator -= (float f)
	{
		x -= f;
		y -= f;
		z -= f;
		w -= f;
		return *this;
	}

	Vector4& operator -= (const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	Vector4& operator *= (float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	Vector4& operator *= (const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	Vector4& operator /= (float f)
	{
		float d = 1.0f / f;
		x = float(x * d);
		y = float(y * d);
		z = float(z * d);
		w = float(w * d);
		return *this;
	}

	Vector4& operator /= (const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	friend Vector4 operator + (const Vector4& v, float f)
	{
		Vector4 tv(v);
		tv += f;
		return tv;
	}

	friend Vector4 operator + (float f, const Vector4& v)
	{
		Vector4 tv(f);
		tv += v;
		return tv;
	}

	friend Vector4 operator + (const Vector4& v1, const Vector4& v2)
	{
		Vector4 tv(v1);
		tv += v2;
		return tv;
	}

	friend Vector4 operator - (const Vector4& v, float f)
	{
		Vector4 tv(v);
		tv -= f;
		return tv;
	}

	friend Vector4 operator - (float f, const Vector4& v)
	{
		Vector4 tv(f);
		tv -= v;
		return tv;
	}

	friend Vector4 operator - (const Vector4& v1, const Vector4& v2)
	{
		Vector4 tv(v1);
		tv -= v2;
		return tv;
	}

	friend Vector4 operator * (const Vector4& v, float f)
	{
		Vector4 tv(v);
		tv *= f;
		return tv;
	}

	friend Vector4 operator * (float f, const Vector4& v)
	{
		Vector4 tv(v);
		tv *= f;
		return tv;
	}

	friend Vector4 operator * (const Vector4& v1, const Vector4& v2)
	{
		Vector4 tv(v1);
		tv *= v2;
		return tv;
	}

	friend Vector4 operator / (const Vector4& v, float f)
	{
		Vector4 tv(v);
		tv /= f;
		return tv;
	}

	friend Vector4 operator / (float f, const Vector4& v)
	{
		Vector4 tv;
		tv.Set(f);
		tv /= v;
		return tv;
	}

	friend Vector4 operator / (const Vector4& v1, const Vector4& v2)
	{
		Vector4 tv;
		tv.Set(v1);
		tv /= v2;
		return tv;
	}

	float Normalize()
	{
		float len = sqrtf(x * x + y * y + z * z + w * w);
		if (len > 0.0001f)
		{
			float k = 1.0f / len;
			x = float(x * k);
			y = float(y * k);
			z = float(z * k);
			w = float(w * k);
		}

		return float(len);
	}

	void Min(Vector4 v)
	{
		if (x > v.x) x = v.x;
		if (y > v.y) y = v.y;
		if (z > v.z) z = v.z;
		if (w > v.w) w = v.w;
	}

	void Max(Vector4 v)
	{
		if (x < v.x) x = v.x;
		if (y < v.y) y = v.y;
		if (z < v.z) z = v.z;
		if (w < v.w) w = v.w;
	}

	float Length()
	{
		return sqrtf(x*x + y * y + z * z + w * w);
	}

	float Length(Vector4 v)
	{
		float dx = x - v.x;
		float dy = y - v.y;
		float dz = z - v.z;
		float dw = w - v.w;
		return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw);
	}

	float Length2()
	{
		return x * x + y * y + z * z + w * w;
	}

	float Length2(Vector4 v)
	{
		float dx = x - v.x;
		float dy = y - v.y;
		float dz = z - v.z;
		float dw = w - v.w;
		return dx * dx + dy * dy + dz * dz + dw * dw;
	}

	void Lerp(Vector4 from, Vector4 to, float k)
	{
		x = from.x + (to.x - from.x) * k;
		y = from.y + (to.y - from.y) * k;
		z = from.z + (to.z - from.z) * k;
		w = from.w + (to.w - from.w) * k;
	}

	float Dot(Vector4 v)
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	Vector4 Cross(Vector4 v)
	{
		Vector4 v_res;
		v_res.x = y * v.w - w * v.y;
		v_res.y = z * v.x - x * v.z;
		v_res.z = w * v.y - y * v.w;
		v_res.z = x * v.z - z * v.x;
		return v_res;
	}
};