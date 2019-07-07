#include "Services/Core/Core.h"

#ifdef PLATFORM_PC
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#endif

#include <algorithm>

bool Sounds::Init(void* data)
{
#ifdef PLATFORM_PC
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
#endif

#ifdef PLATFORM_ANDROID
	SLresult result;

	result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
#endif
    
	return true;
}

SoundInstance* Sounds::CreateSound(const char* file_name)
{
	auto* inst = new SoundInstance();
	if (!inst->Load(file_name))
	{
		core.Log("Sounds", "Error in loading sound instance %s", file_name);
		delete inst;
		return false;
	}

	sounds.push_back(inst);
	std::sort(sounds.begin(), sounds.end());

	return inst;
}

SoundStream* Sounds::CreateStream(const char* file_name)
{
	auto* stream = new SoundStream();
	if (!stream->Load(file_name))
	{
		core.Log("Sounds", "Error in loading sound stream %s", file_name);
		delete stream;
		return false;
	}

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

void Sounds::OnPause()
{
	for (auto* snd : sounds)
	{
		if (snd->playing)
		{
			snd->Pause(true);
		}
	}
}

void Sounds::OnResume()
{
	for (auto* snd : sounds)
	{
		if (snd->playing)
		{
			snd->Pause(false);
		}
	}
}

void Sounds::Release()
{
	ClearAllSounds();

#ifdef PLATFORM_PC
	RELEASE(direct_sound)
#endif

#ifdef PLATFORM_ANDROID
	if (outputMixObject != nullptr)
	{
		(*outputMixObject)->Destroy(outputMixObject);
	}

	if (engineObject != nullptr)
	{
		(*engineObject)->Destroy(engineObject);
	}
#endif
}
