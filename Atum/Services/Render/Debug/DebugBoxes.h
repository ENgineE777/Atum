
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
		Vector3  pos;
		Vector3  normal;
		uint32_t color;
	};

	std::vector<Box> boxes;

	Program* prg;
	DataBuffer* vbuffer;
	VertexDecl* vdecl;
	DataBuffer* ibuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddBox(Matrix pos, Color color, Vector3 scale);
	void Draw(float dt);
	void Release();
};
