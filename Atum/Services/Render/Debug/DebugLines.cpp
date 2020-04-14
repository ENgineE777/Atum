#include "DebugLines.h"
#include "DebugPrograms.h"

#include "Services/Font/Fonts.h"

void DebugLines::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 }, { VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(2, desc);

	buffer = core.render.GetDevice()->CreateBuffer(MaxSize * 2, sizeof(Vertex));

	prg = core.render.GetProgram("DbgLine");
	prg_depth = core.render.GetProgram("DbgLineWithDepth");

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugLines::Draw);
}

void DebugLines::AddLine(Vector3 from, Color from_clr, Vector3 to, Color to_clr, bool use_depth)
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

void DebugLines::AddLine2D(Vector2 from, Color from_clr, Vector2 to, Color to_clr)
{
	lines_2d.push_back(Vertex(Vector3(from.x, from.y, 1.0f), from_clr.Get()));
	lines_2d.push_back(Vertex(Vector3(to.x, to.y, 1.0f), to_clr.Get()));
}

void DebugLines::DrawCircle(int axis, Vector3 pos, Color color, float radius)
{
	float last_dx = -radius * 2.0f;
	float last_dz = -radius * 2.0f;
	int nums = 32;

	for (int i = 0; i < nums + 1; i++)
	{
		float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * radius;
		float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * radius;

		if (last_dx > -radius * 1.5f)
		{
			Vector3 ps;
			Vector3 ps2;

			if (axis == 0)
			{
				ps.Set(0.0f, last_dx, last_dz);
				ps2.Set(0.0f, dx, dz);
			}
			else
			if (axis == 1)
			{
				ps.Set(last_dx, 0.0f, last_dz);
				ps2.Set(dx, 0.0f, dz);
			}
			else
			{
				ps.Set(last_dx, last_dz, 0.0f);
				ps2.Set(dx, dz, 0.0f);
			}

			ps += pos;
			ps2 += pos;

			AddLine(ps, color, ps2, color, false);
		}

		last_dx = dx;
		last_dz = dz;
	}
}

void DebugLines::DrawLines(Program* prog, std::vector<Vertex>& lines, bool is2d)
{
	if (lines.size()==0) return;

	core.render.GetDevice()->SetProgram(prog);

	core.render.GetDevice()->SetVertexDecl(vdecl);
	core.render.GetDevice()->SetVertexBuffer(0, buffer);

	Matrix view_proj;
	core.render.SetTransform(Render::World, Matrix());
	core.render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix view, proj, inv_view;

	if (is2d)
	{
		core.render.GetTransform(Render::View, view);
		core.render.GetTransform(Render::Projection, proj);

		inv_view = view;
		inv_view.Inverse();
	}

	prog->SetMatrix(Shader::Type::Vertex, "view_proj", &view_proj, 1);

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
				v[count * 2 + j].p.x = (2.0f * v[count * 2 + j].p.x / (float)core.render.GetDevice()->GetWidth() - 1) / proj._11;
				v[count * 2 + j].p.y = -(2.0f * v[count * 2 + j].p.y / (float)core.render.GetDevice()->GetHeight() - 1) / proj._22;

				Vector3 dir = inv_view.MulNormal(v[count * 2 + j].p);
				v[count * 2 + j].p = inv_view.Pos() + dir * 10.0f;
			}
		}

		count++;

		if (count == MaxSize)
		{
			buffer->Unlock();

			core.render.GetDevice()->Draw(Device::LinesList, 0, MaxSize);

			count = 0;
			v = (Vertex*)buffer->Lock();
		}
	}
	
	buffer->Unlock();

	if (count>0)
	{
		core.render.GetDevice()->Draw(Device::LinesList, 0, count);
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