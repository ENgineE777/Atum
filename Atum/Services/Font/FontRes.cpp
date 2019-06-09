
#include "Fonts.h"
#include "Services/Core/Core.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "Support/stb/stb_truetype.h"

int GetPow2(int value)
{
	int val = 1;
	for (int i=0;i<15;i++)
	{
		if (value <= val) return val;
		val *= 2;
	}

	return 1024;
}

FontRes::FontRes(const char* fl_name, int hgt)
{
	fileName = fl_name;

	tex_w = -1;
	tex_h = -1;

	height = hgt;

	tex = nullptr;

	need_update_tex = false;
	tex_buffer = nullptr;

	context = new stbtt_pack_context();
}

FontRes::Glyph* FontRes::GenerateChar(int ch)
{
	Glyph& set_glyph = glyphs[ch];

	stbtt_packedchar packed;
	stbtt_PackFontRange(context, font_fb.GetData(), 0, STBTT_POINT_SIZE(used_height), ch, 1, &packed);

	set_glyph.width = (packed.x1 - packed.x0);
	set_glyph.height = (packed.y1 - packed.y0);

	set_glyph.x_offset = (float)packed.xoff;
	set_glyph.y_offset = (float)packed.yoff + height;

	set_glyph.x_advance = packed.xadvance;

	if (set_glyph.width > 0 && set_glyph.height > 0)
	{
		set_glyph.u = (float)(packed.x0) / (float)(tex_w);
		set_glyph.v = (float)(packed.y0) / (float)(tex_h);

		set_glyph.du = (float)(set_glyph.width) / (float)(tex_w);
		set_glyph.dv = (float)(set_glyph.height) / (float)(tex_h);

		set_glyph.skip = 0;
	}
	else
	{
		set_glyph.x_offset = 0;
		set_glyph.x_advance = height * 0.45f;
		set_glyph.skip = 1;
	}

	need_update_tex = true;

	return &set_glyph;
}

bool FontRes::Load()
{
	RELEASE(tex)

	tex_w = -1;
	tex_h = -1;
	tex = nullptr;

	if (!font_fb.Load(fileName.c_str()))
	{
		return false;
	}

	used_height = height * 1.38f;

	tex_w = 1024;
	tex_h = 128;// GetPow2((int)((400.0f / nums) * height));

	if (tex_h > 2048)
	{
		tex_h = 2048;
	}

	cur_tex_x = 3;
	cur_tex_y = 1;
	cur_tex_row_height = 0;

	tex_buffer = new uint8_t[tex_w * tex_h];
	memset(tex_buffer, 0, tex_w * tex_h);

	stbtt_PackBegin(context, tex_buffer, tex_w, tex_h, 0, 1, nullptr);

	tex = core.render.GetDevice()->CreateTexture(tex_w, tex_h, Texture::FMT_A8, 1, false, Texture::Tex2D);

	return true;
}

Font* FontRes::CreateReference()
{
	//AddRef();

	return new Font(this);
}

FontRes::Glyph* FontRes::GetGlyph(int code)
{
	if (glyphs.find(code) == glyphs.end())
	{
		return GenerateChar(code);
	}

	return &glyphs[code];
}

