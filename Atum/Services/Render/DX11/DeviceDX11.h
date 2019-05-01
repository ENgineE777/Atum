
#pragma once

#include "Services/Render/Device.h"
#include "d3d11.h"

/**
\ingroup gr_code_services_render_dx11
*/

class DeviceDX11 : public Device
{
	friend class Render;
	friend class GeometryBufferDX11;
	friend class ShaderDX11;
	friend class TextureDX11;
	friend class VertexDeclDX11;

	ID3D11Device*             pd3dDevice = nullptr;
	ID3D11DeviceContext*      immediateContext = nullptr;
	IDXGISwapChain*           swapChain = nullptr;
	ID3D11Texture2D*          renderTarget = nullptr;
	ID3D11RenderTargetView*   renderTargetView = nullptr;
	ID3D11ShaderResourceView* renderTargetShaderView = nullptr;
	ID3D11Texture2D*          depthStencil = nullptr;
	ID3D11DepthStencilView*   depthStencilView = nullptr;
	IDXGIFactory1*            factory = nullptr;

	struct WindowBackBufferHolder
	{
		int scr_w = 0;
		int scr_h = 0;
		float cur_aspect = 0.0f;
		HWND handle;
		ID3D11Texture2D*          renderTarget = nullptr;
		ID3D11RenderTargetView*   renderTargetView = nullptr;
		ID3D11ShaderResourceView* renderTargetShaderView = nullptr;
		ID3D11Texture2D*          depthStencil = nullptr;
		ID3D11DepthStencilView*   depthStencilView = nullptr;
		IDXGISwapChain*           swapChain = nullptr;
	};

	vector<WindowBackBufferHolder> backbuffer_holders;

	D3D11_BLEND_DESC*  blend_desc;
	ID3D11BlendState*  blend_state;
	bool                      blend_changed;

	D3D11_DEPTH_STENCIL_DESC* ds_desc;
	ID3D11DepthStencilState*  ds_state;
	int                              ds_stencil_ref;
	bool                             ds_changed;

	D3D11_RASTERIZER_DESC* raster_desc;
	ID3D11RasterizerState* raster_state;
	bool                          raster_changed;

	bool vp_was_setted;

	ID3D11RenderTargetView* cur_rt[6];
	ID3D11DepthStencilView* cur_depth;
	bool need_set_rt;
	int  cur_depth_w;
	int  cur_depth_h;

	bool need_apply_prog = false;
	class VertexDeclDX11* cur_vdecl = nullptr;
	bool need_apply_vdecl = false;

	DeviceDX11();
	virtual bool Init(void* external_device);
	virtual void PrepareProgram(Program* program);
	virtual void Release();

	virtual Shader* CreateShader(Shader::Type type, const char* name);
	void UpdateStates();

public:

	static DeviceDX11* instance;

	virtual void  SetVideoMode(int wgt, int hgt, void* data);
	virtual void* GetBackBuffer();
	virtual int   GetWidth();
	virtual int   GetHeight();
	virtual float GetAspect();

	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue);
	virtual void Present();

	virtual void SetProgram(Program* program);

	virtual VertexDecl* CreateVertexDecl(int count, VertexDecl::ElemDesc* elems);
	virtual void SetVertexDecl(VertexDecl* vdecl);

	virtual GeometryBuffer* CreateBuffer(int count, int stride);
	virtual void SetVertexBuffer(int slot, GeometryBuffer* buffer);
	virtual void SetIndexBuffer(GeometryBuffer* buffer);

	virtual Texture* CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp);

	int GetPrimitiveType(Primitive type);
	int CalcPrimCount(Primitive type, int primCount);
	virtual void Draw(Primitive prim, int startVertex, int primCount);
	virtual void DrawIndexed(Primitive prim, int startVertex, int startIndex, int primCount);

	virtual void SetAlphaBlend(bool enable);
	virtual void SetBlendFunc(BlendArg src, BlendArg dest);
	virtual void SetBlendOperation(BlendOp op);
	virtual void SetDepthTest(bool enable);
	virtual void SetDepthWriting(bool enable);
	virtual void SetDepthFunc(CompareFunc func);
	virtual void SetCulling(CullMode mode);
	virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset);

	virtual void SetViewport(const Viewport& viewport);
	virtual void GetViewport(Viewport& viewport);

	virtual void SetRenderTarget(int slot, Texture* rt);
	virtual void SetDepth(Texture* depth);
	virtual void RestoreRenderTarget();
};