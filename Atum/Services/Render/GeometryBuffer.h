
#pragma once

#include "Support/Support.h"

class GeometryBuffer
{
protected:

	int size;
	int stride;

public:

	GeometryBuffer(int count, int strd) { size = count * strd; stride = strd; };
	virtual ~GeometryBuffer() {};

	virtual void* Lock() = 0;
	virtual void Unlock() = 0;
	int GetSize() { return size; };
	int GetStride() { return stride; };
	virtual void Release() = 0;
};
