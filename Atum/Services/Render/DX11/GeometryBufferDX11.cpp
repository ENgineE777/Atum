
#include "GeometryBufferDX11.h"
#include "DeviceDX11.h"

#include "d3d11.h"

GeometryBufferDX11::GeometryBufferDX11(int sz, int strd) : GeometryBuffer(sz, strd)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DeviceDX11::instance->pd3dDevice->CreateBuffer( &bd, NULL, &buffer );
}

void* GeometryBufferDX11::Lock()
{
	D3D11_MAPPED_SUBRESOURCE res;
	DeviceDX11::instance->immediateContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	return res.pData;
}

void GeometryBufferDX11::Unlock()
{
	DeviceDX11::instance->immediateContext->Unmap(buffer, 0);
}

void GeometryBufferDX11::StartDesc()
{	
}

void GeometryBufferDX11::AddDescElem(ElemUsage usage, ElemType type, const char* name)
{
}

void GeometryBufferDX11::EndDesc()
{
}

void* GeometryBufferDX11::GetData()
{
	return buffer;
}

void GeometryBufferDX11::Release()
{
	RELEASE(buffer)

	delete this;
}