
#include "DecodedBuffer.h"
#include "Services/Core/Core.h"

bool DecodedBuffer::Load(const char* filename, bool load_all)
{
	char ext[16];
	StringUtils::GetExtension(filename, ext, 16);

	if (StringUtils::IsEqual(ext, "wav"))
	{
		return LoadWav(filename, load_all);
	}
	else
	if (StringUtils::IsEqual(ext, "ogg"))
	{
		return LoadOgg(filename, load_all);
	}

	return false;
}

bool DecodedBuffer::LoadWav(const char* filename, bool load_all)
{
	stream = core.files.FileOpen(filename, "rb");

	if (!stream) return false;

	WavHeaderType header;

	fread(&header, sizeof(WavHeaderType), 1, stream);

	if (header.chunk_id[0] != 'R' || header.chunk_id[1] != 'I' || header.chunk_id[2] != 'F' || header.chunk_id[3] != 'F')
	{
		return false;
	}

	if (header.format[0] != 'W' || header.format[1] != 'A' || header.format[2] != 'V' || header.format[3] != 'E')
	{
		return false;
	}

	if (header.sub_chunk_id[0] != 'f' || header.sub_chunk_id[1] != 'm' || header.sub_chunk_id[2] != 't' || header.sub_chunk_id[3] != ' ')
	{
		return false;
	}

	if (header.audio_format != 1)
	{
		return false;
	}

	if (header.data_chunk_id[0] != 'd' || header.data_chunk_id[1] != 'a' || header.data_chunk_id[2] != 't' || header.data_chunk_id[3] != 'a')
	{
		return false;
	}

	sample_rate = header.sample_rate;
	bits_per_sample = header.bits_per_sample;
	num_channels = header.num_channels;
	block_align = header.block_align;

	type = FileWav;

	if (load_all)
	{
		decoded_data_size = header.data_size;

		decoded_data = (uint8_t*)malloc(decoded_data_size);

		fread(decoded_data, decoded_data_size, 1, stream);

		fclose(stream);
	}

	return true;
}

bool DecodedBuffer::LoadOgg(const char* filename, bool load_all)
{
	stream = core.files.FileOpen(filename, "rb");

	if (!stream) return false;

	if ((ov_open(stream, &vorbis_file, nullptr, 0)) != 0)
	{
		fclose(stream);

		return false;
	}

	vorbis_info* vorbisInfo = ov_info(&vorbis_file, -1);

	sample_rate = vorbisInfo->rate;
	bits_per_sample = 16;
	num_channels = vorbisInfo->channels;
	block_align = 2 * num_channels;

	type = FileOgg;

	if (load_all)
	{
		std::vector<uint8_t*> chunks;

		constexpr int CHUNK_SIZE = 32 * 1024;

		int last_chuk_size = CHUNK_SIZE;

		bool finished = false;

		while (!finished)
		{
			uint8_t* chunk = (uint8_t*)malloc(CHUNK_SIZE);

			last_chuk_size = Decode(chunk, CHUNK_SIZE, false);

			if (last_chuk_size != CHUNK_SIZE)
			{
				finished = true;
			}

			chunks.push_back(chunk);
		}

		decoded_data_size = ((int)chunks.size() - 1) * CHUNK_SIZE + last_chuk_size;
		decoded_data = (uint8_t*)malloc(decoded_data_size);

		int offset = 0;

		for (int i = 0; i < chunks.size(); i++)
		{
			memcpy(&decoded_data[offset], chunks[i], i == chunks.size() - 1 ? last_chuk_size : CHUNK_SIZE);
			offset += CHUNK_SIZE;

			free(chunks[i]);
		}

		fclose(stream);

		ov_clear(&vorbis_file);
	}

	return true;
}

long DecodedBuffer::Decode(uint8_t* buffer, long length, bool looped)
{
	long read = 0;

	if (type == FileWav)
	{
		while (read < length)
		{
			read += (long)fread(&buffer[read], 1, length - read, stream);

			if (read != length)
			{
				if (looped)
				{
					fseek(stream, sizeof(WavHeaderType), SEEK_SET);
				}
				else
				{
					memset(buffer + read, 0, length - read);
					break;
				}
			}
		}
	}
	else
	if (type == FileOgg)
	{
		while (read < length)
		{
			int bit_stream = 0;
			long og_read = ov_read(&vorbis_file, (char*)buffer + read, length - read, 0, 2, 1, &bit_stream);

			read += og_read;

			if (og_read == 0)
			{
				if (looped)
				{
					ov_time_seek(&vorbis_file, 0);
				}
				else
				{
					memset(buffer + read, 0, length - read);
					break;
				}
			}
		}
	}

	return read;
}

void DecodedBuffer::Clear()
{
	if (stream)
	{
		if (type == FileOgg)
		{
			ov_clear(&vorbis_file);
		}

		fclose(stream);
	}
	else
	{
		if (decoded_data)
		{
			free(decoded_data);
		}
	}
}