
#pragma once

#include "SoundBase.h"

/**
\ingroup gr_code_services_sound
*/

/**
\brief SoundStream

Sound which streams sound file. Good for playing a music.

*/

class SoundStream : public SoundBase
{
	friend class Sounds;

	constexpr static int CHUNK_SIZE = 4 * 1024;

#ifdef PLATFORM_PC
	int buffer_index = 0;
#endif

#ifdef PLATFORM_ANDROID
	static void Callback(SLAndroidSimpleBufferQueueItf bq, void *context);
	uint8_t buffer[CHUNK_SIZE];
#endif

	void Fill();
	bool Load(const char* file_name) override;
	void Update() override;

public:

	bool Play(PlayType type) override;
};