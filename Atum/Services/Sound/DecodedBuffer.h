
#pragma once

#include "Services/File/FileInMemory.h"

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

class DecodedBuffer
{
	enum SourceType
	{
		FileNone = 0,
		FileWav,
		FileOgg
	};

	struct WavHeaderType
	{
		char chunk_id[4];
		uint32_t chunk_size;
		char format[4];
		char sub_chunk_id[4];
		uint32_t sub_chunk_size;
		uint16_t audio_format;
		uint16_t num_channels;
		uint32_t sample_rate;
		uint32_t bytes_per_second;
		uint16_t block_align;
		uint16_t bits_per_sample;
		char data_chunk_id[4];
		uint32_t data_size;
	};

	SourceType type;
	FILE* stream = nullptr;

	OggVorbis_File vorbis_file;

public:

	unsigned long sample_rate = 0;
	unsigned short bits_per_sample = 0;
	unsigned short num_channels = 0;
	unsigned short block_align = 0;

	uint8_t* decoded_data = nullptr;
	unsigned long decoded_data_size = 0;

	bool Load(const char* filename, bool load_all);

	bool LoadWav(const char* filename, bool load_all);
	bool LoadOgg(const char* filename, bool load_all);

	void RestartDecode();
	long Decode(uint8_t* buffer, long length, bool looped);

	void Clear();
};
