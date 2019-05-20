
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
		unsigned long chunk_size;
		char format[4];
		char sub_chunk_id[4];
		unsigned long sub_chunk_size;
		unsigned short audio_format;
		unsigned short num_channels;
		unsigned long sample_rate;
		unsigned long bytes_per_second;
		unsigned short block_align;
		unsigned short bits_per_sample;
		char data_chunk_id[4];
		unsigned long data_size;
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

	long Decode(uint8_t* buffer, long length, bool looped);

	void Clear();
};
