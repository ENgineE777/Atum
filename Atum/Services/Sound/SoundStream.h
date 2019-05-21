
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

	int buffer_index = 0;

	constexpr static int CHUNK_SIZE = 16 * 1024;

	void Fill();
	bool Load(const char* file_name) override;
	void Update() override;

public:

	bool Play(PlayType type) override;
};