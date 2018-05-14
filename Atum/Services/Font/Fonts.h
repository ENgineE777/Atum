#pragma once

#include "Font.h"
#include "FontRes.h"
#include <map>

class Fonts
{
	friend class FontRes;
	std::map<std::string, FontRes*> fonts;

	struct FontVertex
	{
		Vector pos;
		Vector2 uv;
	};

	class FontProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "font_vs.shd"; };
		virtual const char* GetPsName() { return "font_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetAlphaBlend(true);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(false);
		};

		CLASSDECLDIF(Program, FontProgram)
	};

	Program*        fntProg = nullptr;
	GeometryBuffer* vbuffer = nullptr;
	VertexDecl*     vdecl = nullptr;

public: 

	Fonts();
	virtual bool Init();
	virtual Font* CreateFont(const char* file_name, bool is_bold, bool is_italic, int height);
	virtual void DeleteRes(FontRes* res);
	virtual void Release();
};

extern Fonts fonts;