#include "DebugTriangles.h"
#include "DebugPrograms.h"

void DebugTriangles::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 0 },{ VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(3, desc);

	vbuffer = render.GetDevice()->CreateBuffer(3000, sizeof(Vertex));

	prg = render.GetProgram("DbgTriangle");

	debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugTriangles::Draw);
}

void DebugTriangles::AddTriangle(Vector& p1, Vector& p2, Vector& p3, Color& color)
{
	triangles.push_back(Triangle());
	Triangle* triangle = &triangles[triangles.size()-1];

	triangle->p[0] = p1;
	triangle->p[1] = p2;
	triangle->p[2] = p3;
	triangle->color = color.Get();
}

void DebugTriangles::Draw(float dt)
{
	if (triangles.size() == 0)
	{
		return;
	}

	render.GetDevice()->SetProgram(prg);

	render.GetDevice()->SetVertexDecl(vdecl);
	render.GetDevice()->SetVertexBuffer(0, vbuffer);

	Matrix view_proj;
	Matrix tmp;
	render.SetTransform(Render::World, tmp);
	render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix view;
	render.GetTransform(Render::View, view);
	view.Inverse();
	Vector4 vz = Vector4(-view.Vz());

	prg->SetMatrix(Program::Vertex, "view_proj", &view_proj, 1);
	prg->SetVector(Program::Pixel, "lightDir", &vz, 1);

	Matrix trans;
	Color color = COLOR_WHITE;
	prg->SetMatrix(Program::Vertex, "trans", &trans, 1);
	prg->SetVector(Program::Pixel, "color", (Vector4*)&color, 1);

	int index = 0;
	Vertex* vertices = (Vertex*)vbuffer->Lock();

	for (int i=0;i<triangles.size();i++)
	{
		Triangle& triangle = triangles[i];

		Vector normal = (triangle.p[0] - triangle.p[1]);
		Vector dir = triangle.p[2] - triangle.p[1];
		normal.Cross(dir);

		for (int j = 0; j < 3; j++)
		{
			vertices[index * 3 + j].pos = triangle.p[j];
			vertices[index * 3 + j].normal = normal;
			vertices[index * 3 + j].color = triangle.color;
		}

		index++;

		if (index > 330)
		{
			vbuffer->Unlock();
			render.GetDevice()->Draw(Device::TrianglesList, 0, index);

			index = 0;
			vertices = (Vertex*)vbuffer->Lock();
		}
	}

	vbuffer->Unlock();

	if (index > 0)
	{
		render.GetDevice()->Draw(Device::TrianglesList, 0, index);
	}

	triangles.clear();
}

void DebugTriangles::Release()
{
	RELEASE(vbuffer)
}