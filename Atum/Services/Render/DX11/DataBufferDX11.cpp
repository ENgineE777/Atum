
#include "DataBufferDX11.h"
#include "DeviceDX11.h"

#include "d3d11.h"

DataBufferDX11::DataBufferDX11(int sz, int strd) : DataBuffer(sz, strd)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DeviceDX11::instance->pd3dDevice->CreateBuffer( &bd, NULL, &buffer );
}

void* DataBufferDX11::Lock()
{
	D3D11_MAPPED_SUBRESOURCE res;
	DeviceDX11::instance->immediateContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	return res.pData;
}

void DataBufferDX11::Unlock()
{
	DeviceDX11::instance->immediateContext->Unmap(buffer, 0);
}

void DataBufferDX11::Release()
{
	RELEASE(buffer)

	delete this;
}