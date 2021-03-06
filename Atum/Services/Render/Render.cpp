
#include "Render.h"
#include "Services/File/FileInMemory.h"

#include "Debug/Debug.h"
#include <memory>

#ifdef PLATFORM_PC
#include "DX11/DeviceDX11.h"
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#include "Services/Render/GLES3/DeviceGLES.h"
#endif

#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#define STB_IMAGE_IMPLEMENTATION
#include "Support/stb/stb_image.h"

#include "SceneObjects/2D/Sprite.h"

Render::Render()
{
	device = nullptr;
	need_calc_trans = false;
}

bool Render::Init(const char* device_name, void* external_device)
{
#ifdef PLATFORM_PC
	device = new DeviceDX11();
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	device = new DeviceGLES();
#endif

	groupTaskPool = core.taskExecutor.CreateGroupTaskPool();
	debugTaskPool = groupTaskPool->AddTaskPool();

	if (!device->Init(external_device))
	{
		return false;
	}

	device->SetCulling(Device::CullCCW);
	device->SetBlendFunc(Device::ArgSrcAlpha, Device::ArgInvSrcAlpha);
	device->SetDepthTest(true);

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

	triangles2D = new DebugTriangles2D();
	triangles2D->Init(debugTaskPool);

	font = new DebugFont();
	font->Init(debugTaskPool);

	white_tex = device->CreateTexture(2, 2, Texture::FMT_A8R8G8B8, 0, false, Texture::Tex2D);

	uint8_t white_tex_data[4 * 4];
	memset(white_tex_data, 255, 16);

	white_tex->Update(0, 0, white_tex_data, 2 * 4);

	return true;
}

Device* Render::GetDevice()
{
	return device;
}

void Render::SetTransform(Transform _trans, Matrix mat)
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

Program* Render::GetProgram(const char* name)
{
	if (programs.count(name) > 0)
	{
		return programs[name];
	}

	auto decls = ClassFactoryProgram::Decls();
	Program * prg = ClassFactoryProgram::Create(name);
	prg->Init();
	device->PrepareProgram(prg);

	programs[name] = prg;

	return prg;
}

Texture* Render::LoadTexture(const char* name)
{
	if (textures.count(name) > 0)
	{
		TextureRef& ref = textures[name];

		ref.count++;
		return ref.texture;
	}

	FileInMemory buffer;

	if (!buffer.Load(name))
	{
		return nullptr;
	}

	uint8_t* ptr = buffer.GetData();

	int bytes;
	int width;
	int height;
	uint8_t* data = stbi_load_from_memory(ptr, buffer.GetSize(), &width, &height, &bytes, STBI_rgb_alpha);

	Texture* texture = device->CreateTexture(width, height, Texture::FMT_A8R8G8B8, 0, false, Texture::Tex2D);
	texture->name = name;

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

void Render::DebugLine(Vector3 from, Color from_clr, Vector3 to, Color to_clr, bool use_depth)
{
	lines->AddLine(from, from_clr, to, to_clr, use_depth);
}

void Render::DebugLine2D(Vector2 from, Color from_clr, Vector2 to, Color to_clr)
{
	lines->AddLine2D(from, from_clr, to, to_clr);
}

void Render::DebugRect2D(Vector2 from, Vector2 to, Color color)
{
	lines->AddLine2D(Vector2(from.x, from.y), color, Vector2(to.x  , from.y), color);
	lines->AddLine2D(Vector2(to.x  , from.y), color, Vector2(to.x  , to.y  ), color);
	lines->AddLine2D(Vector2(to.x  , to.y  ), color, Vector2(from.x, to.y  ), color);
	lines->AddLine2D(Vector2(from.x, to.y  ), color, Vector2(from.x, from.y), color);
}

void Render::DebugSphere(Vector3 pos, Color color, float radius, bool full_shade)
{
	if (full_shade)
	{
		spheres->AddSphere(pos, color, radius);
	}
	else
	{
		lines->DrawCircle(0, pos, color, radius);
		lines->DrawCircle(1, pos, color, radius);
		lines->DrawCircle(2, pos, color, radius);
	}
}

void Render::DebugBox(Matrix pos, Color color, Vector3 scale)
{
	boxes->AddBox(pos, color, scale);
}

void Render::DebugTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color)
{
	triangles->AddTriangle(p1, p2, p3, color);
}

void Render::DebugTriangle2D(Vector2 p1, Vector2 p2, Vector2 p3, Color color)
{
	triangles2D->AddTriangle(p1, p2, p3, color);
}

void Render::DebugPrintText(Vector2 pos, ScreenCorner corner, Color color, const char* text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	va_end(args);

	font->AddText(pos, corner, color, buffer);
}

void Render::DebugPrintText(Vector2 pos, ScreenCorner corner, bool abs_unit, Color color, const char* text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	va_end(args);

	font->AddText(Sprite::MoveToCamera(pos), corner, color, buffer);
}

void Render::DebugPrintText(Vector3 pos, float dist, Color color, const char* text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	perror(buffer);
	va_end(args);

	font->AddText(pos, dist, color, buffer);
}

void Render::DebugSprite(Texture* texture, Vector2 pos, Vector2 size, Color color, Vector2 offset, float angle)
{
	sprites->AddSprite(texture, pos, size, offset, angle, color);
}

Vector3 Render::TransformToScreen(Vector3 pos, int type)
{
	Matrix view;
	core.render.GetTransform(Render::View, view);

	Matrix view_proj;
	core.render.GetTransform(Render::WrldViewProj, view_proj);

	Vector3 pre_ps = pos * view;
	Vector4 ps2 = view_proj.MulVertex4(pos);
	Vector3 ps;
	ps.Set(ps2.x / ps2.w, ps2.y / ps2.w, ps2.z);

	if (pre_ps.z < 0.0f)
	{
		ps = -ps;
	}

	if (type == 0)
	{
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
			ps.x *= core.render.GetDevice()->GetWidth();
			ps.y *= core.render.GetDevice()->GetHeight();
		}
	}

	return ps;
}

void Render::Release()
{
	device->Release();
}
