
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
	void Update() override;

public:

	bool Play(PlayType type) override;
};