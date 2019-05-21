
#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "DecodedBuffer.h"

/**
\ingroup gr_code_services_sound
*/

/**
\brief SoundBase

Base class for SoundStream and SoundInstance.

*/

class SoundBase
{
	friend class Sounds;
	friend class SoundInstance;
	friend class SoundStream;

public:

	enum PlayType
	{
		Once = 0 /*!< Sound should be played only once */,
		Looped /*!< Sound should be played in loop mode */,
		Autodelete /*!< Play sound once and thet auto delete insctance */,
	};

	/**
	\brief Start to play a sound

	\param[in] type Type of playing
	*/
	virtual bool Play(PlayType type) = 0;

	/**
	\brief Set a volume

	\param[in] volume Value of a volume
	*/
	void SetVolume(float volume);

	/**
	\brief Get master volume

	\return Current master volume
	*/
	float GetVolume();

	/**
	\brief Stop playing
	*/
	void Stop();

	/**
	\brief SoundBase should released only via this method
	*/
	virtual void Release();

private:

	LPDIRECTSOUNDBUFFER8 sound_buffer = nullptr;
	DecodedBuffer decoded_buffer;

	LPDIRECTSOUNDNOTIFY8 play_marker;
	vector<HANDLE> events;

	float volume = 1.0f;
	bool playing = false;

	void CreateSoundBuffer(long size);
	virtual bool Load(const char* file_name) = 0;
	bool Play(PlayType type, bool buffer_looped);
	virtual void Update();

	PlayType play_type;
};