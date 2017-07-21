
#pragma once

#include "Vector.h"
#include "Vector4.h"

class Matrix
{
public:

	union
	{
		float matrix[16];
		float m[4][4];
		struct
		{			
			float _11,_12,_13,_14;
			float _21,_22,_23,_24;
			float _31,_32,_33,_34;
			float _41,_42,_43,_44;
		};
	};

	Matrix(bool empty = false);

	Vector& Vx() { return (Vector&)matrix[0]; };
	Vector& Vy() { return (Vector&)matrix[4]; };
	Vector& Vz() { return (Vector&)matrix[8]; };
	Vector& Pos() { return (Vector&)matrix[12]; };

	Matrix& operator *= (const Matrix& mtx);

	Matrix& Identity();
	Matrix& SetZero();
	Matrix& Set(Matrix & Matrix);

	Matrix& BuildProjection(float viewAngle, float aspect, float zNear, float zFar);
	bool    BuildView(Vector lookFrom, Vector lookTo, Vector upVector);

	Matrix& RotateX(float ang);
	Matrix& RotateY(float ang);
	Matrix& RotateZ(float ang);

	Matrix& Move(Vector & pos);
	Matrix& Scale(Vector & scale);
	Matrix& Inverse();
	bool InverseComplette();
	Matrix& Transposition();

	Vector  MulVertex(const Vector & v) const;
	Vector4 MulVertex4(const Vector & v) const;
	Vector  MulNormal(Vector & v);
	Vector  MulVertexByInverse(Vector & v);
	Vector  MulNormalByInverse(Vector & v);

	Matrix& Multiply(const Matrix & m1, const Matrix & m2);
};

inline Matrix::Matrix(bool empty)
{
	if (!empty) Identity();
}

inline Matrix& Matrix::operator *= (const Matrix & mtx)
{
	Multiply(*this, mtx);
	return *this;
}

inline Matrix operator * (const Matrix & m1, const Matrix & m2)
{
	Matrix m;
	m.Multiply(m1, m2);
	return m;
}

inline Vector operator * (const Matrix & mtx, const Vector & v)
{
	return mtx.MulVertex(v);
}

inline Vector operator * (const Vector & v, const Matrix & mtx)
{
	return mtx.MulVertex(v);
}

inline Matrix & Matrix::Identity()
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return *this;
}

inline Matrix & Matrix::SetZero()
{
	matrix[0] = 0.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 0.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 0.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 0.0f;

	return *this;
}

inline Matrix & Matrix::Set(Matrix & matrix)
{
	this->matrix[0] = matrix.matrix[0]; 
	this->matrix[1] = matrix.matrix[1];
	this->matrix[2] = matrix.matrix[2];
	this->matrix[3] = matrix.matrix[3];

	this->matrix[4] = matrix.matrix[4]; 
	this->matrix[5] = matrix.matrix[5];
	this->matrix[6] = matrix.matrix[6];
	this->matrix[7] = matrix.matrix[7];

	this->matrix[8] = matrix.matrix[8];  
	this->matrix[9] = matrix.matrix[9]; 
	this->matrix[10] = matrix.matrix[10];
	this->matrix[11] = matrix.matrix[11];

	this->matrix[12] = matrix.matrix[12]; 
	this->matrix[13] = matrix.matrix[13];
	this->matrix[14] = matrix.matrix[14];
	this->matrix[15] = matrix.matrix[15];

	return *this;
}

inline Matrix & Matrix::BuildProjection(float viewAngle, float aspect, float zNear, float zFar)
{
	SetZero();

	double Q = double(zFar)/double(zFar - zNear);
	m[1][1] = float(1.0/tan(viewAngle*0.5));
	m[0][0] = m[1][1] * aspect;
	m[2][2] = float(Q);
	m[2][3] = 1.0f;
	m[3][2] = float(-Q*zNear);

	return *this;
}

inline bool Matrix::BuildView(Vector lookFrom, Vector lookTo, Vector upVector)
{
	Identity();
	
	lookTo -= lookFrom;
	if(lookTo.Normalize() == 0.0f)
	{
		
		Pos() = -lookFrom;
		return false;
	}
	
	upVector -= lookTo * lookTo.Dot(upVector);
	
	if (upVector.Normalize() == 0.0f) upVector.y = 1.0f;
	
	Vector v = upVector.Cross(lookTo);
	if (v.Normalize() != 0.0f)
	{
		m[0][0] = v.x;
		m[1][0] = v.y;
		m[2][0] = v.z;
		m[0][1] = upVector.x;
		m[1][1] = upVector.y;
		m[2][1] = upVector.z;
		m[0][2] = lookTo.x;
		m[1][2] = lookTo.y;
		m[2][2] = lookTo.z;
	}
	else
	{
		Pos() = -lookFrom;
		return false;
	}
	
	Pos() = -MulNormal(lookFrom);
	return true;
}

