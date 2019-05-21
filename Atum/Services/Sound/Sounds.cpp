#include "Services/Core/Core.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <algorithm>

bool Sounds::Init(void* data)
{
	HRESULT result;

	result = DirectSoundCreate8(nullptr, &direct_sound, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	
	result = direct_sound->SetCooperativeLevel(*((HWND*)data), DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

SoundInstance* Sounds::CreateSound(const char* file_name)
{
	auto* inst = new SoundInstance();
	inst->Load(file_name);

	sounds.push_back(inst);
	std::sort(sounds.begin(), sounds.end());

	return inst;
}

SoundStream* Sounds::CreateStream(const char* file_name)
{
	auto* stream = new SoundStream();
	stream->Load(file_name);

	sounds.push_back(stream);
	std::sort(sounds.begin(), sounds.end());

	return stream;
}

void Sounds::Update(float dt)
{
	for(int i = 0; i < sounds.size(); i++)
	{
		auto* sound = sounds[i];
		if (!sound->playing && sound->play_type == SoundBase::Autodelete)
		{
			sound->Release();
			i--;

			continue;
		}

		sound->Update();
	}
}

void Sounds::SetMasterVolume(float volume)
{
	master_volume = volume;
}

float Sounds::GetMasterVolume()
{
	return master_volume;
}

void Sounds::ClearAllSounds()
{
	for (int i = 0; i < sounds.size(); i++)
	{
		sounds[i]->Release();
		i--;
	}
}

void Sounds::Release()
{
}