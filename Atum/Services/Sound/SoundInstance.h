
#pragma once

#include "SoundBase.h"

/**
\ingroup gr_code_services_sound
*/

/**
\brief SoundInstance

Sound which loads entire sounf file into memory. Good for small SFX.

*/

class SoundInstance : public SoundBase
{
	friend class Sounds;

	bool Load(const char* file_name) override;
#ifdef PLATFORM_IOS
    void CreateSoundBuffer(long size) override;
#endif
	void Update() override;

#ifdef PLATFORM_ANDROID
	static void Callback(SLAndroidSimpleBufferQueueItf bq, void *context);
	void Fill();
#endif

public:

	bool Play(PlayType type) override;
};
