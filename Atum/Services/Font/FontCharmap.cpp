
#include "FontCharmap.h"

FontCharmap::FontCharmap()
{
	ftFace = 0;
}

FontCharmap::~FontCharmap()
{
	charMap.clear();
}

void FontCharmap::Init(FT_Face face)
{
	ftFace = face;

	if (!ftFace->charmap)
	{
		if (!ftFace->num_charmaps)
		{
			return;
		}

		FT_Set_Charmap(ftFace, ftFace->charmaps[0]);
	}

	ftEncoding = ftFace->charmap->encoding;

	for(unsigned int i = 0; i < FontCharmap::MAX_PRECOMPUTED; i++)
	{
		charIndexCache[i] = FT_Get_Char_Index(ftFace, i);
	}
}

int FontCharmap::GlyphListIndex(const unsigned int characterCode)
{
	return charMap.find(characterCode);
}

int FontCharmap::FontIndex(const unsigned int characterCode)
{
	if(characterCode < FontCharmap::MAX_PRECOMPUTED)
	{
		return charIndexCache[characterCode];
	}

	return FT_Get_Char_Index(ftFace, characterCode);
}


void FontCharmap::InsertIndex(const unsigned int characterCode,
							const size_t containerIndex)
{
	charMap.insert(characterCode, static_cast<FontCharToGlyphIndexMap::GlyphIndex>(containerIndex));
}