#pragma once

#include "FontRes.h"

class Font
{
	FontRes* res;

public: 

	Font(FontRes* res);

	void Print(Matrix& transform, float font_scale, Color color,  const char* text);

	virtual void Release();
};
