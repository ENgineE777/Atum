
#pragma once


#include "Vector.h"
#include "Matrix.h"

class Quaternion
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
		float q[4];
	};

public:
	
	Quaternion();
	
	Quaternion(float qx, float qy, float qz, float qw);
	
	Quaternion(const Matrix& mtx);	
	
public:
	
	Quaternion & operator - ();
	
	Quaternion & operator = (const Quaternion & q);
	
	Quaternion & operator += (const Quaternion & q);
	
	Quaternion & operator += (float f);
	
	Quaternion & operator -= (const Quaternion & q);
	
	Quaternion & operator -= (float f);
	
	Quaternion & operator *= (float f);
	
	Quaternion & operator *= (const Quaternion & q);
	
	Quaternion & operator /= (float f);

public:
	
	void SetIdentity();
	void Set(float qx, float qy, float qz, float qw);
	void Set(const Matrix & mtx);
	inline Quaternion& Set(float angle, Vector v);
	Quaternion& Normalize();
	Quaternion& Conjugate();
	Quaternion& Inverse();
	Quaternion& SLerp(const Quaternion & from, const Quaternion & to, float kBlend);
	Matrix& GetMatrix(Matrix & mtx);
	float GetLength();
	float GetLengthSqr();
	void Rotate(Vector& v);
};

inline Quaternion::Quaternion()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

inline Quaternion::Quaternion(float qx, float qy, float qz, float qw)
{
	Set(qx, qy, qz, qw);
}

inline Quaternion::Quaternion(const Matrix & mtx)
{
	Set(mtx);
}

inline Quaternion& Quaternion::operator - ()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

inline Quaternion& Quaternion::operator = (const Quaternion & q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
}

inline Quaternion& Quaternion::operator += (const Quaternion & q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;	
	return *this;
}

inline Quaternion& Quaternion::operator += (float f)
{
	x += f;
	y += f;
	z += f;
	w += f;
	return *this;
}

