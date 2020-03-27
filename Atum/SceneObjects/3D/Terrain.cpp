
#include "Terrain.h"
#include "SceneObjects/Other/Demo/programs.h"
#include "Services/Render/Render.h"
#include "Services/File/Files.h"

CLASSREG(SceneObject, Terrain, "Terrain")

META_DATA_DESC(Terrain)
BASE_SCENE_OBJ_PROP(Terrain)
FLOAT_PROP(Terrain, scaleh, 0.5f, "Geometry", "ScaleH", "Horizontal scale")
FLOAT_PROP(Terrain, scalev, 0.1f, "Geometry", "ScaleV", "Vertical scale")
FILENAME_PROP(Terrain, tex_name, "", "Geometry", "Texture")
FILENAME_PROP(Terrain, hgt_name, "", "Geometry", "Heightmap")
COLOR_PROP(Terrain, color, COLOR_WHITE, "Geometry", "Color")
META_DATA_DESC_END()

Terrain::Terrain() : SceneObject()
{

}

Terrain::~Terrain()
{

}

bool Terrain::Is3DObject()
{
	return true;
}

void Terrain::Init()
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float2, VertexDecl::Texcoord, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 1 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(3, desc);

	RenderTasks(false)->AddTask(ExecuteLevels::Geometry, this, (Object::Delegate)&Terrain::Render);

	GetScene()->AddToGroup(this, "Terrain");
}

