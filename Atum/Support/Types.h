
#pragma once

#include "Vector2.h"
#include "Matrix.h"

struct Transform2D
{
	Vector2 pos = 0.0f;
	float   depth = 0.5f;
	Vector2 size = 10.0f;
	float   rotation = 0.0f;
	Vector2 offset = 0.5f;
	Matrix  local_trans;
	void BuildLocalTrans()
	{
		local_trans.Identity();
		local_trans.RotateZ(rotation);
		local_trans.Pos() = Vector(pos.x, pos.y, depth);
	}
};