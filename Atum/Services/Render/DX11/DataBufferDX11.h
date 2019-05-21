
#pragma once

#include "Services/Render/DataBuffer.h"
#include <vector>

class DataBufferDX11 : public DataBuffer
{
public:

	struct ID3D11Buffer* buffer = nullptr;

	DataBufferDX11(int sz, int strd);

	virtual void* Lock();
	virtual void Unlock();
	virtual void Release();
};
