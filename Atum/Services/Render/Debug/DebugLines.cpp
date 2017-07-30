#include "DebugLines.h"
#include "DebugPrograms.h"

void DebugLines::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	buffer = render.GetDevice()->CreateBuffer(MaxSize * 2, sizeof(Vertex));

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

void DebugLines::DrawLines(Program* prog, std::vector<Vertex>& lines)
{
	if (lines.size()==0) return;

	prog->Apply();
	render.GetDevice()->SetVertexBuffer(0, buffer);

	Matrix view_proj;
	render.SetTransform(Render::World, Matrix());
	render.GetTransform(Render::WrldViewProj, view_proj);

	prog->VS_SetMatrix("view_proj", &view_proj, 1);
		
	Vertex* v = (Vertex*)buffer->Lock();

	int count = 0;

	for (int i=0; i<lines.size(); i+=2)
	{
		v[count *2 + 0] = lines[i];
		v[count *2 + 1] = lines[i+1];
		
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
	DrawLines(DebugPrograms::line_with_depth_prg, lines_with_depth);
	DrawLines(DebugPrograms::line_prg, lines);
}

void DebugLines::Release()
{
	RELEASE(buffer)
}