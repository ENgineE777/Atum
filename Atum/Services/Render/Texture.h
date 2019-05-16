
#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

class Texture
{
public:

	enum Format
	{
		FMT_A8R8G8B8 = 0 /*!< Hardware button in pressed state */,
		FMT_A8R8 /*!< Hardware button in pressed state */,
		FMT_A8 /*!< Hardware button in pressed state */,
		FMT_R16_FLOAT /*!< Hardware button in pressed state */,
		FMT_D16 /*!< Hardware button in pressed state */
	};

	enum FilterType
	{
		Point = 0 /*!< Hardware button in pressed state */,
		Linear /*!< Hardware button in pressed state */
	};

	enum TextureAddress
	{
		Wrap = 0 /*!< Hardware button in pressed state */,
		Mirror /*!< Hardware button in pressed state */,
		Clamp /*!< Hardware button in pressed state */,
		Border /*!< Hardware button in pressed state */
	};

	enum Type
	{
		Tex2D = 0 /*!< Hardware button in pressed state */,
		Array /*!< Hardware button in pressed state */,
		Cube /*!< Hardware button in pressed state */,
		Volume /*!< Hardware button in pressed state */
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

	static void NextMip(int& width, int& height, int block_size);
	static int GetLevels(int width, int height, int block_size);

	virtual ~Texture() {};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int GetWidth()  { return width; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int GetHeight() { return height; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int GetFormat() { return format; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int GetLods()   { return lods; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual int GetType()   { return type; };

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetFilters(FilterType magmin, FilterType mipmap)
	{
		magminf = magmin;
		mipmapf = mipmap;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetAdress(TextureAddress adress)
	{
		adressU = adress;
		adressV = adress;
		adressW = adress;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetAdressU(TextureAddress adress)
	{
		adressU = adress;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetAdressV(TextureAddress adress)
	{
		adressV = adress;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void SetAdressW(TextureAddress adress)
	{
		adressW = adress;
	};

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void GenerateMips() = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Apply(int slot) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	virtual void Update(int level, int layer, uint8_t* data, int stride) = 0;

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
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