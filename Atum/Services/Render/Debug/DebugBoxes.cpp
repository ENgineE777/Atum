#include "DebugBoxes.h"

void DebugBoxes::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 0 },{ VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(3, desc);

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
		vertices[i].color = 0xffffffff;
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

	prg = render.GetProgram("DbgTriangle");

	debugTaskPool->AddTask(199, this, (Object::Delegate)&DebugBoxes::Draw);
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

	render.GetDevice()->SetAlphaBlend(false);

	for (int i=0;i<boxes.size();i++)
	{
		Box& box = boxes[i];

		prg->SetMatrix(Program::Vertex, "trans", &box.trans, 1);
		prg->SetVector(Program::Pixel, "color", (Vector4*)&box.color, 1);

		render.GetDevice()->DrawIndexed(Device::TrianglesList, 0, 0, 12);
	}

	boxes.clear();
}

void DebugBoxes::Release()
{
	RELEASE(vbuffer)
	RELEASE(ibuffer)
}