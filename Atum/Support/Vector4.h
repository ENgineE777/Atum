
#pragma once

#include "Vector.h"

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
		struct
		{			
			float v4[4];
		};
	};

public:
	
	Vector4();
	
	Vector4(float f);
	
	Vector4(double d);
	
	Vector4(float x, float y, float z);
	
	Vector4(float x, float y, float z, float w);
	
	Vector4(const float f[3]);
	
	Vector4(const double d[3]);
	
	Vector4(const Vector & v);
	
	Vector4(const Vector4 & v);

public:
	
	float operator ~ () const;

	Vector4 operator - () const;
	
	Vector4& operator = (float f);
	
	Vector4& operator = (double d);
	
	Vector4& operator = (const Vector & v);
	
	Vector4& operator = (const Vector4 & v);

public:
	
	void Normalize();
};

inline Vector4::Vector4(){}

inline Vector4::Vector4(float f)
{
	x = y = z = w = f;
}

inline Vector4::Vector4(double d)
{
	w = float(d);
}

inline Vector4::Vector4(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 1.0f;	
}

inline Vector4::Vector4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline Vector4::Vector4(const float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
	w = 1.0f;
}

inline Vector4::Vector4(const double d[3])
{
	x = float(d[0]);
	y = float(d[1]);
	z = float(d[2]);
	w = 1.0f;
}

inline Vector4::Vector4(const Vector & vc)
{
	x = vc.x;
	y = vc.y;
	z = vc.z;
	w = 1.0f;	
}

inline Vector4::Vector4(const Vector4 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

inline float Vector4::operator ~ () const
{
	return x*x + y*y + z*z + w*w;
}

inline Vector4 Vector4::operator - () const
{
	Vector4 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	v.w = -v.w;
	return v;
}

inline Vector4 & Vector4::operator = (float f)
{
	x = y = z = f;
	w = 1.0f;
	return *this;
}

inline Vector4 & Vector4::operator = (double d)
{
	x = y = z = float(d);
	w = 1.0f;
	return *this;
}

inline Vector4 & Vector4::operator = (const Vector & v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;

	w = 1.0f;
	return *this;
}

inline Vector4 & Vector4::operator = (const Vector4 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

inline void Vector4::Normalize()
{
	double k = 1.0/w;
	x = float(k*x);
	y = float(k*y);
	z = float(k*z);
	w = 1.0f;
}
