#include "FontService.h"

FontService fonts;

FontService::FontService()
{
}

bool FontService::Init()
{
	return true;
}

Font* FontService::CreateFont(const char* file_name, bool is_bold, bool is_italic, int height)
{
	if (!vbuffer)
	{
		fntProg = new FontProgram();

		fntProg->Init();

		vbuffer = render.GetDevice()->CreateBuffer(6 * 1000, sizeof(FontService::FontVertex));

		vbuffer->StartDesc();

		vbuffer->AddDescElem(GeometryBuffer::position, GeometryBuffer::float3, "position");
		vbuffer->AddDescElem(GeometryBuffer::texcoord, GeometryBuffer::float2, "texCoord");

		vbuffer->EndDesc();
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

void FontService::DeleteRes(FontRes* res)
{
	//fonts.Delete(res,false);
}

void FontService::Release()
{
}

