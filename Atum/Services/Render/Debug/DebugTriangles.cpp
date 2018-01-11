#include "DebugTriangles.h"
#include "DebugPrograms.h"

void DebugTriangles::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	vbuffer = render.GetDevice()->CreateBuffer(3000, sizeof(Vertex));

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugTriangles::Draw);
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

	DebugPrograms::tri_prg->Apply();

	render.GetDevice()->SetVertexBuffer(0, vbuffer);

	Matrix view_proj;
	render.SetTransform(Render::World, Matrix());
	render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix view;
	render.GetTransform(Render::View, view);
	view.Inverse();
	Vector4 vz = Vector4(-view.Vz());

	DebugPrograms::tri_prg->VS_SetMatrix("view_proj", &view_proj, 1);
	DebugPrograms::tri_prg->PS_SetVector("lightDir", &vz, 1);

	Matrix trans;
	Color color = COLOR_WHITE;
	DebugPrograms::tri_prg->VS_SetMatrix("trans", &trans, 1);
	DebugPrograms::tri_prg->PS_SetVector("color", (Vector4*)&color, 1);

	int index = 0;
	Vertex* vertices = (Vertex*)vbuffer->Lock();

	for (int i=0;i<triangles.size();i++)
	{
		Triangle& triangle = triangles[i];

		Vector normal = (triangle.p[0] - triangle.p[1]);
		normal.Cross(triangle.p[2] - triangle.p[1]);

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