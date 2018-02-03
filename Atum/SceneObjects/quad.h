
#pragma once

#include "Services/Render/Render.h"

class Quad
{
public:

	GeometryBuffer* buffer;

	void Init();
	void Draw(Texture* texture, Matrix& trans, Vector2& pos, Vector2& size, Vector2 uv = 0.0f, Vector2 duv = 1.0f);
};
