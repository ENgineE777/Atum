
#pragma once

#include "DeviceDX11.h"
#include "GeometryBufferDX11.h"
#include "ShaderDX11.h"
#include "TextureDX11.h"

DeviceDX11* DeviceDX11::instance = NULL;

DeviceDX11::DeviceDX11()
{
	pd3dDevice = NULL;
	immediateContext = NULL;
	swapChain = NULL;
	renderTargetView = NULL;
	depthStencil = NULL;
	depthStencilView = NULL;
	factory = NULL;

	instance = this;

	blend_desc = new D3D11_BLEND_DESC();
	ZeroMemory(blend_desc, sizeof(D3D11_BLEND_DESC));
	blend_state = NULL;
	blend_changed = true;

	ds_desc = new D3D11_DEPTH_STENCIL_DESC();
	ZeroMemory(ds_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	ds_state = NULL;
	ds_changed = true;

	raster_desc = new D3D11_RASTERIZER_DESC();
	ZeroMemory(raster_desc, sizeof(D3D11_RASTERIZER_DESC));
	raster_state = NULL;
	raster_changed = true;

	blend_desc->RenderTarget[0].BlendEnable = false;

	blend_desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc->RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	ds_desc->DepthEnable = true;
	ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc->DepthFunc = D3D11_COMPARISON_LESS;

	ds_desc->StencilEnable = false;
	ds_desc->StencilReadMask = 0xFF;
	ds_desc->StencilWriteMask = 0xFF;

	ds_desc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ds_desc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ds_desc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ds_desc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	raster_desc->AntialiasedLineEnable = false;
	raster_desc->CullMode = D3D11_CULL_BACK;
	raster_desc->DepthBias = 0;
	raster_desc->DepthBiasClamp = 0.0f;
	raster_desc->DepthClipEnable = false;

	raster_desc->FillMode = D3D11_FILL_SOLID;
	raster_desc->FrontCounterClockwise = false;
	raster_desc->MultisampleEnable = false;
	raster_desc->ScissorEnable = false;
	raster_desc->SlopeScaledDepthBias = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		cur_rt[i] = NULL;
	}

	cur_depth = NULL;
	need_set_rt = true;

	vp_was_setted = false;
}

bool DeviceDX11::Init(int width, int height, void* data)
{
	hwnd = *((HWND*)data);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE   driverType;
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &pd3dDevice, &featureLevel, &immediateContext);

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));

	SetVideoMode(width, height);

	return true;
}

void DeviceDX11::SetVideoMode(int wgt, int hgt)
{
	scr_w = wgt;
	scr_h = hgt;
	
	RELEASE(depthStencilView)
	RELEASE(depthStencil)
	RELEASE(renderTargetView)
	RELEASE(swapChain)

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = wgt;
	sd.BufferDesc.Height = hgt;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	factory->CreateSwapChain(pd3dDevice, &sd, &swapChain);

	//if( FAILED( hr ) )
	//return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//if( FAILED( hr ) )
	//return hr;

	hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
	{
		return;
	}

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = wgt;
	descDepth.Height = hgt;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = pd3dDevice->CreateTexture2D(&descDepth, NULL, &depthStencil);

	if (FAILED(hr))
	{
		return;
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pd3dDevice->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);

	RestoreRenderTarget();
}

void DeviceDX11::Clear(bool renderTarget, Color color, bool zbuffer, float zValue)
{
	if (renderTarget)
	{
		for (int i = 0; i < 6; i++)
		{
			if (cur_rt[i])
			{
				immediateContext->ClearRenderTargetView(cur_rt[i], (float*)&color.r);
			}
		}
	}

	if (zbuffer && cur_depth)
	{
		immediateContext->ClearDepthStencilView(cur_depth, D3D11_CLEAR_DEPTH, zValue, 0);
	}
}

void DeviceDX11::Present()
{
	swapChain->Present(0, 0);
}

GeometryBuffer* DeviceDX11::CreateBuffer(int count, int stride)
{
	return new GeometryBufferDX11(count, stride);
}

void DeviceDX11::SetVertexBuffer(int slot, GeometryBuffer* buffer)
{
	ID3D11Buffer* vb = NULL;
	unsigned int stride = 0;

	if (buffer)
	{
		vb = (ID3D11Buffer*)buffer->GetData();
		stride = buffer->GetStride();
	}

	unsigned int offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
}

void DeviceDX11::SetIndexBuffer(GeometryBuffer* buffer)
{
	ID3D11Buffer* ib = NULL;
	DXGI_FORMAT fmt = DXGI_FORMAT_R16_UINT;

	if (buffer)
	{
		ib = (ID3D11Buffer*)buffer->GetData();
		if (buffer->GetStride() == 4)
		{
			fmt = DXGI_FORMAT_R32_UINT;
		}
	}

	immediateContext->IASetIndexBuffer(ib, fmt, 0);
}

