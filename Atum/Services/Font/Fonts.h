#pragma once

#include "Font.h"
#include <map>

/**
\ingroup gr_code_services_font
*/

/**
\brief Fonts

This is the manager of fonts. System supports TrueType fonts. 

*/

class Fonts
{
#ifndef DOXYGEN_SKIP
	friend class FontRes;
	std::map<std::string, FontRes*> fonts;

	struct FontVertex
	{
		Vector pos;
		Vector2 uv;
	};

	Program*        fntProg = nullptr;
	DataBuffer* vbuffer = nullptr;
	VertexDecl*     vdecl = nullptr;
#endif

public: 

	/**
	\brief Load a font

	\param[in] file_name Full path to a file with TrueType font
	\param[in] is_bold Should a font has bold style
	\param[in] is_italic Should a font has italic style
	\param[in] height Hieght of font iin pixels

	\return Instance of a font. Nullptr will returned if it is impossible to load a font.
	*/
	virtual Font* LoadFont(const char* file_name, bool is_bold, bool is_italic, int height);

#ifndef DOXYGEN_SKIP
	class FontProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "font_vs.shd"; };
		virtual const char* GetPsName() { return "font_ps.shd"; };

		virtual void ApplyStates();
	};

	Fonts();

	virtual bool Init();

	virtual void DeleteRes(FontRes* res);
	virtual void Release();
#endif
};