
#pragma once

#include "Math/Vector2.h"
#include "Math/Matrix.h"

/**
\ingroup gr_code_common
*/

/**
\brief Transform2D

This class used for storing a transform of 2D objects.

*/

struct Transform2D
{
	/**
	\brief Position of a object
	*/
	Vector2 pos = 0.0f;
	Vector2 axis = 1.0f;

	/**
	\brief Z depth
	*/
	float   depth = 0.5f;

	/**
	\brief Width and height
	*/
	Vector2 size = 10.0f;

	/**
	\brief Rotation around anchor
	*/
	float   rotation = 0.0f;

	/**
	\brief Offset of anchorn in absolute units, i.e. 1 equals to width/height
	*/
	Vector2 offset = 0.5f;

	/**
	\brief Calulated local matrix
	*/
	Matrix  mat_local;

	/**
	\brief Matrix of a parent
	*/
	Matrix  mat_parent;

	/**
	\brief Final matrix
	*/
	Matrix  mat_global;

	/**
	\brief Calculate final matrix
	*/
	void BuildMatrices()
	{
		mat_local.Identity();
		mat_local.RotateZ(rotation);
		mat_local.Pos() = Vector3(axis.x > 0.0f ? pos.x : -pos.x - size.x, axis.y > 0.0f ? pos.y : -pos.y - size.y, depth);

		mat_global = mat_local * mat_parent;
	}
};

enum ScreenCorner
{
	LeftTop,
	RightTop,
	LeftBottom,
	RightBottom
};
