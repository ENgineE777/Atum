
#pragma once

#include "SoundBase.h"

class SoundStream : public SoundBase
{
	int buffer_index = 0;

	constexpr static int CHUNK_SIZE = 16 * 1024;

	void Fill();

public:

	bool Load(const char* file_name) override;
	bool Play(PlayType type) override;
	void Update() override;
};