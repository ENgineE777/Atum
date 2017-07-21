
#pragma once

#include "Services/Render/Texture.h"

class TextureDX11 : public Texture
{
	class ID3D11SamplerState* sampler;
	bool sampler_need_recrete;

public:
	class ID3D11Texture2D* texture;
	class ID3D11ShaderResourceView* srview;

	class ID3D11RenderTargetView* rt;
	class ID3D11DepthStencilView* depth;
	
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
	virtual void* GetData();
	virtual void Release();
};
