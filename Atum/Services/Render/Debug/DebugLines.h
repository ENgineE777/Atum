
#pragma once

#include "Services/Render/Render.h"

class DebugLines : public Object
{		
	enum
	{
		MaxSize = 10000
	};

	struct Vertex
	{
		Vector p;
		uint32_t color;

		Vertex(Vector set_p, uint32_t set_color)
		{
			p = set_p;
			color = set_color;
		};
	};

	std::vector<Vertex> lines;
	std::vector<Vertex> lines_with_depth;

	GeometryBuffer* buffer;

public:

	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddLine(Vector& from, Color& from_clr, Vector& to, Color& to_clr, bool use_depth);
	void DrawLines(Program* prog, std::vector<Vertex>& lines);
	void Draw(float dt);
	void Release();
};
