
#pragma once

#include "Services/Render/Render.h"

class Terrain : public Object
{
public:
	GeometryBuffer* buffer;
	Texture*        texture;
	int      hwidth;
	int      hheight;
	Vector2  hscale;
	uint8_t* hmap;

	struct VertexTri
	{
		Vector  position;
		Vector2 texCoord;
		Vector  normal;
	};

	int sz;

	void Init(Vector2 scale, const char* tex_name, const char* hgt_name);
	void LoadHMap(const char* hgt_name);
	float GetHight(float x, float z);
	void Render(float dt);
	void ShRender(float dt);
	void Render(Program* prg);
};
