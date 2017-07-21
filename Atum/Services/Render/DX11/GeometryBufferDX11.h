
#pragma once

#include "Services/Render/GeometryBuffer.h"
#include <vector>
#include "d3d11.h"

class GeometryBufferDX11 : public GeometryBuffer
{
	class ID3D11Buffer* buffer;

	class ID3D11InputLayout*  layout;

public:
	
	GeometryBufferDX11(int sz, int strd);
	
	virtual void StartDesc();
	virtual void AddDescElem(ElemUsage usage, ElemType type, const char* name);
	virtual void EndDesc();

	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};
