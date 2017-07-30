#include "DebugSpheres.h"
#include "DebugPrograms.h"

void DebugSpheres::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
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

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugSpheres::Draw);
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

	DebugPrograms::tri_prg->Apply();

	render.GetDevice()->SetVertexBuffer(0, vbuffer);
	render.GetDevice()->SetIndexBuffer(ibuffer);

	Matrix view_proj;
	render.SetTransform(Render::World, Matrix());
	render.GetTransform(Render::WrldViewProj, view_proj);

	DebugPrograms::tri_prg->VS_SetMatrix("view_proj", &view_proj, 1);

	render.GetDevice()->SetAlphaBlend(true);

	for (int i=0;i<spheres.size();i++)
	{
		Sphere& sphere = spheres[i];

		Matrix mat;
		mat.Scale(Vector(sphere.radius));
		mat.Pos() = sphere.pos;

		DebugPrograms::tri_prg->VS_SetMatrix("trans", &mat, 1);
		DebugPrograms::tri_prg->PS_SetVector("color", (Vector4*)&sphere.color, 1);

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