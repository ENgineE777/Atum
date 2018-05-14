#include "DebugSpheres.h"
#include "DebugPrograms.h"

void DebugSpheres::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 0 },{ VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(3, desc);

	vbuffer = render.GetDevice()->CreateBuffer(SidesCount * (RigsCount + 1), sizeof(Vertex));

	Vertex* vertices = (Vertex*)vbuffer->Lock();

	float deltaTheta = PI / RigsCount;
	float deltaPhi = 2.0f * PI / SidesCount;
	float curTheta = 0.0f;

	int index = 0;

	for (int i = 0; i <= RigsCount; i++)
	{
		float y = cosf(curTheta);
		float sinTheta = sinf(curTheta);
		float curPhi = 0.0f;

		for (int j = 0; j < SidesCount; j++)
		{
			vertices[index].pos = Vector(sinTheta * sinf(curPhi), y, -sinTheta * cosf(curPhi));
			vertices[index].normal = vertices[index].pos;
			vertices[index].normal.Normalize();
			vertices[index].color = 0xffffffff;
			index++;

			curPhi += deltaPhi;
		}

		curTheta += deltaTheta;
	}

	vbuffer->Unlock();

	ibuffer = render.GetDevice()->CreateBuffer(PrimCount * 3, sizeof(int));

	int* indices = (int*)ibuffer->Lock();

	index = 0;

	for (int i = 0; i < RigsCount; i++)
	{
		int top_index = i * SidesCount;
		int bottom_index = (i+1) * SidesCount;

		for (int j = 0; j < SidesCount; j++)
		{
			indices[index + 0] = (top_index + ((j + 0) % SidesCount));
			indices[index + 1] = (bottom_index + ((j + 1) % SidesCount));
			indices[index + 2] = (bottom_index + ((j + 0) % SidesCount));

			indices[index + 3] = (bottom_index + ((j + 1) % SidesCount));
			indices[index + 4] = (top_index + ((j + 0) % SidesCount));
			indices[index + 5] = (top_index + ((j + 1) % SidesCount));
			index += 6;
		}
	}

	ibuffer->Unlock();

	prg = render.GetProgram("DbgTriangle");

	debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugSpheres::Draw);
}

void DebugSpheres::AddSphere(Vector& pos, Color& color, float radius)
{
	spheres.push_back(Sphere());
	Sphere* sphere = &spheres[spheres.size()-1];

	sphere->pos = pos;
	sphere->color = color;
	sphere->radius = radius;
}

void DebugSpheres::Draw(float dt)
{
	if (spheres.size() == 0)
	{
		return;
	}

	render.GetDevice()->SetProgram(prg);

	render.GetDevice()->SetVertexDecl(vdecl);
	render.GetDevice()->SetVertexBuffer(0, vbuffer);
	render.GetDevice()->SetIndexBuffer(ibuffer);

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

	render.GetDevice()->SetAlphaBlend(true);

	for (int i=0;i<spheres.size();i++)
	{
		Sphere& sphere = spheres[i];

		Matrix mat;
		Vector scale = Vector(sphere.radius);
		mat.Scale(scale);
		mat.Pos() = sphere.pos;

		prg->SetMatrix(Program::Vertex, "trans", &mat, 1);
		prg->SetVector(Program::Pixel, "color", (Vector4*)&sphere.color, 1);

		render.GetDevice()->DrawIndexed(Device::TrianglesList, 0, 0, PrimCount);
	}

	render.GetDevice()->SetAlphaBlend(false);

	spheres.clear();
}

void DebugSpheres::Release()
{
	RELEASE(vbuffer)
	RELEASE(ibuffer)
}