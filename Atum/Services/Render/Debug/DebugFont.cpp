#include "DebugFont.h"
#include "Services/Font/Fonts.h"

DebugFont::DebugFont()
{
	font = nullptr;
	texts.reserve(32);
}

DebugFont::~DebugFont()
{
}

bool DebugFont::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugFont::Draw);

	font = fonts.LoadFont("settings/helvetica", false, false, 16);

	return true;
}

void DebugFont::AddText(std::vector<Text>& texts, Vector pos, float dist, Color color, const char* text)
{
	if (texts.size()>1000) return;

	texts.push_back(Text());

	Text* txt = &texts[texts.size() - 1];

	txt->pos = pos;
	txt->dist = 0.0f;
	txt->color = color;
	StringUtils::Copy(txt->text, 256, text);
}

void DebugFont::AddText(Vector2 pos, Color color, const char* text)
{
	AddText(texts, Vector(pos.x, pos.y, 0.0f), -1.0f, color, text);
}

void DebugFont::AddText(Vector pos, float dist, Color color, const char* text)
{
	AddText(textsIn3d, pos, dist, color, text);
}

void DebugFont::Draw(float dt)
{
	Matrix mat;

	for (int i=0;i<textsIn3d.size();i++)
	{
		Text* txt = &textsIn3d[i];

		Vector pos = render.TransformToScreen(txt->pos, 1);

		if (pos.z>0 && pos.z<txt->dist)
		{
			mat.Pos() = Vector(pos.x * render.GetDevice()->GetWidth(), pos.y * render.GetDevice()->GetHeight(), 0);
			font->Print(mat, 1.0f, txt->color, txt->text);
		}
	}

	textsIn3d.clear();

	for (int i=0;i<texts.size();i++)
	{
		Text* txt = &texts[i];

		mat.Pos() = Vector(txt->pos.x, txt->pos.y, 0);
		font->Print(mat, 1.0f, txt->color, txt->text);
	}

	texts.clear();
}

void DebugFont::Release()
{
	if (font)
	{
		RELEASE(font)
	}
}