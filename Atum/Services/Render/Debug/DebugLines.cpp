#include "DebugLines.h"
#include "DebugPrograms.h"

#include "Services/Font/Fonts.h"

void DebugLines::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 }, { VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(2, desc);

	buffer = render.GetDevice()->CreateBuffer(MaxSize * 2, sizeof(Vertex));

	prg = render.GetProgram("DbgLine");
	prg_depth = render.GetProgram("DbgLineWithDepth");

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugLines::Draw);
}

void DebugLines::AddLine(Vector& from, Color& from_clr, Vector& to, Color& to_clr, bool use_depth)
{
	std::vector<Vertex>* ln;

	if (use_depth)
	{
		ln = &lines_with_depth;
	}
	else
	{
		ln = &lines;
	}

	ln->push_back(Vertex(from, from_clr.Get()));
	ln->push_back(Vertex(to, to_clr.Get()));
}

void DebugLines::AddLine2D(Vector2& from, Color& from_clr, Vector2& to, Color& to_clr)
{
	lines_2d.push_back(Vertex(Vector(from.x, from.y, 1.0f), from_clr.Get()));
	lines_2d.push_back(Vertex(Vector(to.x, to.y, 1.0f), to_clr.Get()));
}

void DebugLines::DrawLines(Program* prog, std::vector<Vertex>& lines, bool is2d)
{
	if (lines.size()==0) return;

	render.GetDevice()->SetProgram(prog);

	render.GetDevice()->SetVertexDecl(vdecl);
	render.GetDevice()->SetVertexBuffer(0, buffer);

	Matrix view_proj;
	render.SetTransform(Render::World, Matrix());
	render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix view, proj, inv_view;

	if (is2d)
	{
		render.GetTransform(Render::View, view);
		render.GetTransform(Render::Projection, proj);

		inv_view = view;
		inv_view.InverseComplette();
	}

	prog->SetMatrix(Program::Vertex, "view_proj", &view_proj, 1);

	Vertex* v = (Vertex*)buffer->Lock();

	int count = 0;

	for (int i=0; i<lines.size(); i+=2)
	{
		v[count * 2 + 0] = lines[i];
		v[count * 2 + 1] = lines[i+1];

		if (is2d)
		{
			for (int j = 0; j < 2; j++)
			{
				v[count * 2 + j].p.x = (2.0f * v[count * 2 + j].p.x / (float)render.GetDevice()->GetWidth() - 1) / proj._11;
				v[count * 2 + j].p.y = -(2.0f * v[count * 2 + j].p.y / (float)render.GetDevice()->GetHeight() - 1) / proj._22;

				Vector dir = inv_view.MulNormal(v[count * 2 + j].p);
				v[count * 2 + j].p = inv_view.Pos() + dir * 10.0f;
			}
		}

		count++;

		if (count == MaxSize)
		{
			buffer->Unlock();

			render.GetDevice()->Draw(Device::LinesList, 0, MaxSize);

			count = 0;
			v = (Vertex*)buffer->Lock();
		}
	}
	
	buffer->Unlock();

	if (count>0)
	{
		render.GetDevice()->Draw(Device::LinesList, 0, count);
	}

	lines.clear();
}

void DebugLines::Draw(float dt)
{
	DrawLines(prg_depth, lines_with_depth, false);
	DrawLines(prg, lines, false);
	DrawLines(prg, lines_2d, true);
}

void DebugLines::Release()
{
	RELEASE(buffer)
}