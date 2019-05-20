
#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "DecodedBuffer.h"

class SoundBase
{
	friend class Sounds;
	friend class SoundInstance;
	friend class SoundStream;

public:

	enum PlayType
	{
		Once = 0,
		Looped,
		Autodelete
	};

	void CreateSoundBuffer(long size);
	virtual bool Load(const char* file_name) = 0;
	virtual bool Play(PlayType type) = 0;
	void SetVolume(float volume);
	float GetVolume();
	void Stop();
	virtual void Update();
	virtual void Release();

private:

	LPDIRECTSOUNDBUFFER8 sound_buffer = nullptr;
	DecodedBuffer decoded_buffer;

	LPDIRECTSOUNDNOTIFY8 play_marker;
	vector<HANDLE> events;

	float volume = 1.0f;
	bool playing = false;

	bool Play(PlayType type, bool buffer_looped);

	PlayType play_type;
};