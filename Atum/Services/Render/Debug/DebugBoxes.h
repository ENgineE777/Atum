
#pragma once

#include "Services/Render/Render.h"

class DebugBoxes : public Object
{
	struct Box
	{
		Matrix trans;
		Color  color;
	};

	struct Vertex
	{
		Vector   pos;
		Vector   normal;
		uint32_t color;
	};

	std::vector<Box> boxes;

	Program*        prg;
	GeometryBuffer* vbuffer;
	VertexDecl*     vdecl;
	GeometryBuffer* ibuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddBox(Matrix pos, Color color, Vector scale);
	void Draw(float dt);
	void Release();
};
