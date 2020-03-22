#include "EditorDrawer.h"
#include "Services/Render/Render.h"
#include "Services/Core/Core.h"

EditorDrawer editor_drawer;

void EditorDrawer::Init()
{
	anchorn = core.render.LoadTexture("settings\\editor\\gizmo_anch.png");
	anchorn->SetFilters(Texture::FilterType::Point, Texture::FilterType::Point);

	center = core.render.LoadTexture("settings\\editor\\gizmo_center.png");
	center->SetFilters(Texture::FilterType::Point, Texture::FilterType::Point);

	node_tex = core.render.LoadTexture("settings\\editor\\node.png");
	node_tex->SetFilters(Texture::Point, Texture::Point);

	arrow_tex = core.render.LoadTexture("settings\\editor\\arrow.png");
	arrow_tex->SetFilters(Texture::Point, Texture::Point);

	checker_texture = core.render.LoadTexture("settings/editor/checker.png");
	checker_texture->SetFilters(Texture::Point, Texture::Point);

	font = core.fonts.LoadFont("settings\\eui\\DroidSans.ttf", false, false, 11);
}

void EditorDrawer::DrawSprite(Texture* tex, Vector2 pos, Vector2 size, Vector2 offset, float rotate, Color color)
{
	Matrix mat;
	mat.RotateZ(rotate);

	pos = Sprite::MoveToCamera(pos, false);
	mat.Pos() = Vector3(pos.x, pos.y, 0.01f);

	size *= Sprite::ed_cam_zoom;

	if (pos.x + size.x < 0 || core.render.GetDevice()->GetWidth() < pos.x ||
		pos.y + size.y < 0 || core.render.GetDevice()->GetHeight() < pos.y)
	{
		return;
	}

	Sprite::Draw(tex, color, mat, offset * Sprite::ed_cam_zoom, size, 0.0f, 1.0f, false);
}

void EditorDrawer::PrintText(Vector2 pos, Color color, const char* text)
{
	pos = Sprite::MoveToCamera(pos, false);

	if (pos.x + 250 < 0 || core.render.GetDevice()->GetWidth() < pos.x ||
		pos.y + 15 < 0 || core.render.GetDevice()->GetHeight() < pos.y)
	{
		return;
	}

	Matrix mat;
	mat.Pos().x = pos.x;
	mat.Pos().y = pos.y;

	char str[64];
	StringUtils::Copy(str, 64, text);

	if ((int)strlen(str) > 15)
	{
		memcpy(&str[15], "...", 4);
	}

	font->Print(mat, 1.0f, color, str);
}

void EditorDrawer::DrawLine(Vector2 from, Vector2 to, Color color)
{
	from = Sprite::MoveToCamera(from, false);
	to = Sprite::MoveToCamera(to, false);

	Vector2 dir = to - from;
	Vector2 size(dir.Length(), 2.0f);

	Vector2 center = from + dir * 0.5f;

	Matrix mat;
	mat.RotateZ(atan2(dir.y / size.x, dir.x / size.x));
	mat.Pos() = Vector3(center.x, center.y, 0.01f);

	Sprite::Draw(nullptr, color, mat, -0.5f * size, size, 0.0f, 1.0f, false);
}

void EditorDrawer::DrawCurve(Vector2 from, Vector2 to, Color color)
{
	Vector2 last = from;

	float offset = fmin((from - to).Length() * 0.5f, 100.0f);
	Vector2 p2 = Vector2(from.x + offset, from.y);
	Vector2 p3 = Vector2(to.x - offset, to.y);

	uint32_t segments = 20;
	float t_step = 1.0f / (float)segments;

	for (uint32_t i = 1; i <= segments; i++)
	{
		float t = t_step * (float)i;
		float u = 1.0f - t;
		float w1 = u * u*u;
		float w2 = 3 * u*u*t;
		float w3 = 3 * u*t*t;
		float w4 = t * t *t;
		Vector2 pos = w1 * from + w2 * p2 + w3 * p3 + w4 * to;

		DrawLine(last, pos, color);

		last = pos;
	}
}