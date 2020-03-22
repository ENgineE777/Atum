
#pragma once

#include "Services/Render/Render.h"

class DebugTriangles : public Object
{
	struct Triangle
	{
		Vector3 p[3];
		uint32_t color;
	};

	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		uint32_t color;
	};

	std::vector<Triangle> triangles;
	Program* prg;
	VertexDecl* vdecl;
	DataBuffer* vbuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color);
	void Draw(float dt);
	void Release();
};
