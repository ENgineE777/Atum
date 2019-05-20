
#pragma once

#include "SoundBase.h"

class SoundInstance : public SoundBase
{
public:

	bool Load(const char* file_name) override;
	bool Play(PlayType type) override;
	void Update() override;
};