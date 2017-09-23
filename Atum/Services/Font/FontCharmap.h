#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include "FontCharToGlyphIndexMap.h"

class FontCharmap
{
	public:

	FontCharmap();

	virtual ~FontCharmap();

	void Init(FT_Face face);

	int GlyphListIndex(const unsigned int characterCode);

	int FontIndex(const unsigned int characterCode);

	void InsertIndex(const unsigned int characterCode,
						const size_t containerIndex);

	private:

	FT_Encoding ftEncoding;

	FT_Face ftFace;

	FontCharToGlyphIndexMap charMap;

	static const unsigned int MAX_PRECOMPUTED = 128;
	unsigned int charIndexCache[MAX_PRECOMPUTED];
};