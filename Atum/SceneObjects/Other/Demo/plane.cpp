
#include "plane.h"
#include "programs.h"

void Plane::Init(float plane_y, const char* tex_name)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float2, VertexDecl::Texcoord, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(2, desc);

	int stride = sizeof(VertexTri);
	buffer = render.GetDevice()->CreateBuffer(6, stride);

	VertexTri* v_tri = (VertexTri*)buffer->Lock();

	float plane_size = 75.0f;
	float plane_tiling = plane_size * 0.3f;

	v_tri[0].position = Vector(-plane_size, plane_y, plane_size);
	v_tri[0].texCoord = Vector2(0.0f, 0.0f);
	v_tri[1].position = Vector(plane_size, plane_y, plane_size);
	v_tri[1].texCoord = Vector2(plane_tiling, 0.0f);
	v_tri[2].position = Vector(plane_size, plane_y, -plane_size);
	v_tri[2].texCoord = Vector2(plane_tiling, plane_tiling);

	v_tri[3].position = Vector(-plane_size, plane_y, plane_size);
	v_tri[3].texCoord = Vector2(0.0f, 0.0f);
	v_tri[4].position = Vector(plane_size, plane_y, -plane_size);
	v_tri[4].texCoord = Vector2(plane_tiling, plane_tiling);
	v_tri[5].position = Vector(-plane_size, plane_y, -plane_size);
	v_tri[5].texCoord = Vector2(0.0f, plane_tiling);

	buffer->Unlock();

	texture = render.LoadTexture("Media//grass.tga");

	//render.AddDelegate("geometry", this, (Object::Delegate)&Plane::Render, 0);
	//render.AddDelegate("shgeometry", this, (Object::Delegate)&Plane::ShRender, 0);
}

void Plane::Render(float dt)
{
	Render(Programs::GetTranglPrg());
}

void Plane::ShRender(float dt)
{
	Render(Programs::GetShdTranglPrg());
}

void Plane::Render(Program* prg)
{
	render.GetDevice()->SetVertexDecl(vdecl);
	render.GetDevice()->SetVertexBuffer(0, buffer);

	render.GetDevice()->SetProgram(prg);

	Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);

	render.SetTransform(Render::World, Matrix());

	Matrix trans;
	render.GetTransform(Render::WrldViewProj, trans);

	Matrix mat;
	prg->SetMatrix(Program::Vertex, "world", &mat, 1);
	prg->SetMatrix(Program::Vertex, "trans", &trans, 1);
	prg->SetVector(Program::Pixel, "color", &color, 1);
	prg->SetTexture(Program::Pixel, "diffuseMap", texture);

	render.GetDevice()->Draw(Device::TrianglesList, 0, 2);
}