
#pragma once

#include "Vector2.h"
#include "Matrix.h"

struct Transform2D
{
	Vector2 pos = 0.0f;
	Vector2 axis = 1.0f;
	float   depth = 0.5f;
	Vector2 size = 10.0f;
	float   rotation = 0.0f;
	Vector2 offset = 0.5f;
	Matrix  mat_local;
	Matrix  mat_parent;
	Matrix  mat_global;

	void BuildMatrices()
	{
		mat_local.Identity();
		mat_local.RotateZ(rotation);
		mat_local.Pos() = Vector(axis.x > 0.0f ? pos.x : -pos.x - size.x, axis.y > 0.0f ? pos.y : -pos.y - size.y, depth);

		mat_global = mat_local * mat_parent;
	}
};