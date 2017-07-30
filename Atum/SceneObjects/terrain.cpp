
#include "Terrain.h"
#include "programs.h"

#define GETHIGHT(i,j) hmap[((j) * hwidth + i)] * hscale.y

void Terrain::Init(Vector2 scale, const char* tex_name, const char* hgt_name)
{
	int stride = sizeof(VertexTri);

	LoadHMap(hgt_name);

	hscale = scale;
	sz = (hwidth) * (hheight) * 2;

	buffer = render.GetDevice()->CreateBuffer(sz * 3, stride);

	VertexTri* v_tri = (VertexTri*)buffer->Lock();

	float start_x = -hwidth * 0.5f * hscale.x;
	float start_z = -hheight * 0.5f * hscale.x;

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
					v_tri[1].position = Vector(i, GETHIGHT(i, j), - j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = Vector(i + 1, GETHIGHT(i+1,j), - j);
					v_tri[0].texCoord = Vector2(1.0f, 0.0f);
					v_tri[2].position = Vector( i + 1, GETHIGHT(i+1, j+1), - j - 1);
					v_tri[2].texCoord = Vector2(1.0f, 1.0f);

					v_tri[4].position = Vector( i, GETHIGHT(i, j), - j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = Vector( i + 1, GETHIGHT(i + 1, j + 1), - j - 1);
					v_tri[3].texCoord = Vector2(1.0f, 1.0f);
					v_tri[5].position = Vector(i, GETHIGHT(i, j + 1), - j - 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
				else
				{
					v_tri[1].position = Vector(i + 1, GETHIGHT(i+1,j), - j);
					v_tri[1].texCoord = Vector2(1.0f, 0.0f);
					v_tri[0].position = Vector(i + 1, GETHIGHT(i+1,j+1), - j - 1);
					v_tri[0].texCoord = Vector2(1.0f, 1.0f);
					v_tri[2].position = Vector(i, GETHIGHT(i, j+1), - j - 1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = Vector(i, GETHIGHT(i, j), - j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = Vector(i + 1, GETHIGHT(i+1, j), - j);
					v_tri[3].texCoord = Vector2(1.0f, 0.0f);
					v_tri[5].position = Vector(i, GETHIGHT(i, j + 1), - j - 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
			}
			else
			{
				if (i % 2 == 0)
				{
					v_tri[1].position = Vector(i, GETHIGHT(i, j), - j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = Vector(i + 1, GETHIGHT(i + 1, j), - j);
					v_tri[0].texCoord = Vector2(1.0f, 0.0f);
					v_tri[2].position = Vector(i, GETHIGHT(i, j + 1), - j - 1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = Vector(i + 1, GETHIGHT(i + 1, j), - j);
					v_tri[4].texCoord = Vector2(1.0f, 0.0f);
					v_tri[3].position = Vector(i + 1, GETHIGHT(i + 1, j + 1), - j - 1);
					v_tri[3].texCoord = Vector2(1.0f, 1.0f);
					v_tri[5].position = Vector(i, GETHIGHT(i, j + 1), - j - 1);
					v_tri[5].texCoord = Vector2(0.0f, 1.0f);
				}
				else
				{
					v_tri[1].position = Vector(i, GETHIGHT(i, j), - j);
					v_tri[1].texCoord = Vector2(0.0f, 0.0f);
					v_tri[0].position = Vector(i + 1, GETHIGHT(i + 1, j + 1), -j - 1);
					v_tri[0].texCoord = Vector2(1.0f, 1.0f);
					v_tri[2].position = Vector(i, GETHIGHT(i, j + 1), -j - 1);
					v_tri[2].texCoord = Vector2(0.0f, 1.0f);

					v_tri[4].position = Vector(i, GETHIGHT(i, j), - j);
					v_tri[4].texCoord = Vector2(0.0f, 0.0f);
					v_tri[3].position = Vector(i + 1, GETHIGHT(i + 1, j), - j);
					v_tri[3].texCoord = Vector2(1.0f, 0.0f);
					v_tri[5].position = Vector(i + 1, GETHIGHT(i + 1, j + 1), - j - 1);
					v_tri[5].texCoord = Vector2(1.0f, 1.0f);
				}
			}

			float cur_du = du * i;
			float cur_dv = dv * j;

			for (int k = 0; k < 6; k++)
			{
				v_tri[k].position.x = start_x + v_tri[k].position.x * hscale.x;
				v_tri[k].position.z = start_z - v_tri[k].position.z * hscale.x;
				v_tri[k].texCoord = Vector2(cur_du + v_tri[k].texCoord.x * du, cur_dv + v_tri[k].texCoord.y * dv);
			}

			for (int k = 0; k < 2; k++)
			{
				Vector v1 = v_tri[k * 3 + 1].position - v_tri[k * 3 + 0].position;
				Vector v2 = v_tri[k * 3 + 2].position - v_tri[k * 3 + 0].position;
				
				v1.Normalize();
				v2.Normalize();

				Vector normal;
				
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

	texture = render.LoadTexture(tex_name);

	//render.AddDelegate("geometry", this, (Object::Delegate)&Terrain::Render, 0);
	//render.AddDelegate("shgeometry", this, (Object::Delegate)&Terrain::ShRender, 0);
}

void Terrain::LoadHMap(const char* hgt_name)
{
	Buffer hbuffer(hgt_name);

	uint8_t* ptr = hbuffer.GetData();

	if (!ptr)
	{
		return;
	}

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

	int colorMode = bitCount / 8;

	hwidth = imageWidth;
	hheight = imageHeight;

	hmap = (uint8_t*)malloc(hwidth * hheight);

	for (int i = 0; i < hheight; i++)
	{
		for (int j = 0; j < hwidth; j++)
		{
			hmap[i * hheight + j] = ptr[((j)* imageWidth + i) * colorMode];
		}
	}
}

float Terrain::GetHight(float x, float z)
{
	float start_x = -hwidth * 0.5f * hscale.x;
	float start_z =  hheight * 0.5f * hscale.x;

	int i = ((float)x - start_x) / (float)(hscale.x);
	int j = (start_z - (float)z) / (float)(hscale.x);

	return hmap[((j)* hwidth + i)] * hscale.y;
}

void Terrain::Render(float dt)
{
	Render(Programs::prg);
}

void Terrain::ShRender(float dt)
{
	Render(Programs::shprg);
}

void Terrain::Render(Program* prg)
{
	render.GetDevice()->SetVertexBuffer(0, buffer);

	prg->Apply();

	Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);

	render.SetTransform(Render::World, Matrix());

	Matrix view_proj;
	render.GetTransform(Render::WrldViewProj, view_proj);

	Matrix mat;
	Matrix world;

	prg->VS_SetMatrix("trans", &world, 1);
	prg->VS_SetMatrix("view_proj", &view_proj, 1);
	prg->PS_SetVector("color", &color, 1);
	prg->PS_SetTexture("diffuseMap", texture);

	render.GetDevice()->Draw(Device::TrianglesList, 0, sz);
}