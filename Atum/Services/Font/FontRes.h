#pragma once

class  Texture;

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/ftstroke.h>

#include "FontCharmap.h"
#include "Support/Support.h"

#include "Services/Render/Render.h"

class Font;

class FontRes
{	
public:

	struct Glyph
	{
		int   width;
		int   height;
		float x_offset;
		float y_offset;
		float x_advance;
		float u;
		float v;
		float du;
		float dv;
		int   skip;
	};

	struct LineData
	{
		int index;
		float offset;
	};

	string fileName;

protected:	

	Texture* tex;
	int      cur_tex_x;
	int      cur_tex_y;
	int      cur_tex_row_height;

	byte*    tex_buffer;

	int height;
	float outline_thin;

	int tex_w;
	int tex_h;

	FT_Stroker stroker;
	FT_Face face;
	FT_Library library;
	
	bool need_update_tex;
	Buffer font_fb;

public:
	
	FontRes(FT_Library library, const char* fl_name, int hgt, float outline_thin);
	virtual bool Load();
	Glyph* GenerateChar(wchar_t ch);
	int  GenerateCharHeight(wchar_t ch);
	Font* CreateReference();
		
	void Print(Matrix& transform, float font_scale, Color color, const char* text);

	int GetHeight();
	int GetCharHeight();

	Glyph* GetCharGlyph(int ch);

	virtual void Release();

	void UpdateTexture();

	vector<Glyph> glyphs;
	FontCharmap glyphList;	
	Glyph* CheckGlyph(wchar_t characterCode);
};
