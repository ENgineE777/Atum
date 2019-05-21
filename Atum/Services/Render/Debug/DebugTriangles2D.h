
#pragma once

#include "Services/Render/Render.h"

class DebugTriangles2D : public Object
{
	struct Triangle
	{
		Vector2 p[3];
		uint32_t color;
	};

	struct Vertex
	{
		Vector2  pos;
		uint32_t color;
	};

	std::vector<Triangle> triangles;
	Program* prg;
	VertexDecl* vdecl;
	DataBuffer* vbuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
	void Draw(float dt);
	void Release();
};
