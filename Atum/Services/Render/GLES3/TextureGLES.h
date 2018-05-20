
#pragma once

#include "Services/Render/Texture.h"

#ifdef PLATFORM_ANDROID
#include <GLES3/gl3.h>
#endif

class TextureGLES : public Texture
{
public:

	GLuint texture;
	GLuint gl_type;

	struct FormatDesc
	{
		int internal_format = 0;
		int format = 0;
		int type = 0;
	};

	FormatDesc fdesc;

	static FormatDesc GetFormat(Format fmt);

	TextureGLES(int w, int h, Format f, int l, bool rt, Type tp);

	virtual void SetFilters(FilterType magmin, FilterType mipmap);
	void SetAdressImpl(GLuint coord, TextureAddress adress);
	virtual void SetAdress(TextureAddress adress);
	virtual void SetAdressU(TextureAddress adress);
	virtual void SetAdressV(TextureAddress adress);
	virtual void SetAdressW(TextureAddress adress);
	
	virtual void GenerateMips();
	virtual void Apply(int slot);

	virtual void Update(int level, int layer, uint8_t* data, int stride);
	virtual void Release();
};
