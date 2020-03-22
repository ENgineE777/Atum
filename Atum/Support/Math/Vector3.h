
#pragma once

#include <math.h>

/**
\ingroup gr_code_common
*/

/**
\brief 3 component Vector3

Implementation of a Vector3 with 3 components.

*/

struct Vector3
{
public:

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		
		float v[3];
	};

	Vector3();
	Vector3(float f);
	Vector3(float x, float y, float z);

	void Set(float f);
	void Set(float x, float y, float z);
	void Set(Vector3 v);

	Vector3 operator - () const;
	Vector3& operator = (float f);
	Vector3& operator = (const Vector3& v);
	Vector3& operator += (float f);
	Vector3& operator += (const Vector3& v);
	Vector3& operator -= (float f);
	Vector3& operator -= (const Vector3& v);
	Vector3& operator *= (float f);
	Vector3& operator *= (const Vector3& v);
	Vector3& operator /= (float f);
	Vector3& operator /= (const Vector3& v);

	float Normalize();
	Vector3& Min(Vector3 v);
	Vector3& Max(Vector3 v);

	float Length();
	float Length2();

	Vector3& Lerp(Vector3 from, Vector3 to, float kBlend);

	bool Equals(Vector3 pos, float delta);

	float Dot(Vector3 v);
	Vector3 Cross(Vector3 v);
};


inline Vector3::Vector3(){}
inline Vector3::Vector3(float f)
{
	x = f;
	y = f;
	z = f;
}

inline Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline void Vector3::Set(float f)
{
	x = f;
	y = f;
	z = f;
}

inline void Vector3::Set(float set_x, float set_y, float set_z)
{
	x = set_x;
	y = set_y;
	z = set_z;
}

inline void Vector3::Set(Vector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline Vector3 Vector3::operator - () const
{
	Vector3 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

inline Vector3& Vector3::operator = (float f)
{
	x = f;
	y = f;
	z = f;
	return *this;
}

inline Vector3& Vector3::operator = (const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

inline Vector3& Vector3::operator += (float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

inline Vector3& Vector3::operator += (const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline Vector3& Vector3::operator -= (float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

inline Vector3& Vector3::operator -= (const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline Vector3& Vector3::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

inline Vector3& Vector3::operator *= (const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

inline Vector3& Vector3::operator /= (float f)
{
	float d = 1.0f / f;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

inline Vector3& Vector3::operator /= (const Vector3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

inline Vector3 operator + (const Vector3& v, float f)
{
	Vector3 tv(v);
	tv += f;
	return tv;
}

inline Vector3 operator + (float f, const Vector3& v)
{
	Vector3 tv(f);
	tv += v;
	return tv;
}

inline Vector3 operator + (const Vector3& v1, const Vector3& v2)
{
	Vector3 tv(v1);
	tv += v2;
	return tv;
}

inline Vector3 operator - (const Vector3& v, float f)
{
	Vector3 tv(v);
	tv -= f;
	return tv;
}

inline Vector3 operator - (float f, const Vector3& v)
{
	Vector3 tv(f);
	tv -= v;
	return tv;
}

inline Vector3 operator - (const Vector3& v1, const Vector3& v2)
{
	Vector3 tv(v1);
	tv -= v2;
	return tv;
}

inline Vector3 operator * (const Vector3& v, float f)
{
	Vector3 tv(v);
	tv *= f;
	return tv;
}

inline Vector3 operator * (float f, const Vector3& v)
{
	Vector3 tv(v);
	tv *= f;
	return tv;
}

inline Vector3 operator * (const Vector3& v1, const Vector3& v2)
{
	Vector3 tv(v1);
	tv *= v2;
	return tv;
}

inline Vector3 operator / (const Vector3& v, float f)
{
	Vector3 tv(v);
	tv /= f;
	return tv;
}

inline Vector3 operator / (float f, const Vector3& v)
{
	Vector3 tv;
	tv.Set(f);
	tv /= v;
	return tv;
}

inline Vector3 operator / (const Vector3& v1, const Vector3& v2)
{
	Vector3 tv;
	tv.Set(v1);
	tv /= v2;
	return tv;
}

inline float Vector3::Normalize()
{
	float len = sqrtf(x*x + y*y + z*z);
	if(len > 0.0)
	{
		float k = 1.0f/len;
		x = float(x*k);
		y = float(y*k);
		z = float(z*k);
	}
	return float(len);
}

inline Vector3& Vector3::Min(Vector3 v)
{
	if(x > v.x) x = v.x;
	if(y > v.y) y = v.y;
	if(z > v.z) z = v.z;
	return *this;
}

inline Vector3& Vector3::Max(Vector3 v)
{
	if(x < v.x) x = v.x;
	if(y < v.y) y = v.y;
	if(z < v.z) z = v.z;
	return *this;
}

inline float Vector3::Length()
{
	return float(sqrtf(x*x + y*y + z*z));
}

inline float Vector3::Length2()
{
	return x*x + y*y + z*z;
}

inline Vector3& Vector3::Lerp(Vector3 from, Vector3 to, float kBlend)
{
	x = from.x + (to.x - from.x)*kBlend;
	y = from.y + (to.y - from.y)*kBlend;
	z = from.z + (to.z - from.z)*kBlend;
	return *this;
}

inline bool Vector3::Equals(Vector3 pos, float delta)
{
	return ((fabs(x - pos.x)<delta) && (fabs(y - pos.y)<delta) && (fabs(y - pos.y)<delta));
}

inline float Vector3::Dot(Vector3 v)
{
	return x * v.x + y * v.y + z * v.z;
}

inline Vector3 Vector3::Cross(Vector3 v)
{
	Vector3 v_res;
	v_res.x = y * v.z - z * v.y;
	v_res.y = z * v.x - x * v.z;
	v_res.z = x * v.y - y * v.x;
	return v_res;
}
