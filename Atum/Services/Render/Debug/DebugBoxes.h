
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
		Vector pos;
		Vector normal;
	};

	std::vector<Box> boxes;

	GeometryBuffer* vbuffer;
	GeometryBuffer* ibuffer;

public:
	
	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddBox(Matrix& pos, Color& color, Vector& scale);
	void Draw(float dt);
	void Release();
};
