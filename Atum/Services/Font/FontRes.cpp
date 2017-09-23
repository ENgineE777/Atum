
#include "FontService.h"

//#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
//#include "stb_truetype.h"
//stbtt_fontinfo stb_font;

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

FontRes::FontRes(FT_Library set_library, const char* fl_name, int hgt, float set_outline_thin)
{
	fileName = fl_name;

	tex_w = -1;
	tex_h = -1;
	
	library = set_library;
	height = hgt;
	outline_thin = set_outline_thin;

	tex = nullptr;

	need_update_tex = false;
	tex_buffer = nullptr;

	face = nullptr;
}

int FontRes::GenerateCharHeight(wchar_t ch)
{
	FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT );

	FT_Glyph glyph;

	FT_Get_Glyph( face->glyph, &glyph );

	if (outline_thin > 0.01f)
	{
		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 0);
	}

	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	FT_Glyph glyph_stroke;
	FT_BitmapGlyph bitmap_glyph_stroke;
	FT_Bitmap bitmap_stroke;

	if (outline_thin > 0.01f)
	{
		FT_Get_Glyph( face->glyph, &glyph_stroke );

		FT_Glyph_Stroke(&glyph_stroke, stroker, 0);
		FT_Glyph_To_Bitmap( &glyph_stroke, ft_render_mode_normal, 0, 1 );

		bitmap_glyph_stroke = (FT_BitmapGlyph)glyph_stroke;
		bitmap_stroke = bitmap_glyph_stroke->bitmap;
	}

	int hgt = bitmap_glyph->top;

	FT_Done_Glyph(glyph);

	if (outline_thin > 0.01f) FT_Done_Glyph(glyph_stroke);

	return hgt;
}

FontRes::Glyph* FontRes::GenerateChar(wchar_t ch)
{
	glyphs.push_back(Glyph());

	int index = (int)glyphs.size() - 1;
	glyphList.InsertIndex(ch, index);

	Glyph& set_glyph = glyphs[index];
	
	int fnt_ndex = glyphList.FontIndex( ch );

	if (fnt_ndex == 0)
	{
		//core->TraceTo("FontGen", "%i - absent in %s", ch, GetFileName());
	}

	FT_Load_Glyph( face, fnt_ndex, FT_LOAD_DEFAULT );
 	
	FT_Glyph glyph;
	
	FT_Get_Glyph( face->glyph, &glyph );
	
	if (outline_thin > 0.01f)
	{
		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 0);
	}
	
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
 	
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	FT_Glyph glyph_stroke;
	FT_BitmapGlyph bitmap_glyph_stroke;
	FT_Bitmap bitmap_stroke;

	if (outline_thin > 0.01f)
	{
		FT_Get_Glyph( face->glyph, &glyph_stroke );

		FT_Glyph_Stroke(&glyph_stroke, stroker, 0);
		FT_Glyph_To_Bitmap( &glyph_stroke, ft_render_mode_normal, 0, 1 );

		bitmap_glyph_stroke = (FT_BitmapGlyph)glyph_stroke;
		bitmap_stroke = bitmap_glyph_stroke->bitmap;
	}

	set_glyph.width = bitmap.width;
	set_glyph.height = bitmap.rows;
	set_glyph.x_offset = (float)(face->glyph->metrics.horiBearingX) * (1.0f/64.0f);

	set_glyph.x_advance = (float)(face->glyph->metrics.horiAdvance) * (1.0f/64.0f);

	set_glyph.y_offset = (float)(height - bitmap_glyph->top);

	if ((int)bitmap.width > 0 && (int)bitmap.rows > 0 && (int)(cur_tex_y + bitmap.rows) < tex_h && ch>=32)
	{
		if (cur_tex_x + bitmap.width > 1023)
		{
			cur_tex_x = 1;
			cur_tex_y += cur_tex_row_height + 1;
			cur_tex_row_height = 0;
		}
		
		if (cur_tex_row_height < (int)bitmap.rows)
		{
			cur_tex_row_height = (int)bitmap.rows;
		}

		if (tex_w != -1)
		{
			for (int j=0; j < (int)bitmap.rows; j++)
			{
				for (int i=0; i < (int)bitmap.width; i++)
				{
					if (outline_thin > 0.01f)
					{
						int k = bitmap.buffer[i + bitmap.width * j] - bitmap_stroke.buffer[i + bitmap.width * j];
						if (k<0) k = 0;

						tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 0] = k;
					}
					else
					{
						tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 0] = 255;
					}

					tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 1] = bitmap.buffer[i + bitmap.width * j];
				}
			}

			set_glyph.u = (float)(cur_tex_x) / (float)(tex_w);
			set_glyph.v = (float)(cur_tex_y) / (float)(tex_h);

			set_glyph.du = (float)(bitmap.width) / (float)(tex_w);
			set_glyph.dv = (float)(bitmap.rows) / (float)(tex_h);

			set_glyph.skip = 0;
		}

		cur_tex_x += bitmap.width + 1;
	}
	else
	{
		set_glyph.x_offset = height * 0.45f;
		set_glyph.skip = 1;
	}

	FT_Done_Glyph(glyph);
	
	if (outline_thin > 0.01f) FT_Done_Glyph(glyph_stroke);

	need_update_tex = true;

	return &set_glyph;
}

