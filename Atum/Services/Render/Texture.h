
#pragma once

#include "Support/Support.h"

class Texture
{	
public:

	enum Format
	{
		FMT_A8R8G8B8 = 0,
		FMT_R16_FLOAT,
		FMT_D16
	};

	enum FilterType
	{		
		Point = 0,
		Linear
	};

	enum TextureAddress
	{
		Wrap = 0,
		Mirror,
		Clamp,
		Border
	};

	enum Type
	{
		Tex2D = 0,
		Array,
		Cube,
		Volume
	};

	Texture(int w, int h, Format f, int l, Type tp)
	{
		width = w;
		height = h;
		format = f;
		lods = l;
		type = tp;

		magminf = Linear;
		mipmapf = Linear;

		adressU = Wrap;
		adressV = Wrap;
		adressW = Wrap;
	};

	virtual ~Texture() {};

	virtual int GetWidth()  { return width; };
	virtual int GetHeight() { return height; };
	virtual int GetFormat() { return format; };
	virtual int GetLods()   { return lods; };
	virtual int GetType()   { return type; };

	virtual void SetFilters(FilterType magmin, FilterType mipmap)
	{
		magminf = magmin;
		mipmapf = mipmap;
	};

	virtual void SetAdress(TextureAddress adress)
	{
		adressU = adress;
		adressV = adress;
		adressW = adress;
	};

	virtual void SetAdressU(TextureAddress adress)
	{
		adressU = adress;
	};

	virtual void SetAdressV(TextureAddress adress)
	{
		adressV = adress;
	};

	virtual void SetAdressW(TextureAddress adress)
	{
		adressW = adress;
	};

	virtual void* GetData() = 0;

	virtual void GenerateMips() = 0;
	virtual void Apply(int slot) = 0;

	virtual void Update(int level, int layer, uint8_t* data, int stride) = 0;

	virtual void Release() = 0;

protected:

	int width;
	int height;
	Format format;
	int lods;

	FilterType magminf;
	FilterType mipmapf;
	TextureAddress adressU;
	TextureAddress adressV;
	TextureAddress adressW;
	Type type;
};