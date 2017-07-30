
#include "Render.h"

#include "Render.h"
#include "DX11/DeviceDX11.h"
#include "Debug/Debug.h"
#include <memory>

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

		lines = new DebugLines();
		lines->Init(debugTaskPool);

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

	ptr += 2;

	uint8_t imageTypeCode = *((uint8_t*)ptr);

	if (imageTypeCode != 2 && imageTypeCode != 3)
	{
		return NULL;
	}

	ptr += 10;

	short int imageWidth = *((short int*)ptr);
	ptr += 2;

	short int imageHeight = *((short int*)ptr);
	ptr += 2;

	uint8_t bitCount = *((uint8_t*)ptr);
	ptr += 2;

	int colorMode = bitCount / 8;

	Texture* texture = device->CreateTexture(imageWidth, imageHeight, Texture::FMT_A8R8G8B8, 0, false, Texture::Tex2D);

	int pitch;
	int pitchSlice;

	uint8_t* dest = (uint8_t*)malloc(imageWidth * imageHeight * 4);

	for (int j = 0; j < imageHeight; j++)
	{
		if (colorMode == 4)
		{
			for (int i = 0; i < imageWidth; i++)
			{
				uint8_t* dst = &dest[((imageHeight - 1 - j) * imageWidth + i) * 4];
				memcpy(dst, &ptr[(j * imageWidth + i) * 4], 4);
				uint8_t tmp = dst[0];
				dst[0] = dst[2];
				dst[2] = tmp;
			}
		}
		else
		if (colorMode == 3)
		{
			for (int i = 0; i < imageWidth; i++)
			{
				uint8_t* dst = &dest[((imageHeight - 1 - j) * imageWidth + i) * 4];
				memcpy(dst, &ptr[(j * imageWidth + i) * 3], 3);
				uint8_t tmp = dst[0];
				dst[0] = dst[2];
				dst[2] = tmp;

				dst[3] = 255;
			}
		}
	}

	texture->Update(0, 0, dest, imageWidth * 4);

	free(dest);

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

void Render::DebugSphere(Vector& pos, Color& color, float radius)
{
	spheres->AddSphere(pos, color, radius);
}

void Render::DebugBox(Matrix& pos, Color& color, Vector& scale)
{
	boxes->AddBox(pos, color, scale);
}

void Render::Release()
{
	device->Release();
}