bool FontRes::Load()
{
	RELEASE(tex)

	tex_w = -1;
	tex_h = -1;
	tex = nullptr;

	if (font_fb.Load(fileName.c_str()))
	{
		if (FT_New_Memory_Face( library, font_fb.GetData(), font_fb.GetSize(), 0, &face))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	glyphList.Init(face);

	FT_Set_Char_Size( face, height << 6, height << 6, 108, 108);	
 
	FT_Select_Charmap( face, FT_ENCODING_UNICODE);	

	if (outline_thin > 0.01f)
	{
		FT_Stroker_New(library, &stroker);
		FT_Stroker_Set(stroker,
					   (int)(outline_thin * 64),
						FT_STROKER_LINECAP_ROUND,
						FT_STROKER_LINEJOIN_ROUND,0);
	}
	
	height = (int)(((float)GenerateCharHeight('A') + (float)GenerateCharHeight('B') + (float)GenerateCharHeight('C')) / 3.0f);

	int nums = (int)(1024.0f / height);

	tex_w = 1024;
	tex_h = GetPow2((int)((400.0f / nums) * height));	

	if (tex_h > 2048)
	{
		tex_h = 2048;
	}

	cur_tex_x = 1;
	cur_tex_y = 1;
	cur_tex_row_height = 0;

	tex_buffer = new byte[tex_w * tex_h * 2];
	memset(tex_buffer,0,tex_w * tex_h * 2);

	tex = render.GetDevice()->CreateTexture(tex_w, tex_h, Texture::FMT_A8R8, 1, false, Texture::Tex2D);

	return true;
}

Font* FontRes::CreateReference()
{
	//AddRef();

	return new Font(this);
}

FontRes::Glyph* FontRes::CheckGlyph(wchar_t characterCode)
{
	int index = glyphList.GlyphListIndex(characterCode);

	if (index == -1)
	{
		return GenerateChar(characterCode);
	}	

	return &glyphs[index];
}

void FontRes::Print(Matrix& transform, float font_scale, Color color, const char* text)
{
	if (!tex) return;

	int len = 0;

	len = StringUtils::GetLen(text);

	if (len == 0) return;

	fonts.fntProg->Apply();
	
	Matrix tmp;

	Vector4 params[4];

	transform._41 = (float)((int)transform._41);
	transform._42 = (float)((int)transform._42);
	transform._43 = (float)((int)transform._43);

	tmp = transform;

	params[0] = Vector4((float)render.GetDevice()->GetWidth(),
						(float)render.GetDevice()->GetHeight(),
						0.5f,
						0.0f);

	fonts.fntProg->VS_SetVector("desc", &params[0], 1);
	fonts.fntProg->VS_SetMatrix("transform", &tmp, 1);

	if (font_scale > 1.01f)
	{
		tex->SetFilters(Texture::Linear, Texture::Linear);
	}
	else
	{
		tex->SetFilters(Texture::Point, Texture::Point);
	}

	render.GetDevice()->SetVertexBuffer(0, fonts.vbuffer);

	fonts.fntProg->PS_SetTexture("diffuseMap", tex);
	fonts.fntProg->PS_SetVector("color", (Vector4*)&color, 1);

	float scr_x = 0;

	scr_x += 0.5f;

	float scr_y = 0;

	scr_y += 1.0f;

	scr_y += 2.5f;

	int dr_index = 0;
	
	FontService::FontVertex* v = (FontService::FontVertex*)fonts.vbuffer->Lock();

	int w = 0;
	int bytes = 0;	

	int line = -1;
	int x_offset = 0;

	int next_white_space = 0;

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
		
		Glyph* set_glyph = CheckGlyph(w);
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
		
			v[dr_index * 6 + 0].uv = Vector2( char_u, char_v + char_dv);
			v[dr_index * 6 + 1].uv = Vector2( char_u, char_v);
			v[dr_index * 6 + 2].uv = Vector2( char_u + char_du, char_v);

			v[dr_index * 6 + 3].uv = Vector2( char_u, char_v + char_dv);
			v[dr_index * 6 + 4].uv = Vector2( char_u + char_du, char_v);
			v[dr_index * 6 + 5].uv = Vector2( char_u + char_du, char_v + char_dv);
			
			dr_index++;
		}
		
		scr_x += set_glyph->x_advance * font_scale;

		if (dr_index >= 998)
		{
			fonts.vbuffer->Unlock();

			if (need_update_tex) UpdateTexture();

			render.GetDevice()->Draw(Device::TrianglesList, 0, 2 * dr_index);

			dr_index = 0;

			v = (FontService::FontVertex*)fonts.vbuffer->Lock();
		}
	}
	
	fonts.vbuffer->Unlock();

	if (dr_index > 0)
	{
		if (need_update_tex) UpdateTexture();

		render.GetDevice()->Draw(Device::TrianglesList, 0, 2 * dr_index);
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

FontRes::Glyph* FontRes::GetCharGlyph(int ch)
{
	return CheckGlyph(ch);
}

void FontRes::UpdateTexture()
{
	need_update_tex = false;

	tex->Update(0, 0, tex_buffer, tex_w * 2);
}

void FontRes::Release()
{
	if (face)
	{
		FT_Done_Face(face);
	}

	if (tex_buffer)
	{
		free(tex_buffer);
	}

	RELEASE(tex)

	delete this;
}

