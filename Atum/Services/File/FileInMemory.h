
#pragma once

#include <stdint.h>
#include <string.h>

class FileInMemory
{
	uint8_t* buffer = nullptr;
	int      size = 0;
	uint8_t* ptr;

public:

	FileInMemory() = default;
	~FileInMemory();

	bool Load(const char* name);
	
	uint8_t* GetData();
	int GetSize();
	void* GetPtr();

	void Read(void* dest, int len);
	void Skip(int len);

protected:
	void Release();
};
