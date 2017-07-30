#include "DebugBoxes.h"
#include "DebugPrograms.h"

void DebugBoxes::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	vbuffer = render.GetDevice()->CreateBuffer(24, sizeof(Vertex));

	Vertex* vertices = (Vertex*)vbuffer->Lock();

	Vector base_vertices[] =
	{
		Vector(-0.5f, 0.5f, -0.5f),
		Vector(0.5f, 0.5f, -0.5f),
		Vector(-0.5f, -0.5f, -0.5f),
		Vector(0.5f, -0.5f, -0.5f),
		Vector(-0.5f, 0.5f, 0.5f),
		Vector(0.5f, 0.5f, 0.5f),
		Vector(-0.5f, -0.5f, 0.5f),
		Vector( 0.5f, -0.5f, 0.5f)
	};

	int bace_indices[] =
	{
		0, 1, 2, 2, 1, 3,
		4, 0, 6, 6, 0, 2,
		7, 5, 6, 6, 5, 4,
		3, 1, 7, 7, 1, 5,
		4, 5, 0, 0, 5, 1,
		3, 7, 2, 2, 7, 6
	};

	int box_indices[] =
	{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9,10,10, 9,11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23
	};

	for (int i = 0; i < 36; i++)
	{
		vertices[box_indices[i]].pos = base_vertices[bace_indices[i]];
	}

	Vector noramls[] =
	{
		Vector( 0.0f, 0.0f,-1.0f),
		Vector( 0.0f, 0.0f, 1.0f),
		Vector(-1.0f, 0.0f, 0.0f),
		Vector( 1.0f, 0.0f, 0.0f),
		Vector( 0.0f, 1.0f, 0.0f),
		Vector( 0.0f,-1.0f, 0.0f)
	};

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			vertices[i * 4 + j].normal = noramls[i];
		}
	}

	vbuffer->Unlock();

	ibuffer = render.GetDevice()->CreateBuffer(12 * 3, sizeof(int));

	int* indices = (int*)ibuffer->Lock();

	for (int i = 0; i < 36; i++)
	{
		indices[i] = box_indices[i];
	}

	ibuffer->Unlock();

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugBoxes::Draw);
}

void DebugBoxes::AddBox(Matrix& trans, Color& color, Vector& scale)
{
	boxes.push_back(Box());
	Box* box = &boxes[boxes.size()-1];

	box->trans = Matrix().Scale(scale) * trans;
	box->color = color;
}

void DebugBoxes::Draw(float dt)
{
	if (boxes.size() == 0)
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

	//render.GetDevice()->SetAlphaBlend(true);

	for (int i=0;i<boxes.size();i++)
	{
		Box& box = boxes[i];

		DebugPrograms::tri_prg->VS_SetMatrix("trans", &box.trans, 1);
		DebugPrograms::tri_prg->PS_SetVector("color", (Vector4*)&box.color, 1);

		render.GetDevice()->DrawIndexed(Device::TrianglesList, 0, 0, 12);
	}

	render.GetDevice()->SetAlphaBlend(true);

	boxes.clear();
}

void DebugBoxes::Release()
{
	RELEASE(vbuffer)
	RELEASE(ibuffer)
}