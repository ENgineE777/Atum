
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
	std::vector<Vertex> lines_2d;

	Program*        prg;
	Program*        prg_depth;
	VertexDecl*     vdecl;
	GeometryBuffer* buffer;

public:

	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddLine(Vector& from, Color& from_clr, Vector& to, Color& to_clr, bool use_depth);
	void AddLine2D(Vector2& from, Color& from_clr, Vector2& to, Color& to_clr);
	void DrawLines(Program* prog, std::vector<Vertex>& lines, bool is2d);
	void Draw(float dt);
	void Release();
};
