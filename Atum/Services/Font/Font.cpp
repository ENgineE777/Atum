#include "Font.h"

Font::Font(FontRes* set_res)
{
	res = set_res;
}

void Font::TextOut(Matrix& transform, float font_scale, Color color, const char* text)
{
	if (!text[0]) return;

	res->Print(transform, font_scale, color, text);
}

void Font::Release()
{
	//res->DecRef();
	delete this;
}