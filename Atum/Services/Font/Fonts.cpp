#include "Fonts.h"
#include "Services/Core/Core.h"

CLASSREGEX(Program, FontProgram, Fonts::FontProgram, "FontProgram")
CLASSREGEX_END(Program, FontProgram)

void Fonts::FontProgram::ApplyStates()
{
	core.render.GetDevice()->SetAlphaBlend(true);
	core.render.GetDevice()->SetDepthWriting(false);
	core.render.GetDevice()->SetDepthTest(false);
};

Fonts::Fonts()
{
}

bool Fonts::Init()
{
	return true;
}

Font* Fonts::LoadFont(const char* file_name, bool is_bold, bool is_italic, int height)
{
	if (!vbuffer)
	{
		fntProg = core.render.GetProgram("FontProgram");

		vbuffer = core.render.GetDevice()->CreateBuffer(6 * 1000, sizeof(Fonts::FontVertex));

		VertexDecl::ElemDesc desc[] = {{ VertexDecl::Float3, VertexDecl::Position, 0 }, { VertexDecl::Float2, VertexDecl::Texcoord, 0 }};
		vdecl = core.render.GetDevice()->CreateVertexDecl(2, desc);
	}

	if (!file_name[0]) return nullptr;

	if (height > 100)
	{
		height = 100;
	}

	char nm[512];
	char font_path[128];
	font_path[0] = 0;

	FontRes* res = nullptr;

	const char* prefix = "";

	if (is_bold && !is_italic)
	{
		prefix = "_b";
	}
	else
	if (!is_bold && is_italic)
	{
		prefix = "_i";
	}
	else
	if (is_bold && is_italic)
	{
		prefix = "_bi";
	}

	StringUtils::Printf(font_path, 128, "%s%s", file_name, prefix);
	StringUtils::Printf(nm, 512, "%s%i%s", font_path, height, prefix);

	if (fonts.count(nm) > 0)
	{
		res = fonts[nm];
	}

	file_name = font_path;

	if (!res)
	{
		res = new FontRes(file_name, height);

		if (!res->Load())
		{
			res->Release();

			return nullptr;
		}

		fonts[nm] = res;
	}

	return res->CreateReference();
}

void Fonts::DeleteRes(FontRes* res)
{
	//fonts.Delete(res,false);
}

void Fonts::Release()
{
}

