
#pragma once

#include "Services/Render/Render.h"

class ColorCorrection
{
public:

	GeometryBuffer* buffer;
	Texture* ring_rt[2];

	void Init();
	void Draw(Texture* rt);
};
