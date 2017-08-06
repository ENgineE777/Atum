
#include "TextureDX11.h"
#include "DeviceDX11.h"
#include "Services/Render/Render.h"

#include "d3d11.h"

int TextureDX11::GetFormat(Format fmt)
{
	switch (fmt)
	{
		case FMT_A8R8G8B8: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case FMT_R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
		case FMT_D16: return DXGI_FORMAT_R16_TYPELESS;
	}

	return 0;
}

TextureDX11::TextureDX11(int w, int h, Format f, int l, bool is_rt, Type tp) : Texture(w, h, f, l, tp)
{
	texture = NULL;

	DXGI_FORMAT fmt = (DXGI_FORMAT)GetFormat(f);

	D3D11_TEXTURE2D_DESC desc;

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = l;
	desc.ArraySize = 1;
	desc.Format = fmt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.Usage = D3D11_USAGE_DEFAULT;


	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (fmt == DXGI_FORMAT_R16_TYPELESS)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.Usage = D3D11_USAGE_DEFAULT;

		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
	}

	DeviceDX11::instance->pd3dDevice->CreateTexture2D(&desc, NULL, &texture);

	if (l == 0)
	{
		texture->GetDesc(&desc);
		l = desc.MipLevels;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = fmt;

	if (fmt == DXGI_FORMAT_R16_TYPELESS)
	{
		srvDesc.Format = DXGI_FORMAT_R16_UNORM;
	}

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = l;

	sampler = NULL;
	sampler_need_recrete = true;

	DeviceDX11::instance->pd3dDevice->CreateShaderResourceView( texture, &srvDesc, &srview );

	rt = NULL;
	depth = NULL;
}

void TextureDX11::SetFilters(FilterType magmin, FilterType mipmap)
{
	Texture::SetFilters(magmin, mipmap);

	sampler_need_recrete = true;
}

void TextureDX11::SetAdress(TextureAddress adress)
{
	Texture::SetAdress(adress);

	sampler_need_recrete = true;
}

void TextureDX11::SetAdressU(TextureAddress adress)
{
	Texture::SetAdressU(adress);

	sampler_need_recrete = true;
}


void TextureDX11::SetAdressV(TextureAddress adress)
{
	Texture::SetAdressV(adress);

	sampler_need_recrete = true;
}


void TextureDX11::SetAdressW(TextureAddress adress)
{
	Texture::SetAdressW(adress);

	sampler_need_recrete = true;
}

void TextureDX11::GenerateMips()
{
	DeviceDX11::instance->immediateContext->GenerateMips(srview);
}

void TextureDX11::Apply(int slot)
{
	if (sampler_need_recrete)
	{
		RELEASE(sampler)

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof(sampDesc) );

		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		if (magminf == Point && mipmapf == Point)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else
		if (magminf == Point && mipmapf == Linear)
		{
			filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		}
		else
		if (magminf == Linear && mipmapf == Point)
		{
			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
		else
		if (magminf == Linear && mipmapf == Linear)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}		

		sampDesc.Filter = filter;
		sampDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)(adressU + 1);
		sampDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)(adressV + 1);
		sampDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)(adressW + 1);
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		DeviceDX11::instance->pd3dDevice->CreateSamplerState( &sampDesc, &sampler );

		sampler_need_recrete = false;
	}

	DeviceDX11::instance->immediateContext->PSSetSamplers( slot, 1, &sampler );
}

void TextureDX11::Update(int level, int layer, uint8_t* data, int stride)
{
	UINT index = D3D11CalcSubresource(level, layer, 1);
	DeviceDX11::instance->immediateContext->UpdateSubresource(texture, index, NULL, data, stride, 0);
}

void* TextureDX11::GetData()
{
	return srview;
}

void TextureDX11::Release()
{
	if (!render.TexRefIsEmpty(this))
	{
		return;
	}

	RELEASE(sampler)
	RELEASE(texture)
	RELEASE(srview)
	
	delete this;
}