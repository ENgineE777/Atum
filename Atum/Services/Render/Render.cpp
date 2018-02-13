
#include "Render.h"

#include "Render.h"
#include "DX11/DeviceDX11.h"
#include "Debug/Debug.h"
#include <memory>

//#define STBI_NO_JPEG

//STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM


#define STB_IMAGE_IMPLEMENTATION
#include "Support/stb/stb_image.h"

Render render;

Render::Render()
{
	device = NULL;
	need_calc_trans = false;
}

bool Render::Init(const char* device_name, int width, int height, void* data)
{
	device = new DeviceDX11();

	groupTaskPool = taskExecutor.CreateGroupTaskPool();
	debugTaskPool = groupTaskPool->AddTaskPool();

	taskExecutor.SetTaskPoolExecutionLevel(groupTaskPool, 100);

	if (device->Init(width, height, data))
	{
		device->SetCulling(Device::CullCCW);

		DebugPrograms::Init();

		spheres = new DebugSpheres();
		spheres->Init(debugTaskPool);

		boxes = new DebugBoxes();
		boxes->Init(debugTaskPool);

		triangles = new DebugTriangles();
		triangles->Init(debugTaskPool);

		lines = new DebugLines();
		lines->Init(debugTaskPool);

		sprites = new DebugSprites();
		sprites->Init(debugTaskPool);

		font = new DebugFont();
		font->Init(debugTaskPool);

		return true;
	}

	return false;
}

Device* Render::GetDevice()
{
	return device;
}

void Render::SetTransform(Transform _trans, Matrix& mat)
{
	if (_trans != WrldViewProj)
	{
		trans[_trans] = mat;
		need_calc_trans = true;
	}
}

void Render::GetTransform(Transform _trans, Matrix& mat)
{
	if (_trans == WrldViewProj && need_calc_trans)
	{
		CalcTrans();
	}

	mat = trans[_trans];
}

void Render::CalcTrans()
{
	trans[3] = trans[0] * trans[1] * trans[2];
	need_calc_trans = false;
}

Texture* Render::LoadTexture(const char* name)
{
	if (textures.count(name) > 0)
	{
		TextureRef& ref = textures[name];

		ref.count++;
		return ref.texture;
	}

	Buffer buffer(name);

	uint8_t* ptr = buffer.GetData();

	if (!ptr)
	{
		return NULL;
	}

	int bytes;
	int width;
	int height;
	uint8_t* data = stbi_load_from_memory(ptr, buffer.GetSize(), &width, &height, &bytes, STBI_rgb_alpha);


	Texture* texture = device->CreateTexture(width, height, Texture::FMT_A8R8G8B8, 0, false, Texture::Tex2D);

	texture->Update(0, 0, data, width * 4);

	free(data);

	texture->GenerateMips();

	TextureRef& ref = textures[name];

	ref.count = 1;
	ref.texture = texture;

	return texture;
}

bool Render::TexRefIsEmpty(Texture* texture)
{
	typedef std::map<std::string, TextureRef>::iterator it_type;

	for (it_type iterator = textures.begin(); iterator != textures.end(); iterator++)
	{
		if (iterator->second.texture == texture)
		{
			iterator->second.count--;

			if (iterator->second.count == 0)
			{
				textures.erase(iterator);
				return true;
			}

			return false;
		}
	}

	return true;
}

void Render::AddExecutedLevelPool(int level)
{
	groupTaskPool->AddFilter(level);
}

void Render::ExecutePool(int level, float dt)
{
	groupTaskPool->ExecutePool(level, dt);
}

TaskExecutor::SingleTaskPool* Render::AddTaskPool()
{
	return groupTaskPool->AddTaskPool();
}

void Render::DelTaskPool(TaskExecutor::SingleTaskPool* pool)
{
	groupTaskPool->DelTaskPool(pool);
}

void Render::Execute(float dt)
{
	groupTaskPool->Execute(dt);
}

void Render::DebugLine(Vector& from, Color& from_clr, Vector& to, Color& to_clr, bool use_depth)
{
	lines->AddLine(from, from_clr, to, to_clr, use_depth);
}

void Render::DebugLine2D(Vector2& from, Color& from_clr, Vector2& to, Color& to_clr)
{
	lines->AddLine2D(from, from_clr, to, to_clr);
}

void Render::DebugSphere(Vector& pos, Color& color, float radius)
{
	spheres->AddSphere(pos, color, radius);
}

void Render::DebugBox(Matrix& pos, Color& color, Vector& scale)
{
	boxes->AddBox(pos, color, scale);
}

void Render::DebugTriangle(Vector& p1, Vector& p2, Vector& p3, Color& color)
{
	triangles->AddTriangle(p1, p2, p3, color);
}

void Render::DebugPrintText(Vector2 pos, Color color, const char* text)
{
	font->AddText(pos, color, text);
}

void Render::DebugPrintText(Vector pos, float dist, Color color, const char* text)
{
	font->AddText(pos, dist, color, text);
}

void Render::DebugSprite(Texture* texture, Vector2& pos, Vector2& size)
{
	sprites->AddSprite(texture, pos, size);
}

Vector Render::TransformToScreen(const Vector& pos, int type)
{
	Matrix view;
	render.GetTransform(Render::View, view);

	Matrix view_proj;
	render.GetTransform(Render::WrldViewProj, view_proj);

	Vector pre_ps = pos * view;
	Vector4 ps2 = view_proj.MulVertex4(pos);
	Vector ps;
	ps.Set(ps2.x / ps2.w, ps2.y / ps2.w, ps2.z);

	if (type == 0)
	{
		ps.x = ps.x;
		ps.y = ps.y;
		ps.z = pre_ps.z;
	}
	else
	if (type == 1 || type == 2)
	{
		ps.x = 0.5f + ps.x*0.5f;
		ps.y = 0.5f - ps.y*0.5f;
		ps.z = pre_ps.z;

		if (type == 2)
		{
			ps.x *= render.GetDevice()->GetWidth();
			ps.y *= render.GetDevice()->GetHeight();
		}
	}

	return ps;
}

void Render::Release()
{
	device->Release();
}