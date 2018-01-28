
#pragma once

#include "Vector2.h"
#include "Matrix.h"

typedef unsigned char  byte;
typedef signed short   int16;
typedef unsigned short word;

#ifdef __LP64__
typedef unsigned int  dword;
#else
typedef unsigned long dword;
#endif

typedef signed long long int   int64;
typedef unsigned long long int uint64;

struct Transform2D
{
	Vector2 pos = 0.0f;
	Vector2 size = 10.0f;
	float   rotation = 0.0f;
	Vector2 offset = 0.5f;
	Matrix  local_trans;
	void BuildLocalTrans()
	{
		local_trans.Identity();
		local_trans.RotateZ(rotation);
		local_trans.Pos() = Vector(pos.x, pos.y, 0.0f);
	}
};