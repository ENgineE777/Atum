
#pragma once

#include "Services/Render/Render.h"

class DebugTriangles : public Object
{
	struct Triangle
	{
		Vector p[3];
		uint32_t color;
	};

	struct Vertex
	{
		Vector pos;
		Vector normal;
		uint32_t color;
	};

	std::vector<Triangle> triangles;
	Program*        prg;
	VertexDecl*     vdecl;
	GeometryBuffer* vbuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddTriangle(Vector p1, Vector p2, Vector p3, Color color);
	void Draw(float dt);
	void Release();
};