inline Quaternion& Quaternion::operator -= (const Quaternion & q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

inline Quaternion& Quaternion::operator -= (float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;
	return *this;
}

inline Quaternion& Quaternion::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

inline Quaternion & Quaternion::operator *= (const Quaternion & q)
{
	float a = (w + x)*(q.w + q.x);
	float b = (z - y)*(q.y - q.z);
	float c = (x - w)*(q.y - q.z);
	float d = (y + z)*(q.x - q.w);
	float e = (x + z)*(q.x + q.y);
	float f = (x - z)*(q.x - q.y);
	float g = (w + y)*(q.w - q.z);
	float h = (w - y)*(q.w + q.z);
	x = a - ( e + f + g + h)*0.5f; 
	y =-c + ( e - f + g - h)*0.5f;
	z =-d + ( e - f - g + h)*0.5f;
	w = b + (-e - f + g + h)*0.5f;
	return *this;
}

inline Quaternion & Quaternion::operator /= (float f)
{
	double k = 1.0/f;
	x = float(x*k);
	y = float(y*k);
	z = float(z*k);
	w = float(w*k);
	return *this;
}

inline Quaternion operator + (float f, const Quaternion & q)
{
	Quaternion tq(q);
	tq += f;
	return tq;
}

inline Quaternion operator + (const Quaternion & q, float f)
{
	Quaternion tq(q);
	tq += f;
	return tq;
}

inline Quaternion operator + (const Quaternion & q1, const Quaternion & q2)
{
	Quaternion tq(q1);
	tq += q2;
	return tq;
}

inline Quaternion operator - (float f, const Quaternion & q)
{
	Quaternion tq(q);
	tq -= f;
	return tq;
}

inline Quaternion operator - (const Quaternion & q, float f)
{
	Quaternion tq(q);
	tq -= f;
	return tq;
}

inline Quaternion operator - (const Quaternion & q1, const Quaternion & q2)
{
	Quaternion tq(q1);
	tq -= q2;
	return tq;
}

inline Quaternion operator * (float f, const Quaternion & q)
{
	Quaternion tq(q);
	tq *= f;
	return tq;
}

inline Quaternion operator * (const Quaternion & q, float f)
{
	Quaternion tq(q);
	tq *= f;
	return tq;
}

inline Quaternion operator * (const Quaternion & q1, const Quaternion & q2)
{
	Quaternion tq(q1);
	tq *= q2;
	return tq;
}

inline Quaternion operator / (const Quaternion & q, float f)
{
	Quaternion tq(q);
	tq /= f;
	return tq;
}


inline void Quaternion::SetIdentity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

inline void Quaternion::Set(float qx, float qy, float qz, float qw)
{
	x = qx;
	y = qy;
	z = qz;
	w = qw;	
}

Quaternion& Quaternion:: Set(float angle, Vector v)
{

	w=cos(angle/2);
	v=v*sin(angle/2);
	x=v.x;
	y=v.y;
	z=v.z;

	return (*this);
}

inline void Quaternion::Set(const Matrix & mtx)
{
	float tr = mtx.m[0][0] + mtx.m[1][1] + mtx.m[2][2];
	if (tr > 0.0f)
	{
		double s = sqrt(tr + 1.0f);
		w = float(s*0.5);
		s = 0.5/s;
		x = float((mtx.m[1][2] - mtx.m[2][1])*s);
		y = float((mtx.m[2][0] - mtx.m[0][2])*s);
		z = float((mtx.m[0][1] - mtx.m[1][0])*s);
	}
	else
	{
		long i, j, k;
		if (mtx.m[1][1] > mtx.m[0][0])
		{
			if (mtx.m[2][2] > mtx.m[1][1])
			{
				i = 2;
				j = 0;
				k = 1;
			}
			else
			{
				i = 1;
				j = 2;
				k = 0;
			}
		}
		else
		if(mtx.m[2][2] > mtx.m[0][0])
		{
			i = 2;
			j = 0;
			k = 1;
		}
		else
		{
			i = 0;
			j = 1;
			k = 2;
		}

		double s = sqrt((mtx.m[i][i] - (mtx.m[j][j] + mtx.m[k][k])) + 1.0);
		q[i] = float(s*0.5);
		if(s != 0.0) s = 0.5/s;
		q[j] = float((mtx.m[i][j] + mtx.m[j][i])*s);
		q[k] = float((mtx.m[i][k] + mtx.m[k][i])*s);
		q[3] = float((mtx.m[j][k] - mtx.m[k][j])*s);
	}
}

inline Quaternion & Quaternion::Normalize()
{
	double len = double(x)*double(x) + double(y)*double(y) + double(z)*double(z) + double(w)*double(w);
	if (len <= 0.0)
	{
		return *this;
	}

	len = 1.0/sqrt(len);
	x = float(x*len);
	y = float(y*len);
	z = float(z*len);
	w = float(w*len);
	return *this;
}

inline Quaternion & Quaternion::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

inline Quaternion & Quaternion::Inverse()
{
	Conjugate();
	Normalize();
	return *this;
}

inline Quaternion & Quaternion::SLerp(const Quaternion & from, const Quaternion & to, float kBlend)
{
	double cosomega = from.x*to.x + from.y*to.y + from.z*to.z + from.w*to.w;
	double k = 1.0f;
	
	if (cosomega < 0.0f)
	{
		cosomega = -cosomega;
		k = -1.0f;
	}
	
	if (cosomega > 1.0)
	{
		cosomega = 1.0;
	}
	
	double k0, k1;
	
	if (1.0 - cosomega > 0.005)
	{
		
		double omega = acos(cosomega);
		double sinomega = 1.0/sin(omega);
		k0 = sin(omega*(1.0 - kBlend))*sinomega;
		k1 = sin(omega*kBlend)*sinomega;
	}
	else
	{
		k0 = 1.0 - kBlend;
		k1 = kBlend;
	}

	k0 *= k;
	
	x = float(from.x*k0 + to.x*k1);
	y = float(from.y*k0 + to.y*k1);
	z = float(from.z*k0 + to.z*k1);
	w = float(from.w*k0 + to.w*k1);
	
	return *this;
}

inline Matrix& Quaternion::GetMatrix(Matrix& mtx)
{
	float kLen = x*x + y*y + z*z + w*w;

	kLen = 2.0f/kLen;
	float xx = x*x*kLen, xy = x*y*kLen, xz = x*z*kLen;
	float yy = y*y*kLen, yz = y*z*kLen;
	float zz = z*z*kLen;
	float wx = w*x*kLen, wy = w*y*kLen, wz = w*z*kLen;
	mtx.m[0][0] = 1.0f - (yy + zz);
	mtx.m[0][1] = xy + wz;
	mtx.m[0][2] = xz - wy;
	mtx.m[0][3] = 0.0f;
	mtx.m[1][0] = xy - wz;
	mtx.m[1][1] = 1.0f - (xx + zz);
	mtx.m[1][2] = yz + wx;
	mtx.m[1][3] = 0.0f;
	mtx.m[2][0] = xz + wy;
	mtx.m[2][1] = yz - wx;
	mtx.m[2][2] = 1.0f - (xx + yy);
	mtx.m[2][3] = 0.0f;
	mtx.m[3][0] = 0.0f;
	mtx.m[3][1] = 0.0f;
	mtx.m[3][2] = 0.0f;
	mtx.m[3][3] = 1.0f;
	return mtx;
}

inline float Quaternion::GetLength()
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

inline float Quaternion::GetLengthSqr()
{
	return x*x + y*y + z*z + w*w;
}

inline void Quaternion::Rotate(Vector& v)
{
	Quaternion r ( v.x * w + v.z * y - v.y * z,
					v.y * w + v.x * z - v.z * x,
					v.z * w + v.y * x - v.x * y,
					v.x * x + v.y * y + v.z * z );

	v.x = w * r.x + x * r.w + y * r.z - z * r.y;
	v.y = w * r.y + y * r.w + z * r.x - x * r.z;
	v.z = w * r.z + z * r.w + x * r.y - y * r.x;
}
