
#pragma once

#include "Services/Render/GeometryBuffer.h"
#include <vector>

class GeometryBufferDX11 : public GeometryBuffer
{
public:

	struct ID3D11Buffer* buffer = nullptr;

	GeometryBufferDX11(int sz, int strd);

	virtual void* Lock();
	virtual void Unlock();
	virtual void Release();
};
