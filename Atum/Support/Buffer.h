
#pragma once

#include "stdio.h"
#include <memory>

#include <sys/stat.h>

#ifdef PLATFORM_ANDROID
#include "Platform/Android/android_fopen.h"
#endif

class Buffer
{
	uint8_t* buffer = nullptr;
	int      size = 0;
	uint8_t* ptr;

public:

	static bool IsFileExist(const char*  name)
	{
		struct stat buffer;
		return (stat(name, &buffer) == 0);
	}

	Buffer()
	{
	};

	bool Load(const char* name)
	{
		Release();

		FILE* file = fopen(name, "rb");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);

			buffer = (uint8_t*)malloc(size + 1);
			fread(buffer, size, 1, file);

			buffer[size] = 0;

			fclose(file);

			ptr = buffer;

			return true;
		}

		return false;
	}

	~Buffer()
	{
		Release();
	}

	uint8_t* GetData()
	{
		return buffer;
	}

	int GetSize()
	{
		return size;
	}

	void* GetPtr()
	{
		return ptr;
	}

	void Read(void* dest, int len)
	{
		memcpy(dest, ptr, len);
		Skip(len);
	}

	inline void Skip(int len)
	{
		ptr += len;
	}

	void Release()
	{
		if (buffer)
		{
			free(buffer);
			buffer = nullptr;
			size = 0;
		}
	}
};