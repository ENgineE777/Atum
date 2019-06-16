
#pragma once

#ifdef PLATFORM_PC
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#endif

#ifdef PLATFORM_ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#endif

#include "SoundInstance.h"
#include "SoundStream.h"

/**
\ingroup gr_code_services_sound
*/

/**
\brief Sounds

This class manages sounds. Sounds can be entrielly loaded into memory or can be streamed.
First type is usefull for SFX. For music better to use streamed version. Wav and Ogg formats
are supported.

Example of playing SFX:

\code
auto sound_inst = CreateSound("sound01.wav");
test_inst->Play(SoundBase::Autodelete);
\endcode

*/

class Sounds
{
	friend class SoundBase;
	friend class SoundInstance;
	friend class SoundStream;
	float master_volume = 1.0f;

#ifdef PLATFORM_PC
	IDirectSound8* direct_sound = nullptr;
#endif

#ifdef PLATFORM_ANDROID
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
	SLObjectItf outputMixObject;
#endif

	std::vector<SoundBase*> sounds;

public:

	/**
	\brief Create SoundInstance

	\param[in] file_name Full path to a file

	\return Pointer to SoundInstance
	*/
	SoundInstance* CreateSound(const char* file_name);

	/**
	\brief Create SoundStream

	\param[in] file_name Full path to a file

	\return Pointer to SoundStream
	*/
	SoundStream* CreateStream(const char* file_name);

	/**
	\brief Set master volume

	\param[in] volume Value of a volume 
	*/
	void SetMasterVolume(float volume);

	/**
	\brief Get master volume

	\return Current master volume
	*/
	float GetMasterVolume();

#ifndef DOXYGEN_SKIP
	bool Init(void* data);

	void Update(float dt);

	void ClearAllSounds();

	void OnPause();
	void OnResume();
	void Release();
#endif
};