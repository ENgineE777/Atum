
#pragma once

#include "stdio.h"
#include <memory>

class Buffer
{
	uint8_t* buffer;
	uint8_t* ptr;
	int      size;

public:

	Buffer(const char* name)
	{
		buffer = NULL;
		size = 0;

		FILE* file = fopen(name, "rb");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);

			buffer = (uint8_t*)malloc(size);
			fread(buffer, size, 1, file);

			fclose(file);
		}

		ptr = buffer;
	}

	~Buffer()
	{
		if (buffer)
		{
			free(buffer);
		}
	}

	uint8_t* GetData()
	{
		return buffer;
	}

	int GetSize()
	{
		return size;
	}

	void Read(void* dst, int sz)
	{
		int k = size - (ptr - buffer) + sz;
		memcpy(dst, ptr, sz);
		ptr += sz;
	}

	void Skip(int sz)
	{
		ptr += sz;
	}
};