Shader* DeviceDX11::CreateShader(Shader::Type type, const char* name)
{
	return new ShaderDX11(type, name);
}

Texture* DeviceDX11::CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp)
{
	return new TextureDX11(w, h, f, l, rt, tp);
}

int DeviceDX11::GetPrimitiveType(Primitive type)
{
	int tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (type)
	{
		case LineStrip:
		{
			tp = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		}
		break;
		case LinesList:
		{
			tp = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		}
		break;
		case TriangleStrip:
		{
			tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}
		break;
		case TrianglesList:
		{
			tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
		break;
	}

	return tp;
}

int DeviceDX11::CalcPrimCount(Primitive type, int primCount)
{
	int tp = primCount;

	switch (type)
	{
		case LineStrip:
		{
			tp = primCount + 1;
		}
		break;
		case LinesList:
		{
			tp = primCount * 2;
		}
		break;
		case TriangleStrip:
		{
			tp = primCount + 2;
		}
		break;
		case TrianglesList:
		{
			tp = primCount * 3;
		}
		break;
	}

	return tp;
}

void DeviceDX11::Draw(Primitive prim, int startVertex, int primCount)
{
	UpdateStates();

	if (primCount > 5000)
	{
		int k = 0;
		k++;
	}
	Device::PreDraw();

	immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(prim));
	immediateContext->Draw(CalcPrimCount(prim, primCount), startVertex);
}

void DeviceDX11::DrawIndexed(Primitive prim, int startVertex, int starIndex, int primCount)
{
	UpdateStates();

	Device::PreDraw();

	immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(prim));
	immediateContext->DrawIndexed(CalcPrimCount(prim, primCount), starIndex, startVertex);
}


void DeviceDX11::SetAlphaBlend(bool enable)
{
	blend_desc->RenderTarget[0].BlendEnable = enable;

	blend_changed = true;
}

void DeviceDX11::SetBlendFunc(BlendArg src, BlendArg dest)
{
	blend_desc->RenderTarget[0].SrcBlend = (D3D11_BLEND)(src + 1);
	blend_desc->RenderTarget[0].DestBlend = (D3D11_BLEND)(dest + 1);

	blend_changed = true;
}

void DeviceDX11::SetBlendOperation(BlendOp op)
{
	blend_desc->RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(op + 1);

	blend_changed = true;
}

void DeviceDX11::SetDepthTest(bool enable)
{
	ds_desc->DepthEnable = enable;
	ds_changed = true;
}

void DeviceDX11::SetDepthWriting(bool enable)
{
	if (enable)
	{
		ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}
	else
	{
		ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	ds_changed = true;
}

void DeviceDX11::SetDepthFunc(CompareFunc func)
{
	ds_desc->DepthFunc = (D3D11_COMPARISON_FUNC)(func + 1);
	ds_changed = true;
}

void DeviceDX11::SetCulling(CullMode mode)
{
	raster_desc->CullMode = (D3D11_CULL_MODE)(mode + 1);
	raster_changed = true;
}

void DeviceDX11::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
{
	float curDepthBias = 0.0f;
	float curBiasSlope = 0.0f;

	if (enable)
	{
		curDepthBias = depthOffset;
		curBiasSlope = slopeZBias;
	}

	raster_desc->DepthBias = curDepthBias;
	raster_desc->SlopeScaledDepthBias = curBiasSlope;

	raster_changed = true;
}

void DeviceDX11::UpdateStates()
{
	if (blend_changed)
	{
		RELEASE(blend_state)

		pd3dDevice->CreateBlendState(blend_desc, &blend_state);
		immediateContext->OMSetBlendState(blend_state, 0, 0xffffffff);

		blend_changed = false;
	}

	if (ds_changed)
	{
		RELEASE(ds_state)

		instance->pd3dDevice->CreateDepthStencilState(ds_desc, &ds_state);
		immediateContext->OMSetDepthStencilState(ds_state, 255);

		ds_changed = false;
	}

	if (raster_changed)
	{
		RELEASE(raster_state)

		pd3dDevice->CreateRasterizerState(raster_desc, &raster_state);
		immediateContext->RSSetState(raster_state);

		raster_changed = false;
	}

	if (need_set_rt)
	{
		ID3D11ShaderResourceView* pSRV[3] = { NULL, NULL, NULL };
		immediateContext->PSSetShaderResources(0, 3, pSRV);

		int count = 0;

		for (int i = 0; i < 6; i++)
		{
			if (cur_rt[i])
			{
				count++;
			}
			else
			{
				break;
			}
		}

		immediateContext->OMSetRenderTargets(count, cur_rt, cur_depth);
		need_set_rt = false;

		D3D11_VIEWPORT vp;

		if (cur_rt[0])
		{
			vp.Width = (FLOAT)cur_rt_w;
			vp.Height = (FLOAT)cur_rt_h;
		}
		else
		{
			vp.Width = (FLOAT)cur_depth_w;
			vp.Height = (FLOAT)cur_depth_h;
		}

		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		immediateContext->RSSetViewports(1, &vp);
	}
}

void DeviceDX11::SetScissor(bool enable)
{
	raster_desc->ScissorEnable = enable;
	raster_changed = true;
}

void DeviceDX11::SetScissorRect(const Rect& rect)
{
	D3D11_RECT rc;
	rc.bottom = rect.bottom;
	rc.left = rect.left;
	rc.right = rect.right;
	rc.top = rect.top;

	immediateContext->RSSetScissorRects(1, &rc);
}

void DeviceDX11::GetScissorRect(Rect& rect)
{
	D3D11_RECT rc;
	unsigned int num = 1;
	immediateContext->RSGetScissorRects(&num, &rc);

	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right;
	rect.bottom = rc.bottom;
}

void DeviceDX11::SetViewport(const Viewport& viewport)
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)viewport.width;
	vp.Height = (float)viewport.height;
	vp.MinDepth = viewport.minZ;
	vp.MaxDepth = viewport.maxZ;
	vp.TopLeftX = viewport.x;
	vp.TopLeftY = viewport.y;

	immediateContext->RSSetViewports(1, &vp);

	vp_was_setted = true;
}

