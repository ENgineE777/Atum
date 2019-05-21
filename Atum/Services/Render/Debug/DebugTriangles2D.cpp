#include "DebugTriangles2D.h"
#include "DebugPrograms.h"

void DebugTriangles2D::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float2, VertexDecl::Position, 0 },{ VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(2, desc);

	vbuffer = core.render.GetDevice()->CreateBuffer(3000, sizeof(Vertex));

	prg = core.render.GetProgram("DbgTriangle2D");

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugTriangles2D::Draw);
}

void DebugTriangles2D::AddTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color)
{
	triangles.push_back(Triangle());
	Triangle* triangle = &triangles[triangles.size()-1];

	triangle->p[0] = p1;
	triangle->p[1] = p2;
	triangle->p[2] = p3;
	triangle->color = color.Get();
}

void DebugTriangles2D::Draw(float dt)
{
	if (triangles.size() == 0)
	{
		return;
	}

	core.render.GetDevice()->SetProgram(prg);

	core.render.GetDevice()->SetVertexDecl(vdecl);
	core.render.GetDevice()->SetVertexBuffer(0, vbuffer);

	Vector4 param = Vector4((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight(), 0, 0);
	prg->SetVector(Shader::Type::Vertex, "param", &param, 1);

	int index = 0;
	Vertex* vertices = (Vertex*)vbuffer->Lock();

	for (int i=0;i<triangles.size();i++)
	{
		Triangle& triangle = triangles[i];

		for (int j = 0; j < 3; j++)
		{
			vertices[index * 3 + j].pos = triangle.p[j];
			vertices[index * 3 + j].color = triangle.color;
		}

		index++;

		if (index > 330)
		{
			vbuffer->Unlock();
			core.render.GetDevice()->Draw(Device::TrianglesList, 0, index);

			index = 0;
			vertices = (Vertex*)vbuffer->Lock();
		}
	}

	vbuffer->Unlock();

	if (index > 0)
	{
		core.render.GetDevice()->Draw(Device::TrianglesList, 0, index);
	}

	triangles.clear();
}

void DebugTriangles2D::Release()
{
	RELEASE(vbuffer)
}