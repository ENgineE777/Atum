#include "DebugFont.h"
#include "Services/Core/Core.h"

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

	font = core.fonts.LoadFont("settings/helvetica", false, false, font_size);

	return true;
}

void DebugFont::AddText(std::vector<Text>& texts, Vector3 pos, ScreenCorner corner, float dist, Color color, const char* text)
{
	if (texts.size()>1000) return;

	texts.push_back(Text());

	Text* txt = &texts[texts.size() - 1];

	txt->pos = pos;
	txt->corner = corner;
	txt->dist = 0.0f;
	txt->color = color;
	StringUtils::Copy(txt->text, 256, text);
}

void DebugFont::AddText(Vector2 pos, ScreenCorner corner, Color color, const char* text)
{
	AddText(texts, Vector3(pos.x, pos.y, 0.0f), corner, -1.0f, color, text);
}

void DebugFont::AddText(Vector3 pos, float dist, Color color, const char* text)
{
	AddText(textsIn3d, pos, LeftTop, dist, color, text);
}

void DebugFont::Draw(float dt)
{
	Matrix mat;

	for (int i=0;i<textsIn3d.size();i++)
	{
		Text* txt = &textsIn3d[i];

		Vector3 pos = core.render.TransformToScreen(txt->pos, 1);

		if (pos.z>0 && pos.z<txt->dist)
		{
			mat.Pos() = Vector3(pos.x * core.render.GetDevice()->GetWidth(), pos.y * core.render.GetDevice()->GetHeight(), 0);
			font->Print(mat, 1.0f, txt->color, txt->text);
		}
	}

	textsIn3d.clear();

	std::vector<FontRes::LineBreak> line_breaks;
	mat.Pos().z = 0.0f;

	Vector2 screen = Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight());

	for (int i=0;i<texts.size();i++)
	{
		Text* txt = &texts[i];

		font->GetLineBreak(line_breaks, txt->text, 1000);

		mat.Pos().x = (txt->corner == ScreenCorner::LeftTop || txt->corner == ScreenCorner::LeftBottom) ? (txt->pos.x + 1) : (screen.x - txt->pos.x - line_breaks[0].width);
		mat.Pos().y = (txt->corner == ScreenCorner::LeftTop || txt->corner == ScreenCorner::RightTop) ? (txt->pos.y + 1) : (screen.y - txt->pos.y - font_size);
		font->Print(mat, 1.0f, COLOR_BLACK, txt->text);

		mat.Pos() -= 1.0f;
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