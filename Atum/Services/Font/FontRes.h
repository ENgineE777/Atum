#pragma once

class  Texture;

#include "Support/Support.h"
#include "Services/Render/Render.h"
#include "stb_truetype.h"

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
	byte*    tex_buffer2;

	int height;
	float used_height;

	int tex_w;
	int tex_h;

	stbtt_pack_context context;
	bool need_update_tex;
	Buffer font_fb;

	std::map<int, Glyph> glyphs;

public:

	FontRes(const char* fl_name, int hgt);
	virtual bool Load();
	Glyph* GenerateChar(int ch);
	Font* CreateReference();

	void Print(Matrix& transform, float font_scale, Color color, const char* text);

	int GetHeight();
	int GetCharHeight();

	virtual void Release();

	void UpdateTexture();

	Glyph* GetGlyph(int code);
};
