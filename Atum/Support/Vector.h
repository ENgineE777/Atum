
#pragma once

#include <stdlib.h>
#include <math.h>

struct Vector
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

	Vector();
	Vector(float f);
	Vector(float x, float y, float z);
	Vector(const Vector& v);

	void Set(float f);
	void Set(float x, float y, float z);
	void Set(const Vector&v);

	Vector operator - () const;
	Vector& operator = (float f);
	Vector& operator = (const Vector& v);
	Vector& operator += (float f);
	Vector& operator += (const Vector& v);
	Vector& operator -= (float f);
	Vector& operator -= (const Vector& v);
	Vector& operator *= (float f);
	Vector& operator *= (const Vector& v);
	Vector& operator /= (float f);
	Vector& operator /= (const Vector& v);

	float Normalize();
	Vector& Min(Vector& v);
	Vector& Max(Vector& v);

	float Length();
	float Length2();

	Vector& Lerp(Vector& from, Vector& to, float kBlend);

	bool Equals(Vector& pos, float delta);

	float Dot(Vector& v);
	Vector Cross(Vector& v);
};


inline Vector::Vector(){}
inline Vector::Vector(float f)
{
	x = f;
	y = f;
	z = f;
}

inline Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline Vector::Vector(const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline void Vector::Set(float f)
{
	x = f;
	y = f;
	z = f;
}

inline void Vector::Set(float set_x, float set_y, float set_z)
{
	x = set_x;
	y = set_y;
	z = set_z;
}

inline void Vector::Set(const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline Vector Vector::operator - () const
{
	Vector v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

inline Vector& Vector::operator = (float f)
{
	x = f;
	y = f;
	z = f;
	return *this;
}

inline Vector& Vector::operator = (const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

inline Vector& Vector::operator += (float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

inline Vector& Vector::operator += (const Vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline Vector& Vector::operator -= (float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

inline Vector& Vector::operator -= (const Vector& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline Vector& Vector::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

inline Vector& Vector::operator *= (const Vector& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

inline Vector& Vector::operator /= (float f)
{
	float d = 1.0f / f;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

inline Vector& Vector::operator /= (const Vector& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

inline Vector operator + (const Vector& v, float f)
{
	Vector tv(v);
	tv += f;
	return tv;
}

inline Vector operator + (float f, const Vector& v)
{
	Vector tv(v);
	tv += f;
	return tv;
}

inline Vector operator + (const Vector& v1, const Vector& v2)
{
	Vector tv(v1);
	tv += v2;
	return tv;
}

inline Vector operator - (const Vector& v, float f)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

inline Vector operator - (float f, const Vector& v)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

inline Vector operator - (const Vector& v1, const Vector& v2)
{
	Vector tv(v1);
	tv -= v2;
	return tv;
}

inline Vector operator * (const Vector& v, float f)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

inline Vector operator * (float f, const Vector& v)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

inline Vector operator * (const Vector& v1, const Vector& v2)
{
	Vector tv(v1);
	tv *= v2;
	return tv;
}

inline Vector operator / (const Vector& v, float f)
{
	Vector tv(v);
	tv /= f;
	return tv;
}

inline Vector operator / (float f, const Vector& v)
{
	Vector tv;
	tv.Set(f);
	tv /= v;
	return tv;
}

inline Vector operator / (const Vector& v1, const Vector& v2)
{
	Vector tv;
	tv.Set(v1);
	tv /= v2;
	return tv;
}

inline float Vector::Normalize()
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

inline Vector& Vector::Min(Vector& v)
{
	if(x > v.x) x = v.x;
	if(y > v.y) y = v.y;
	if(z > v.z) z = v.z;
	return *this;
}

inline Vector& Vector::Max(Vector& v)
{
	if(x < v.x) x = v.x;
	if(y < v.y) y = v.y;
	if(z < v.z) z = v.z;
	return *this;
}

inline float Vector::Length()
{
	return float(sqrtf(x*x + y*y + z*z));
}

inline float Vector::Length2()
{
	return x*x + y*y + z*z;
}

inline Vector& Vector::Lerp(Vector& from, Vector& to, float kBlend)
{
	x = from.x + (to.x - from.x)*kBlend;
	y = from.y + (to.y - from.y)*kBlend;
	z = from.z + (to.z - from.z)*kBlend;
	return *this;
}

inline bool Vector::Equals(Vector& pos, float delta)
{
	return ((fabs(x - pos.x)<delta) && (fabs(y - pos.y)<delta) && (fabs(y - pos.y)<delta));
}

inline float Vector::Dot(Vector& v)
{
	return x * v.x + y * v.y + z * v.z;
}

inline Vector Vector::Cross(Vector& v)
{
	Vector v_res;
	v_res.x = y * v.z - z * v.y;
	v_res.y = z * v.x - x * v.z;
	v_res.z = x * v.y - y * v.x;
	return v_res;
}
