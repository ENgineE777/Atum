
#include "FileInMemory.h"
#include "Files.h"

bool FileInMemory::Load(const char* name)
{
	Release();

	FILE* file = files.FileOpen(name, "rb");

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

FileInMemory::~FileInMemory()
{
	Release();
}

uint8_t* FileInMemory::GetData()
{
	return buffer;
}

int FileInMemory::GetSize()
{
	return size;
}

void* FileInMemory::GetPtr()
{
	return ptr;
}

void FileInMemory::Read(void* dest, int len)
{
	memcpy(dest, ptr, len);
	Skip(len);
}

void FileInMemory::Skip(int len)
{
	ptr += len;
}

void FileInMemory::Release()
{
	if (buffer)
	{
		free(buffer);
		buffer = nullptr;
		size = 0;
	}
}
