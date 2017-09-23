#pragma once

#include "Font.h"
#include "FontRes.h"
#include <map>

class FontService
{
	friend class FontRes;
	FT_Library library;
	std::map<std::string, FontRes*> fonts;

	struct FontVertex
	{
		Vector pos;
		Vector2 uv;
	};

	class FontProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//font_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//font_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(true);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(false);
		};
	};

	FontProgram*    fntProg = nullptr;
	GeometryBuffer* vbuffer = nullptr;

public: 

	FontService();
	virtual bool Init();
	virtual Font* CreateFont(const char* file_name, bool is_bold, bool is_italic, int height, float outline_thin);
	virtual void DeleteRes(FontRes* res);
	virtual void Release();
};

extern FontService fonts;