float FontRes::GetLineBreak(vector<FontRes::LineBreak>& line_breaks, const char* text, int width)
{
	float scr_y =(float) height;

	int w = 0;
	int bytes = 0;

	int line = -1;

	line_breaks.clear();

	FontRes::LineBreak breaker;
	breaker.width = 0.5f;

	int last_whitespace = -1;

	int len = StringUtils::GetLen(text);

	for (int i = 0; i<len; i++)
	{
		if (!StringUtils::BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		if (w > 65000) continue;

		if (w == 10)
		{
			continue;
		}
		else
		if (w == '\\')
		{
			if (i <len - 1)
			{
				if (text[i + 1] == 'n')
				{
					i++;

					breaker.index = i;
					line_breaks.push_back(breaker);
					breaker.width = 0.5f;
					scr_y += height;
					last_whitespace = -1;
					continue;
				}
			}
		}

		if (w == ' ')
		{
			last_whitespace = i;
		}

		Glyph* set_glyph = GetGlyph(w);
		if (!set_glyph) continue;

		breaker.width += set_glyph->x_advance;
	}

	line_breaks.push_back(breaker);

	return scr_y;
}

void FontRes::Print(vector<FontRes::LineBreak>& line_breaks, Matrix& transform, float font_scale, Color color, const char* text)
{
	if (!tex) return;

	//render.DebugSprite(tex, 0.0f, 1024.0f);

	int len = 0;

	len = StringUtils::GetLen(text);

	if (len == 0) return;

	core.render.GetDevice()->SetProgram(core.fonts.fntProg);

	Matrix tmp;

	Vector4 params[4];

	transform._41 = (float)((int)transform._41);
	transform._42 = (float)((int)transform._42);
	transform._43 = (float)((int)transform._43);

	tmp = transform;

	params[0] = Vector4((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight(), 0.5f, 0.0f);

	core.fonts.fntProg->SetVector(Shader::Type::Vertex, "desc", &params[0], 1);
	core.fonts.fntProg->SetMatrix(Shader::Type::Vertex, "transform", &tmp, 1);

	if (font_scale > 1.01f)
	{
		tex->SetFilters(Texture::Linear, Texture::Linear);
	}
	else
	{
		tex->SetFilters(Texture::Point, Texture::Point);
	}

	core.render.GetDevice()->SetVertexDecl(core.fonts.vdecl);
	core.render.GetDevice()->SetVertexBuffer(0, core.fonts.vbuffer);

	core.fonts.fntProg->SetTexture(Shader::Type::Pixel, "diffuseMap", tex);
	core.fonts.fntProg->SetVector(Shader::Type::Pixel, "color", (Vector4*)&color, 1);

	float scr_x = 0;

	scr_x += 0.5f;

	float scr_y = 0;

	scr_y += 0.5f;

	int dr_index = 0;
	
	Fonts::FontVertex* v = (Fonts::FontVertex*)core.fonts.vbuffer->Lock();

	int w = 0;
	int bytes = 0;

	int line = -1;
	int x_offset = 0;

	for (int i=0;i<len;i++)
	{
		if (!StringUtils::BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		if (w > 65000) continue;

		if (w == 10)
		{
			continue;
		}
		else
		if (w == '\\')
		{
			if (i <len-1)
			{
				if (text[i+1] == 'n')
				{
					i++;
					continue;
				}
			}
		}
		
		Glyph* set_glyph = GetGlyph(w);
		if (!set_glyph) continue;

		if (set_glyph->skip == 0)
		{
			float char_x = scr_x + x_offset + set_glyph->x_offset * font_scale;
			float char_y = scr_y + set_glyph->y_offset * font_scale - 0.5f;
			float char_w = set_glyph->width * font_scale;
			float char_h = set_glyph->height * font_scale;

			float char_u = set_glyph->u;
			float char_v = set_glyph->v;

			float char_du = set_glyph->du;
			float char_dv = set_glyph->dv;

			v[dr_index * 6 + 0].pos = Vector(char_x, char_y + char_h , 0.5f);
			v[dr_index * 6 + 1].pos = Vector(char_x, char_y, 0.5f);
			v[dr_index * 6 + 2].pos = Vector(char_x + char_w,  char_y, 0.5f);

			v[dr_index * 6 + 3].pos = Vector(char_x, char_y + char_h, 0.5f);
			v[dr_index * 6 + 4].pos = Vector(char_x + char_w,  char_y, 0.5f);
			v[dr_index * 6 + 5].pos = Vector(char_x + char_w, char_y + char_h, 0.5f);
		
			v[dr_index * 6 + 0].uv = Vector2(char_u, char_v + char_dv);
			v[dr_index * 6 + 1].uv = Vector2(char_u, char_v);
			v[dr_index * 6 + 2].uv = Vector2(char_u + char_du, char_v);

			v[dr_index * 6 + 3].uv = Vector2(char_u, char_v + char_dv);
			v[dr_index * 6 + 4].uv = Vector2(char_u + char_du, char_v);
			v[dr_index * 6 + 5].uv = Vector2(char_u + char_du, char_v + char_dv);
			
			dr_index++;
		}
		
		scr_x += set_glyph->x_advance * font_scale;

		if (dr_index >= 998)
		{
			core.fonts.vbuffer->Unlock();

			if (need_update_tex) UpdateTexture();

			core.render.GetDevice()->Draw(Device::TrianglesList, 0, 2 * dr_index);

			dr_index = 0;

			v = (Fonts::FontVertex*)core.fonts.vbuffer->Lock();
		}
	}
	
	core.fonts.vbuffer->Unlock();

	if (dr_index > 0)
	{
		if (need_update_tex) UpdateTexture();

		core.render.GetDevice()->Draw(Device::TrianglesList, 0, 2 * dr_index);
	}
}

int FontRes::GetHeight()
{
	return height;
}

int FontRes::GetCharHeight()
{
	return height;
}

void FontRes::UpdateTexture()
{
	need_update_tex = false;
	tex->Update(0, 0, tex_buffer, tex_w);
}

void FontRes::Release()
{
	DELETE_PTR(context)
	if (tex_buffer)
	{
		free(tex_buffer);
		stbtt_PackEnd(context);
	}

	RELEASE(tex)

	delete this;
}

