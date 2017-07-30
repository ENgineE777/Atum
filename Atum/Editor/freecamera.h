
#pragma once

#include "Support/Support.h"

class FreeCamera : public Object
{
public:

	Vector2 angles;
	Vector  pos;
	Matrix  view;
	Matrix  proj;

	int alias_forward;
	int alias_strafe;
	int alias_fast;
	int alias_rotate_active;
	int alias_rotate_x;
	int alias_rotate_y;

	void Init();

	void OnResize(int width, int height);
	void Update(float dt);
};
