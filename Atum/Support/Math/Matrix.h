
#pragma once

#include "Vector3.h"
#include "Vector4.h"

/**
\ingroup gr_code_common
*/

/**
\brief Matrix

Implementation of a matrix.

*/

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

	Matrix(bool empty = false)
	{
		if (!empty) Identity();
	}

	Vector3& Vx()
	{
		return (Vector3&)matrix[0];
	}

	Vector3& Vy()
	{
		return (Vector3&)matrix[4];
	}

	Vector3& Vz()
	{
		return (Vector3&)matrix[8];
	}

	Vector3& Pos()
	{
		return (Vector3&)matrix[12];
	}

	bool operator == (const Matrix& mat)
	{
		for (int i = 0; i < 16; i++)
		{
			if (fabs(matrix[i] - mat.matrix[i]) > 0.001f)
			{
				return false;
			}
		}

		return true;
	}

	Matrix& operator *= (const Matrix& mtx)
	{
		Multiply(*this, mtx);
		return *this;
	}

	friend Matrix operator * (const Matrix& m1, const Matrix& m2)
	{
		Matrix m;
		m.Multiply(m1, m2);
		return m;
	}

	friend Vector3 operator * (const Matrix& mtx, const Vector3& v)
	{
		return mtx.MulVertex(v);
	}

	friend Vector3 operator * (const Vector3& v, const Matrix& mtx)
	{
		return mtx.MulVertex(v);
	}

	void Identity()
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
	}

	void SetZero()
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
	}

	void BuildProjection(float viewAngle, float aspect, float zNear, float zFar)
	{
		SetZero();

		double Q = double(zFar) / double(zFar - zNear);
		m[1][1] = float(1.0 / tan(viewAngle*0.5));
		m[0][0] = m[1][1] * aspect;
		m[2][2] = float(Q);
		m[2][3] = 1.0f;
		m[3][2] = float(-Q * zNear);
	}

	bool BuildView(const Vector3& lookFrom, const Vector3& lookTo, const Vector3& upVector)
	{
		Identity();

		Vector3 tmp_lookTo(lookTo);
		Vector3 tmp_upVector(upVector);

		tmp_lookTo -= lookFrom;

		if (tmp_lookTo.Normalize() == 0.0f)
		{
			Pos() = -lookFrom;
			return false;
		}

		tmp_upVector -= tmp_lookTo * tmp_lookTo.Dot(tmp_upVector);

		if (tmp_upVector.Normalize() == 0.0f) tmp_upVector.y = 1.0f;

		Vector3 v = tmp_upVector.Cross(tmp_lookTo);
		if (v.Normalize() > 0.0f)
		{
			m[0][0] = v.x;
			m[1][0] = v.y;
			m[2][0] = v.z;
			m[0][1] = tmp_upVector.x;
			m[1][1] = tmp_upVector.y;
			m[2][1] = tmp_upVector.z;
			m[0][2] = tmp_lookTo.x;
			m[1][2] = tmp_lookTo.y;
			m[2][2] = tmp_lookTo.z;
		}
		else
		{
			Pos() = -lookFrom;
			return false;
		}

		Pos() = -MulNormal(lookFrom);

		return true;
	}

	void RotateX(float ang)
	{
		Matrix m;
		m.m[1][1] = cosf(ang);
		m.m[1][2] = sinf(ang);
		m.m[2][1] = -sinf(ang);
		m.m[2][2] = cosf(ang);

		Multiply(Matrix(*this), m);
	}

	void RotateY(float ang)
	{
		Matrix m;
		m.m[0][0] = cosf(ang);
		m.m[0][2] = -sinf(ang);
		m.m[2][0] = sinf(ang);
		m.m[2][2] = cosf(ang);

		Multiply(Matrix(*this), m);
	}

	void RotateZ(float ang)
	{
		Matrix m;
		m.m[0][0] = cosf(ang);
		m.m[0][1] = sinf(ang);
		m.m[1][0] = -sinf(ang);
		m.m[1][1] = cosf(ang);

		Multiply(Matrix(*this), m);
	}

	void Scale(const Vector3& scale)
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
	}

	bool Inverse()
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
			matrix[i] = (float)(inv[i] * det);
		}

		return true;
	}

	void Transpose()
	{
		float tmp;
		tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
		tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
		tmp = m[0][3]; m[0][3] = m[3][0]; m[3][0] = tmp;
		tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;
		tmp = m[1][3]; m[1][3] = m[3][1]; m[3][1] = tmp;
		tmp = m[2][3]; m[2][3] = m[3][2]; m[3][2] = tmp;
	}

	Vector3 MulVertex(const Vector3& v) const
	{
		Vector3 tv;
		tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
		tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
		tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];

		return tv;
	}

	Vector4 MulVertex4(const Vector3& v) const
	{
		Vector4 tv;
		tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
		tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
		tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
		tv.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];

		return tv;
	}

	Vector3 MulNormal(const Vector3& v)
	{
		Vector3 tv;
		tv.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
		tv.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
		tv.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;

		return tv;
	}

	void Multiply(const Matrix& m1, const Matrix& m2)
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
	}
};