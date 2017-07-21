
#pragma once

#include "Services/Render/Render.h"

class FreeCamera : public Object
{
public:

	Vector2 angles;
	Vector  pos;
	Matrix  view;
	Matrix  proj;

	void Init();

	void Update(float dt);
};
