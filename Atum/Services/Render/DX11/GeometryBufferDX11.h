
#pragma once

#include "Services/Render/GeometryBuffer.h"
#include <vector>

/**
\ingroup gr_code_services_render_dx11
*/

class GeometryBufferDX11 : public GeometryBuffer
{
public:

	struct ID3D11Buffer* buffer = nullptr;

	GeometryBufferDX11(int sz, int strd);

	virtual void* Lock();
	virtual void Unlock();
	virtual void Release();
};
