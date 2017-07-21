
#pragma once

#include "Vector4.h"

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
	Vector2(const float f[2]);	
	Vector2(const double d[2]);	
	Vector2(const Vector2& v);	
	//Vector2(const IVector& v);	
	//Vector2(const IVector4& v);

public:
			
	Vector2& operator = (float f);	
	Vector2& operator = (double d);	
	Vector2& operator = (const Vector2& v);	
	//Vector2& operator = (const IVector& v);
	//Vector2& operator = (const IVector4& v);
	
	Vector2& operator *= (float f);
	Vector2& operator *= (double d);
	Vector2& operator *= (const Vector2& v);

	Vector2& operator += (float f);
	Vector2& operator += (double d);
	Vector2& operator += (const Vector2& v);

	Vector2& operator -= (float f);
	Vector2& operator -= (double d);
	Vector2& operator -= (const Vector2& v);	

public:

	void Normalize();
	float Length();

	bool InSquare(const Vector2 & min, const Vector2 & max) const;
	
	float Distance(const ::Vector2 & v) const;

	Vector2 & Lerp(const Vector2 & from, const Vector2 & to, float kBlend);
};

inline Vector2::Vector2(){}

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

inline Vector2::Vector2(const float f[2])
{
	x = f[0];
	y = f[1];	
}

inline Vector2::Vector2(const double d[2])
{
	x = float(d[0]);
	y = float(d[1]);	
}

inline Vector2::Vector2(const Vector2& vc)
{
	x = vc.x;
	y = vc.y;	
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

inline void Vector2::Normalize()
{
	double k = 1.0/sqrt(x*x + y*y);
	x = float(k*x);
	y = float(k*y);	
}

inline float Vector2::Length()
{
	return sqrtf(x*x + y*y);
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator *= (float f)
{
	x *= f;
	y *= f;	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator *= (double d)
{
	x *= float(d);
	y *= float(d);	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator *= (const Vector2 & v)
{
	x *= v.x;
	y *= v.y;	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator += (float f)
{
	x += f;
	y += f;	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator += (double d)
{
	x += float(d);
	y += float(d);	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator += (const Vector2 & v)
{
	x += v.x;
	y += v.y;	
	return *this;
}
//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator -= (float f)
{
	x -= f;
	y -= f;	
	return *this;
}

//Покомпонентное умножение с присваиванием
inline Vector2 & Vector2::operator -= (double d)
{
	x -= float(d);
	y -= float(d);
	return *this;
}

//Покомпонентное умножение с присваиванием
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

/*!\relates IVector
Умножить
*/
inline Vector2 operator * (double d, const Vector2 & v)
{
	Vector2 tv(v);
	tv *= d;
	return tv;
}

/*!\relates IVector
Умножить
*/
inline Vector2 operator * (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv *= v2;
	return tv;
}

inline Vector2 operator + (const Vector2 & v, double d)
{
	Vector2 tv(v);
	tv += d;
	return tv;
}

/*!\relates IVector
Умножить
*/
inline Vector2 operator + (double d, const Vector2 & v)
{
	Vector2 tv(v);
	tv += d;
	return tv;
}

/*!\relates IVector
Умножить
*/
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

/*!\relates IVector
Умножить
*/
inline Vector2 operator - (double d, const Vector2 & v)
{
	Vector2 tv(v);
	tv -= d;
	return tv;
}

/*!\relates IVector
Умножить
*/
inline Vector2 operator - (const Vector2 & v1, const Vector2 & v2)
{
	Vector2 tv(v1);
	tv -= v2;
	return tv;
}

inline float operator | (const Vector2& v1, const Vector2& v2)
{	
	return v1.x*v2.x + v1.y*v2.y;
}

inline bool Vector2::InSquare(const Vector2 & min, const Vector2 & max) const
{
	if(y < min.y || y > max.y) return false;
	if(x < min.x || x > max.x) return false;
	return true;
}

inline float Vector2::Distance(const ::Vector2 & v) const
{
	float dx = x - v.x;
	float dy = y - v.y;
	return ::sqrtf(dx * dx + dy * dy);
}

inline Vector2 & Vector2::Lerp(const Vector2 & from, const Vector2 & to, float kBlend)
{
	x = from.x + (to.x - from.x) * kBlend;
	y = from.y + (to.y - from.y) * kBlend;
	
	return *this;
}