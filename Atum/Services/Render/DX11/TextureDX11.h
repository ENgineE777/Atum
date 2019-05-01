
#pragma once

#include "Services/Render/Texture.h"

/**
\ingroup gr_code_services_render_dx11
*/

class TextureDX11 : public Texture
{
	struct ID3D11SamplerState* sampler;
	bool sampler_need_recrete;

public:
	struct ID3D11Texture2D* texture;
	struct ID3D11ShaderResourceView* srview;

	struct ID3D11RenderTargetView* rt;
	struct ID3D11DepthStencilView* depth;
	
	static int GetFormat(Format fmt);

	TextureDX11(int w, int h, Format f, int l, bool rt, Type tp);

	virtual void SetFilters(FilterType magmin, FilterType mipmap);
	virtual void SetAdress(TextureAddress adress);
	virtual void SetAdressU(TextureAddress adress);
	virtual void SetAdressV(TextureAddress adress);
	virtual void SetAdressW(TextureAddress adress);
	
	virtual void GenerateMips();
	virtual void Apply(int slot);

	virtual void Update(int level, int layer, uint8_t* data, int stride);
	virtual void Release();
};
