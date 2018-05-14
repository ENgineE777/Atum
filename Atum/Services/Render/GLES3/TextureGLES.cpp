
#include "TextureGLES.h"
#include "Services/Render/Render.h"

TextureGLES::FormatDesc TextureGLES::GetFormat(Format fmt)
{
	FormatDesc desc;

	switch (fmt)
	{
		case FMT_A8R8G8B8:
			desc.internal_format = GL_RGBA;
			desc.format = GL_RGBA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case FMT_A8R8:
			desc.internal_format = GL_LUMINANCE_ALPHA;
			desc.format = GL_LUMINANCE_ALPHA;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case FMT_A8:
			desc.internal_format = GL_LUMINANCE;
			desc.format = GL_LUMINANCE;
			desc.type = GL_UNSIGNED_BYTE;
			break;
		case FMT_R16_FLOAT: 
			desc.internal_format = GL_RED;
			desc.format = GL_RED;
			desc.type = GL_FLOAT;
			break;
		case FMT_D16:
			desc.internal_format = GL_DEPTH_COMPONENT;
			desc.format = GL_DEPTH_COMPONENT;
			desc.type = GL_FLOAT;
			break;
	}

	return desc;
}

TextureGLES::TextureGLES(int w, int h, Format f, int l, bool is_rt, Type tp) : Texture(w, h, f, l, tp)
{
	GLuint types[] = { GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_3D};
	gl_type = types[type];

	fdesc = GetFormat(format);

	glGenTextures(1, &texture);
	glBindTexture(gl_type, texture);

	if (lods == 0)
	{
		lods = Texture::GetLevels(width, height, 1);
	}

	w = width;
	h = height;

	for (int i = 0; i < lods; i++)
	{
		glTexImage2D(gl_type, i, fdesc.internal_format, w, h, 0, fdesc.format, fdesc.type, nullptr);

		NextMip(w, h, 1);
	}

	glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void TextureGLES::SetFilters(FilterType magmin, FilterType mipmap)
{
	Texture::SetFilters(magmin, mipmap);

	glBindTexture(gl_type, texture);

	if (magmin == Point)
	{
		glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (lods == 1)
	{

	}
	else
	{
		if (magminf == Point && mipmapf == Point)
		{
			glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		else
		if (magminf == Point && mipmapf == Linear)
		{
			glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		else
		if (magminf == Linear && mipmapf == Point)
		{
			glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		if (magminf == Linear && mipmapf == Linear)
		{
			glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
	}
}

void TextureGLES::SetAdress(TextureAddress adress)
{
	Texture::SetAdress(adress);
}

void TextureGLES::SetAdressImpl(GLuint coord, TextureAddress adress)
{
	glBindTexture(gl_type, texture);

	if (adress == Clamp)
	{
		glTexParameteri(gl_type, adress, GL_CLAMP_TO_EDGE);
	}
	else
	if (adress == Mirror)
	{
		glTexParameteri(gl_type, adress, GL_MIRRORED_REPEAT);
	}
	else
	{
		glTexParameteri(gl_type, adress, GL_REPEAT);
	}
}

void TextureGLES::SetAdressU(TextureAddress adress)
{
	Texture::SetAdressU(adress);
	SetAdressImpl(GL_TEXTURE_WRAP_S, adress);
}


void TextureGLES::SetAdressV(TextureAddress adress)
{
	Texture::SetAdressV(adress);
	SetAdressImpl(GL_TEXTURE_WRAP_T, adress);
}

void TextureGLES::SetAdressW(TextureAddress adress)
{
	Texture::SetAdressW(adress);
	SetAdressImpl(GL_TEXTURE_WRAP_R, adress);
}

void TextureGLES::GenerateMips()
{
	glBindTexture(gl_type, texture);
	glGenerateMipmap(gl_type);
}

void TextureGLES::Apply(int slot)
{
}

void TextureGLES::Update(int level, int layer, uint8_t* data, int stride)
{
	glBindTexture(gl_type, texture);
	glTexSubImage2D(gl_type, level, 0, 0, width, height, fdesc.format, fdesc.type, data);
}

void TextureGLES::Release()
{
	if (!render.TexRefIsEmpty(this))
	{
		return;
	}

	delete this;
}