inline Matrix & Matrix::RotateX(float ang)
{
	Matrix m;
	m.m[1][1] = cosf(ang);
	m.m[1][2] = sinf(ang);
	m.m[2][1] = -sinf(ang);
	m.m[2][2] = cosf(ang);

	Multiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::RotateY(float ang)
{
	Matrix m;
	m.m[0][0] = cosf(ang);
	m.m[0][2] = -sinf(ang);
	m.m[2][0] = sinf(ang);
	m.m[2][2] = cosf(ang);

	Multiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::RotateZ(float ang)
{
	Matrix m;
	m.m[0][0] = cosf(ang);
	m.m[0][1] = sinf(ang);
	m.m[1][0] = -sinf(ang);
	m.m[1][1] = cosf(ang);

	Multiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::Move(Vector & pos)
{
	this->Pos().x += pos.x;
	this->Pos().y += pos.y;
	this->Pos().z += pos.z;
	return *this;
}

inline Matrix & Matrix::Scale(Vector & scale)
{
	m[0][0] *= scale.x;
	m[1][0] *= scale.x;
	m[2][0] *= scale.x;
	m[3][0] *= scale.x;
	m[0][1] *= scale.y;
	m[1][1] *= scale.y;
	m[2][1] *= scale.y;
	m[3][1] *= scale.y;
	m[0][2] *= scale.z;
	m[1][2] *= scale.z;
	m[2][2] *= scale.z;
	m[3][2] *= scale.z;

	return *this;
}

inline Matrix & Matrix::Inverse()
{
	matrix[12] = -(Pos().Dot(Vx()));
	matrix[13] = -(Pos().Dot(Vy()));
	matrix[14] = -(Pos().Dot(Vz()));
	
	float tmp;
	tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;
	
	return *this;
}

inline bool Matrix::InverseComplette()
{
	double inv[16], det;
	int i;

	inv[0] = matrix[5] * matrix[10] * matrix[15] -
				matrix[5] * matrix[11] * matrix[14] -
				matrix[9] * matrix[6] * matrix[15] +
				matrix[9] * matrix[7] * matrix[14] +
				matrix[13] * matrix[6] * matrix[11] -
				matrix[13] * matrix[7] * matrix[10];

	inv[4] = -matrix[4] * matrix[10] * matrix[15] +
				matrix[4] * matrix[11] * matrix[14] +
				matrix[8] * matrix[6] * matrix[15] -
				matrix[8] * matrix[7] * matrix[14] -
				matrix[12] * matrix[6] * matrix[11] +
				matrix[12] * matrix[7] * matrix[10];

	inv[8] = matrix[4] * matrix[9] * matrix[15] -
				matrix[4] * matrix[11] * matrix[13] -
				matrix[8] * matrix[5] * matrix[15] +
				matrix[8] * matrix[7] * matrix[13] +
				matrix[12] * matrix[5] * matrix[11] -
				matrix[12] * matrix[7] * matrix[9];

	inv[12] = -matrix[4] * matrix[9] * matrix[14] +
				matrix[4] * matrix[10] * matrix[13] +
				matrix[8] * matrix[5] * matrix[14] -
				matrix[8] * matrix[6] * matrix[13] -
				matrix[12] * matrix[5] * matrix[10] +
				matrix[12] * matrix[6] * matrix[9];

	inv[1] = -matrix[1] * matrix[10] * matrix[15] +
				matrix[1] * matrix[11] * matrix[14] +
				matrix[9] * matrix[2] * matrix[15] -
				matrix[9] * matrix[3] * matrix[14] -
				matrix[13] * matrix[2] * matrix[11] +
				matrix[13] * matrix[3] * matrix[10];

	inv[5] = matrix[0] * matrix[10] * matrix[15] -
				matrix[0] * matrix[11] * matrix[14] -
				matrix[8] * matrix[2] * matrix[15] +
				matrix[8] * matrix[3] * matrix[14] +
				matrix[12] * matrix[2] * matrix[11] -
				matrix[12] * matrix[3] * matrix[10];

	inv[9] = -matrix[0] * matrix[9] * matrix[15] +
				matrix[0] * matrix[11] * matrix[13] +
				matrix[8] * matrix[1] * matrix[15] -
				matrix[8] * matrix[3] * matrix[13] -
				matrix[12] * matrix[1] * matrix[11] +
				matrix[12] * matrix[3] * matrix[9];

	inv[13] = matrix[0] * matrix[9] * matrix[14] -
				matrix[0] * matrix[10] * matrix[13] -
				matrix[8] * matrix[1] * matrix[14] +
				matrix[8] * matrix[2] * matrix[13] +
				matrix[12] * matrix[1] * matrix[10] -
				matrix[12] * matrix[2] * matrix[9];

	inv[2] = matrix[1] * matrix[6] * matrix[15] -
				matrix[1] * matrix[7] * matrix[14] -
				matrix[5] * matrix[2] * matrix[15] +
				matrix[5] * matrix[3] * matrix[14] +
				matrix[13] * matrix[2] * matrix[7] -
				matrix[13] * matrix[3] * matrix[6];

	inv[6] = -matrix[0] * matrix[6] * matrix[15] +
				matrix[0] * matrix[7] * matrix[14] +
				matrix[4] * matrix[2] * matrix[15] -
				matrix[4] * matrix[3] * matrix[14] -
				matrix[12] * matrix[2] * matrix[7] +
				matrix[12] * matrix[3] * matrix[6];

	inv[10] = matrix[0] * matrix[5] * matrix[15] -
				matrix[0] * matrix[7] * matrix[13] -
				matrix[4] * matrix[1] * matrix[15] +
				matrix[4] * matrix[3] * matrix[13] +
				matrix[12] * matrix[1] * matrix[7] -
				matrix[12] * matrix[3] * matrix[5];

	inv[14] = -matrix[0] * matrix[5] * matrix[14] +
				matrix[0] * matrix[6] * matrix[13] +
				matrix[4] * matrix[1] * matrix[14] -
				matrix[4] * matrix[2] * matrix[13] -
				matrix[12] * matrix[1] * matrix[6] +
				matrix[12] * matrix[2] * matrix[5];

	inv[3] = -matrix[1] * matrix[6] * matrix[11] +
				matrix[1] * matrix[7] * matrix[10] +
				matrix[5] * matrix[2] * matrix[11] -
				matrix[5] * matrix[3] * matrix[10] -
				matrix[9] * matrix[2] * matrix[7] +
				matrix[9] * matrix[3] * matrix[6];

	inv[7] = matrix[0] * matrix[6] * matrix[11] -
				matrix[0] * matrix[7] * matrix[10] -
				matrix[4] * matrix[2] * matrix[11] +
				matrix[4] * matrix[3] * matrix[10] +
				matrix[8] * matrix[2] * matrix[7] -
				matrix[8] * matrix[3] * matrix[6];

	inv[11] = -matrix[0] * matrix[5] * matrix[11] +
				matrix[0] * matrix[7] * matrix[9] +
				matrix[4] * matrix[1] * matrix[11] -
				matrix[4] * matrix[3] * matrix[9] -
				matrix[8] * matrix[1] * matrix[7] +
				matrix[8] * matrix[3] * matrix[5];

	inv[15] = matrix[0] * matrix[5] * matrix[10] -
				matrix[0] * matrix[6] * matrix[9] -
				matrix[4] * matrix[1] * matrix[10] +
				matrix[4] * matrix[2] * matrix[9] +
				matrix[8] * matrix[1] * matrix[6] -
				matrix[8] * matrix[2] * matrix[5];

	det = matrix[0] * inv[0] + matrix[1] * inv[4] + matrix[2] * inv[8] + matrix[3] * inv[12];

	if (fabs(det) < 0.001f)
	{
		return false;
	}

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
	{
		matrix[i] = inv[i] * det;
	}

	return true;
}

inline Matrix & Matrix::Transposition()
{
	float tmp;
	tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[0][3]; m[0][3] = m[3][0]; m[3][0] = tmp;
	tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;
	tmp = m[1][3]; m[1][3] = m[3][1]; m[3][1] = tmp;
	tmp = m[2][3]; m[2][3] = m[3][2]; m[3][2] = tmp;

	return *this;
}

inline Matrix & Matrix::Multiply(const Matrix & m1, const Matrix & m2)
{
	m[0][0] = m2.m[0][0]*m1.m[0][0] + m2.m[1][0]*m1.m[0][1] + m2.m[2][0]*m1.m[0][2] + m2.m[3][0]*m1.m[0][3];
	m[0][1] = m2.m[0][1]*m1.m[0][0] + m2.m[1][1]*m1.m[0][1] + m2.m[2][1]*m1.m[0][2] + m2.m[3][1]*m1.m[0][3];
	m[0][2] = m2.m[0][2]*m1.m[0][0] + m2.m[1][2]*m1.m[0][1] + m2.m[2][2]*m1.m[0][2] + m2.m[3][2]*m1.m[0][3];
	m[0][3] = m2.m[0][3]*m1.m[0][0] + m2.m[1][3]*m1.m[0][1] + m2.m[2][3]*m1.m[0][2] + m2.m[3][3]*m1.m[0][3];
	m[1][0] = m2.m[0][0]*m1.m[1][0] + m2.m[1][0]*m1.m[1][1] + m2.m[2][0]*m1.m[1][2] + m2.m[3][0]*m1.m[1][3];
	m[1][1] = m2.m[0][1]*m1.m[1][0] + m2.m[1][1]*m1.m[1][1] + m2.m[2][1]*m1.m[1][2] + m2.m[3][1]*m1.m[1][3];
	m[1][2] = m2.m[0][2]*m1.m[1][0] + m2.m[1][2]*m1.m[1][1] + m2.m[2][2]*m1.m[1][2] + m2.m[3][2]*m1.m[1][3];
	m[1][3] = m2.m[0][3]*m1.m[1][0] + m2.m[1][3]*m1.m[1][1] + m2.m[2][3]*m1.m[1][2] + m2.m[3][3]*m1.m[1][3];
	m[2][0] = m2.m[0][0]*m1.m[2][0] + m2.m[1][0]*m1.m[2][1] + m2.m[2][0]*m1.m[2][2] + m2.m[3][0]*m1.m[2][3];
	m[2][1] = m2.m[0][1]*m1.m[2][0] + m2.m[1][1]*m1.m[2][1] + m2.m[2][1]*m1.m[2][2] + m2.m[3][1]*m1.m[2][3];
	m[2][2] = m2.m[0][2]*m1.m[2][0] + m2.m[1][2]*m1.m[2][1] + m2.m[2][2]*m1.m[2][2] + m2.m[3][2]*m1.m[2][3];
	m[2][3] = m2.m[0][3]*m1.m[2][0] + m2.m[1][3]*m1.m[2][1] + m2.m[2][3]*m1.m[2][2] + m2.m[3][3]*m1.m[2][3];
	m[3][0] = m2.m[0][0]*m1.m[3][0] + m2.m[1][0]*m1.m[3][1] + m2.m[2][0]*m1.m[3][2] + m2.m[3][0]*m1.m[3][3];
	m[3][1] = m2.m[0][1]*m1.m[3][0] + m2.m[1][1]*m1.m[3][1] + m2.m[2][1]*m1.m[3][2] + m2.m[3][1]*m1.m[3][3];
	m[3][2] = m2.m[0][2]*m1.m[3][0] + m2.m[1][2]*m1.m[3][1] + m2.m[2][2]*m1.m[3][2] + m2.m[3][2]*m1.m[3][3];
	m[3][3] = m2.m[0][3]*m1.m[3][0] + m2.m[1][3]*m1.m[3][1] + m2.m[2][3]*m1.m[3][2] + m2.m[3][3]*m1.m[3][3];

	return *this;
}

inline Vector Matrix::MulVertex(const Vector & v) const
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0];
	tv.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1];
	tv.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2];

	return tv;
}

inline Vector4 Matrix::MulVertex4(const Vector & v) const
{
	Vector4 tv;
	tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
	tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
	tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
	tv.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];

	return tv;
}

inline Vector Matrix::MulNormal(Vector & v)
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z;
	tv.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z;
	tv.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z;

	return tv;
}

inline Vector Matrix::MulVertexByInverse(Vector & v)
{
	Vector tv;
	tv.x = m[0][0]*(v.x - m[3][0]) + m[0][1]*(v.y - m[3][1]) + m[0][2]*(v.z - m[3][2]);
	tv.y = m[1][0]*(v.x - m[3][0]) + m[1][1]*(v.y - m[3][1]) + m[1][2]*(v.z - m[3][2]);
	tv.z = m[2][0]*(v.x - m[3][0]) + m[2][1]*(v.y - m[3][1]) + m[2][2]*(v.z - m[3][2]);

	return tv;
}

inline Vector Matrix::MulNormalByInverse(Vector & v)
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z;
	tv.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z;
	tv.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z;

	return tv;
}
