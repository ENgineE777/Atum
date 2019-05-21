
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
		FMT_A8R8G8B8 = 0,
		FMT_A8R8,
		FMT_A8,
		FMT_R16_FLOAT,
		FMT_D16
	};

	enum FilterType
	{
		Point = 0 /*!< Point filter */,
		Linear /*!< Linera filtraion */
	};

	enum TextureAddress
	{
		Wrap = 0 /*!< Wrapping */,
		Mirror /*!< Mirroring */,
		Clamp /*!< Clamping */,
		Border /*!< Use border color */
	};

	enum Type
	{
		Tex2D = 0 /*!< 2D texture */,
		Array /*!< Array texture */,
		Cube /*!< Cube texture */,
		Volume /*!< Volumetric texture */
	};

#ifndef DOXYGEN_SKIP
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

	virtual void Apply(int slot) = 0;
	virtual void Update(int level, int layer, uint8_t* data, int stride) = 0;

#endif

	/**
	\brief Get width of a texture

	\return Width of a texture
	*/
	virtual int GetWidth() { return width; };

	/**
	\brief Get height of a texture

	\return Height of a texture
	*/
	virtual int GetHeight() { return height; };

	/**
	\brief Get format of a texture

	\return Format of a texture
	*/
	virtual int GetFormat() { return format; };

	/**
	\brief Get number of lods of a texture

	\return Number of a lods of a texture
	*/
	virtual int GetLods() { return lods; };

	/**
	\brief Get type of a texture

	\return Type of a texture
	*/
	virtual int GetType() { return type; };

	/**
	\brief Set filtering type

	\param[in] magmin Filtering for magnifying
	\param[in] mipmap Filtering for mip mapping

	*/
	virtual void SetFilters(FilterType magmin, FilterType mipmap)
	{
		magminf = magmin;
		mipmapf = mipmap;
	};

	/**
	\brief Set adress type

	\param[in] adress Adress type
	*/
	virtual void SetAdress(TextureAddress adress)
	{
		adressU = adress;
		adressV = adress;
		adressW = adress;
	};

	/**
	\brief Set adress type for u coordinate

	\param[in] adress Adress type
	*/
	virtual void SetAdressU(TextureAddress adress)
	{
		adressU = adress;
	};

	/**
	\brief Set adress type for v coordinate

	\param[in] adress Adress type
	*/
	virtual void SetAdressV(TextureAddress adress)
	{
		adressV = adress;
	};

	/**
	\brief Set adress type for w coordinate

	\param[in] adress Adress type
	*/
	virtual void SetAdressW(TextureAddress adress)
	{
		adressW = adress;
	};

	/**
	\brief Generate full mip map chain
	*/
	virtual void GenerateMips() = 0;

	/**
	\brief Texture should released only via this method
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