void Terrain::ApplyProperties()
{
	int stride = sizeof(VertexTri);

	LoadHMap(hgt_name.c_str());

	sz = (hwidth) * (hheight) * 2;

	RELEASE(buffer)
	buffer = core.render.GetDevice()->CreateBuffer(sz * 3, stride);

	VertexTri* v_tri = (VertexTri*)buffer->Lock();

	float start_x = -hwidth * 0.5f * scaleh;
	float start_z = -hheight * 0.5f * scaleh;

	float du = 1.0f / (hwidth - 1.0f);
	float dv = 1.0f / (hheight - 1.0f);

	bool shift = false;

	for (int j = 0; j < hheight - 1; j++)
	{
		for (int i = 0; i < hwidth - 1; i++)
		{
			if (shift)
			{
				if (i % 2 == 0)
				{
					v_tri[1].position = GetVecHeight(i,j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = GetVecHeight(i+1, j);
					v_tri[0].texCoord = Vector2(1.0f, 0.0f);
					v_tri[2].position = GetVecHeight(i+1, j+1);
					v_tri[2].texCoord = Vector2(1.0f, 1.0f);

					v_tri[4].position = GetVecHeight(i, j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = GetVecHeight(i + 1, j + 1);
					v_tri[3].texCoord = Vector2(1.0f, 1.0f);
					v_tri[5].position = GetVecHeight(i, j + 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
				else
				{
					v_tri[1].position = GetVecHeight(i+1,j);
					v_tri[1].texCoord = Vector2(1.0f, 0.0f);
					v_tri[0].position = GetVecHeight(i+1,j+1);
					v_tri[0].texCoord = Vector2(1.0f, 1.0f);
					v_tri[2].position = GetVecHeight(i, j+1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = GetVecHeight(i, j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = GetVecHeight(i+1, j);
					v_tri[3].texCoord = Vector2(1.0f, 0.0f);
					v_tri[5].position = GetVecHeight(i, j + 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
			}
			else
			{
				if (i % 2 == 0)
				{
					v_tri[1].position = GetVecHeight(i, j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = GetVecHeight(i + 1, j);
					v_tri[0].texCoord = Vector2(1.0f, 0.0f);
					v_tri[2].position = GetVecHeight(i, j + 1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = GetVecHeight(i + 1, j);
					v_tri[4].texCoord = Vector2(1.0f, 0.0f);
					v_tri[3].position = GetVecHeight(i + 1, j + 1);
					v_tri[3].texCoord = Vector2(1.0f, 1.0f);
					v_tri[5].position = GetVecHeight(i, j + 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
				else
				{
					v_tri[1].position = GetVecHeight(i, j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = GetVecHeight(i + 1, j + 1);
					v_tri[0].texCoord = Vector2(1.0f, 1.0f);
					v_tri[2].position = GetVecHeight(i, j + 1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = GetVecHeight(i, j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = GetVecHeight(i + 1, j);
					v_tri[3].texCoord = Vector2(1.0f, 0.0f);
					v_tri[5].position = GetVecHeight(i + 1, j + 1);
					v_tri[5].texCoord = Vector2(1.0f, 1.0f);
				}
			}

			float cur_du = du * i;
			float cur_dv = dv * j;

			for (int k = 0; k < 6; k++)
			{
				v_tri[k].position.x = start_x + v_tri[k].position.x * scaleh;
				v_tri[k].position.z = start_z - v_tri[k].position.z * scaleh;
				v_tri[k].texCoord = Vector2(cur_du + v_tri[k].texCoord.x * du, cur_dv + v_tri[k].texCoord.y * dv);
			}

			for (int k = 0; k < 2; k++)
			{
				Vector3 v1 = v_tri[k * 3 + 1].position - v_tri[k * 3 + 0].position;
				Vector3 v2 = v_tri[k * 3 + 2].position - v_tri[k * 3 + 0].position;
				
				v1.Normalize();
				v2.Normalize();

				Vector3 normal;
				
				normal = v1.Cross(v2);
				normal.Normalize();

				for (int l = 0; l < 3; l++)
				{
					v_tri[k * 3 + l].normal = normal;
				}
			}

			v_tri += 6;
		}

		shift = !shift;
	}

	buffer->Unlock();

	RELEASE(texture)
	texture = core.render.LoadTexture(tex_name.c_str());
}

float Terrain::GetHeight(int i, int j)
{
	return hmap ? hmap[((j)* hwidth + i)] * scalev : 1.0f;
}

Vector3 Terrain::GetVecHeight(int i, int j)
{
	return Vector3((float)i, GetHeight(i, j), -(float)j);
}

void Terrain::LoadHMap(const char* hgt_name)
{
	FileInMemory hbuffer;

	FREE_PTR(hmap)

	uint8_t* ptr = nullptr;
	int colorMode = 4;

	if (!hbuffer.Load(hgt_name))
	{
		hwidth = 512;
		hheight = 512;
	}
	else
	{
		ptr = hbuffer.GetData();
		ptr += 2;

		uint8_t imageTypeCode = *((uint8_t*)ptr);

		if (imageTypeCode != 2 && imageTypeCode != 3)
		{
			return;
		}

		ptr += 10;

		short int imageWidth = *((short int*)ptr);
		ptr += 2;

		short int imageHeight = *((short int*)ptr);
		ptr += 2;

		uint8_t bitCount = *((uint8_t*)ptr);
		ptr += 2;

		colorMode = bitCount / 8;

		hwidth = imageWidth;
		hheight = imageHeight;
	}

	hmap = (uint8_t*)malloc(hwidth * hheight);

	for (int i = 0; i < hheight; i++)
	{
		for (int j = 0; j < hwidth; j++)
		{
			hmap[i * hheight + j] = ptr ? ptr[((j)* hwidth + i) * colorMode] : 0;
		}
	}

#ifdef PLATFORM_PC
	string cooked_name = hgt_name + string("hm");

	if (!core.files.IsFileExist(cooked_name.c_str()))
	{
		core.physics.CookHeightmap(hwidth, hheight, hmap, cooked_name.c_str());
	}
#endif
}

void Terrain::Render(float dt)
{
	Render(Programs::GetTranglPrg());
}

void Terrain::ShRender(float dt)
{
	Render(Programs::GetShdTranglPrg());
}

void Terrain::Render(Program* prg)
{
	core.render.GetDevice()->SetVertexDecl(vdecl);
	core.render.GetDevice()->SetVertexBuffer(0, buffer);

	core.render.GetDevice()->SetProgram(prg);

	core.render.SetTransform(Render::World, Matrix());

	Matrix view_proj;
	core.render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix mat;
	Matrix world;

	prg->SetMatrix(Shader::Type::Vertex, "trans", &world, 1);
	prg->SetMatrix(Shader::Type::Vertex, "view_proj", &view_proj, 1);
	prg->SetVector(Shader::Type::Pixel, "color", (Vector4*)&color, 1);
	prg->SetTexture(Shader::Type::Pixel, "diffuseMap", texture);

	core.render.GetDevice()->Draw(Device::TrianglesList, 0, sz);
}

bool Terrain::Play()
{
	body.object = this;
	body.body = PScene()->CreateHeightmap(hwidth, hheight, Vector2(scaleh, scalev), (hgt_name + string("hm")).c_str(), 1);
	body.body->SetUserData(&body);

	return true;
}

void Terrain::Release()
{
	RELEASE(body.body);

	SceneObject::Release();
}