void DeviceDX11::GetViewport(Viewport& viewport)
{
	if (vp_was_setted)
	{
		D3D11_VIEWPORT d3dviewport;
		unsigned int num = 1;
		immediateContext->RSGetViewports(&num, &d3dviewport);

		viewport.x = d3dviewport.TopLeftX;
		viewport.y = d3dviewport.TopLeftY;
		viewport.width = d3dviewport.Width;
		viewport.height = d3dviewport.Height;
		viewport.minZ = d3dviewport.MinDepth;
		viewport.maxZ = d3dviewport.MaxDepth;
	}
	else
	{
		viewport.x = 0;
		viewport.y = 0;

		if (cur_rt[0])
		{
			viewport.width = (FLOAT)cur_rt_w;
			viewport.height = (FLOAT)cur_rt_h;
		}
		else
		{
			viewport.width = (FLOAT)cur_depth_w;
			viewport.height = (FLOAT)cur_depth_h;
		}

		viewport.minZ = 0.0f;
		viewport.maxZ = 1.0f;
	}
}

void DeviceDX11::SetRenderTarget(int slot, Texture* rt)
{
	vp_was_setted = false;

	if (rt)
	{
		TextureDX11* rt_dx11 = (TextureDX11*)rt;

		if (!rt_dx11->rt)
		{
			pd3dDevice->CreateRenderTargetView(rt_dx11->texture, NULL, &rt_dx11->rt);
		}

		if (slot == 0)
		{
			cur_rt_w = rt_dx11->GetWidth();
			cur_rt_h = rt_dx11->GetHeight();
		}

		cur_rt[slot] = rt_dx11->rt;
	}
	else
	{
		if (slot == 0)
		{
			cur_rt_w = -1;
			cur_rt_h = -1;
		}

		cur_rt[slot] = NULL;
	}

	need_set_rt = true;
}

void DeviceDX11::SetDepth(Texture* depth)
{
	vp_was_setted = false;

	if (depth)
	{
		TextureDX11* depth_dx11 = (TextureDX11*)depth;

		if (!depth_dx11->depth)
		{
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D16_UNORM;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

			pd3dDevice->CreateDepthStencilView(depth_dx11->texture, &descDSV, &depth_dx11->depth);
		}

		cur_depth_w = depth_dx11->GetWidth();
		cur_depth_h = depth_dx11->GetHeight();

		cur_depth = depth_dx11->depth;
	}
	else
	{
		cur_depth_w = -1;
		cur_depth_h = -1;
		cur_depth = NULL;
	}

	need_set_rt = true;
}

void DeviceDX11::RestoreRenderTarget()
{
	cur_rt_w = scr_w;
	cur_rt_h = scr_h;
	
	for (int i = 0; i < 6; i++)
	{
		cur_rt[0] = NULL;
	}

	cur_rt[0] = renderTargetView;
	cur_depth = depthStencilView;
	need_set_rt = true;
}

void DeviceDX11::Release()
{
	delete this;
}