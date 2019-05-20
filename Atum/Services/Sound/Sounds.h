
#pragma once

#ifdef PLATFORM_PC
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#endif

#include "SoundInstance.h"
#include "SoundStream.h"

class Sounds
{
	friend class SoundBase;
	friend class SoundInstance;
	friend class SoundStream;
	float master_volume = 1.0f;

#ifdef PLATFORM_PC
	IDirectSound8* direct_sound = nullptr;
#endif

	std::vector<SoundBase*> sounds;

public:

	bool Init(void* data);

	SoundInstance* CreateSound(const char* file_name);
	SoundStream* CreateStream(const char* file_name);

	void Update(float dt);

	void SetMasterVolume(float volume);
	float GetMasterVolume();


	void Release();
};