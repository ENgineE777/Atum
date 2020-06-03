
#pragma once

#include "Services/Render/Device.h"
#include "d3d11.h"

class DeviceDX11 : public Device
{
	friend class Render;
	friend class DataBufferDX11;
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
	bool Init(void* external_device) override;
	void PrepareProgram(Program* program) override;
	void Release() override;

	Shader* CreateShader(Shader::Type type, const char* name) override;
	void UpdateStates();

public:

	static DeviceDX11* instance;

	void  SetVideoMode(int wgt, int hgt, void* data) override;
	void* GetBackBuffer() override;
	int   GetWidth() override;
	int   GetHeight() override;
	float GetAspect() override;

	void Clear(bool renderTarget, Color color, bool zbuffer, float zValue) override;
	void Present() override;

	void SetProgram(Program* program) override;

	VertexDecl* CreateVertexDecl(int count, VertexDecl::ElemDesc* elems) override;
	void SetVertexDecl(VertexDecl* vdecl) override;

	DataBuffer* CreateBuffer(int count, int stride) override;
	void SetVertexBuffer(int slot, DataBuffer* buffer) override;
	void SetIndexBuffer(DataBuffer* buffer) override;

	Texture* CreateTexture(int w, int h, Texture::Format f, int l, bool rt, Texture::Type tp) override;

	int GetPrimitiveType(Primitive type);
	int CalcPrimCount(Primitive type, int primCount);
	void Draw(Primitive prim, int startVertex, int primCount) override;
	void DrawIndexed(Primitive prim, int startVertex, int startIndex, int primCount) override;

	void SetAlphaBlend(bool enable) override;
	void SetBlendFunc(BlendArg src, BlendArg dest) override;
	void SetBlendOperation(BlendOp op) override;
	void SetDepthTest(bool enable) override;
	void SetDepthWriting(bool enable) override;
	void SetDepthFunc(CompareFunc func) override;
	void SetCulling(CullMode mode) override;
	void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset) override;

	void SetScissors(bool enable) override;
	void SetScissorRect(Rect rec) override;

	void SetViewport(const Viewport& viewport) override;
	void GetViewport(Viewport& viewport) override;

	void SetRenderTarget(int slot, Texture* rt) override;
	void SetDepth(Texture* depth) override;
	void RestoreRenderTarget() override;
};