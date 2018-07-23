#pragma once

#include "FontRes.h"

class Font
{
	FontRes* res;

public: 

	Font(FontRes* res);

	float GetLineBreak(std::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);
	void Print(std::vector<FontRes::LineBreak>& line_breaks, Matrix& transform, float font_scale, Color color, const char* text);
	void Print(Matrix& transform, float font_scale, Color color,  const char* text);

	virtual void Release();
};
