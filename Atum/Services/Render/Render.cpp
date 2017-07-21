
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

	if (device->Init(width, height, data))
	{
		device->SetCulling(Device::CullCCW);

		DebugPrograms::Init();

		spheres = new DebugSpheres();
		spheres->Init();

		boxes = new DebugBoxes();
		boxes->Init();

		lines = new DebugLines();
		lines->Init();

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

void Render::AddExecutedDelgateList(const char* list_name, int level)
{
	int index = -1;

	for (int i = 0; i<executedDelegateLists.size(); i++)
	{
		if (level<executedDelegateLists[i].level)
		{
			index = i;
			break;
		}
	}

	executedDelegateLists.push_back(ExecutedDelegateList());
	ExecutedDelegateList* list = &executedDelegateLists[executedDelegateLists.size() - 1];

	strcpy(list->name, list_name);
	list->level = level;

	if (index != -1)
	{
		for (int i = executedDelegateLists.size() - 1; i>index; i--)
		{
			ExecutedDelegateList tmp = executedDelegateLists[i];
			executedDelegateLists[i] = executedDelegateLists[i - 1];
			executedDelegateLists[i - 1] = tmp;
		}
	}
}

void Render::Execute(float dt)
{
	for (int i = 0; i<executedDelegateLists.size(); i++)
	{
		ExecuteDelgateList(executedDelegateLists[i].name, dt);
	}
}

Render::DelegateList* Render::FindExecuteList(const char* list_name)
{
	for (int i = 0; i <delegateList.size(); i++)
	{
		if (strcmp(delegateList[i].name, list_name) == 0)
		{
			return &delegateList[i];
		}

	}

	return NULL;
}

void Render::ExecuteDelgateList(const char* list_name, float dt)
{
	DelegateList* list = FindExecuteList(list_name);

	if (list)
	{
		for (int j = 0; j<list->list.size(); j++)
		{
			(list->list[j].entity->*list->list[j].call)(dt);
		}
	}
}

void Render::AddDelegate(const char* list_name, Object* entity, Object::Delegate call, int level)
{
	DelegateList* list = FindExecuteList(list_name);

	if (!list)
	{
		delegateList.push_back(DelegateList());
		list = &delegateList[delegateList.size() - 1];
		strcpy(list->name, list_name);
	}

	list->list.push_back(DelegateObject());
	DelegateObject* delegate = &list->list[list->list.size() - 1];

	delegate->entity = entity;
	delegate->call = call;
	delegate->level = level;

	int index = list->list.size() - 1;

	while (index>0 && list->list[index - 1].level>list->list[index].level)
	{
		DelegateObject tmp = list->list[index];
		list->list[index] = list->list[index - 1];
		list->list[index - 1] = tmp;

		index--;
	}
}

void Render::DelDelegate(const char* list_name, Object* entity)
{
	DelegateList* list = FindExecuteList(list_name);

	if (list)
	{
		for (int i = 0; i<list->list.size(); i++)
		{
			if (entity == list->list[i].entity)
			{
				list->list.erase(list->list.begin() + i);
				break;
			}
		}
	}
}

void Render::DelAllDelegates(Object* entity)
{
	for (int j = 0; j < delegateList.size(); j++)
	{
		DelegateList* list = &delegateList[j];

		for (int i = 0; i < list->list.size(); i++)
		{
			if (entity == list->list[i].entity)
			{
				list->list.erase(list->list.begin() + i);
				break;
			}
		}
	}
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