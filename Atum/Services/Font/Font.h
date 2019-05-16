#pragma once

#include "FontRes.h"

/**
\ingroup gr_code_services_font
*/

/**
\brief Font

This is a inctance of font. Instance allow to draw text with selected font.

*/

class Font
{
#ifndef DOXYGEN_SKIP
	FontRes* res;
#endif

public: 

#ifndef DOXYGEN_SKIP
	Font(FontRes* res);
#endif

	/**
	\brief Gather a info line breaks in a particula text

	\param[out] line_breaks Array of postions of line breakes
	\param[in] text Context of a text
	\param[in] width Maximum width of line
	*/
	float GetLineBreak(std::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);

	/**
	\brief Print a text with precalculated postions of line breaks

	\param[out] line_breaks Array of postions of line breakes
	\param[in] transform Transformation of a position
	\param[in] font_scale Scale of a font during rendering of a text
	\param[in] color Color of a font during rendering of a text
	\param[in] text Context of a text

	*/
	void Print(std::vector<FontRes::LineBreak>& line_breaks, Matrix& transform, float font_scale, Color color, const char* text);

	/**
	\brief Print a text

	\param[in] transform Transformation of a position
	\param[in] font_scale Scale of a font during rendering of a text
	\param[in] color Color of a font during rendering of a text
	\param[in] text Context of a text

	*/
	void Print(Matrix& transform, float font_scale, Color color,  const char* text);

	/**
	\brief This method needed to be call after instance of a font is no longer required
	*/
	virtual void Release();
};
