
#include "CharacterCamera2D.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, CharacterCamera2D, "CharacterCamera2D")

META_DATA_DESC(CharacterCamera2D)
FLOAT_PROP(CharacterCamera2D, trans.pos.x, 100.0f, "Geometry", "PosX")
FLOAT_PROP(CharacterCamera2D, trans.pos.y, 100.0f, "Geometry", "PosY")
STRING_PROP(CharacterCamera2D, target_name, "", "Prop", "Target")
FLOAT_PROP(CharacterCamera2D, screen_border, 100.0f, "Prop", "ScreenBorder")
FLOAT_PROP(CharacterCamera2D, left_border, -300.0f, "Prop", "LeftBorder")
FLOAT_PROP(CharacterCamera2D, right_border, 300.0f, "Prop", "RightBorder")
META_DATA_DESC_END()

CharacterCamera2D::CharacterCamera2D() : SceneObject()
{
}

CharacterCamera2D::~CharacterCamera2D()
{
}

void CharacterCamera2D::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Camera, this, (Object::Delegate)&CharacterCamera2D::Update);
}

void CharacterCamera2D::Update(float dt)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	if (owner->Playing())
	{
		if (target)
		{
			if (target->trans.pos.x < left_border)
			{
				target->trans.pos.x = left_border;
			}

			if (right_border < target->trans.pos.x)
			{
				target->trans.pos.x = right_border;
			}

			float half_screen = (float)render.GetDevice()->GetWidth() / scale * 0.5f;
			float margin = 50.0f;
			float border = half_screen - screen_border;

			if (target->trans.pos.x < trans.pos.x - border)
			{
				trans.pos.x = target->trans.pos.x + border;
			}

			if (trans.pos.x + border < target->trans.pos.x)
			{
				trans.pos.x = target->trans.pos.x - border;
			}

			half_screen -= margin;

			if (trans.pos.x < left_border + half_screen)
			{
				trans.pos.x = left_border + half_screen;
			}

			if (right_border - half_screen < trans.pos.x)
			{
				trans.pos.x = right_border - half_screen;
			}
		}

		Sprite::cam_pos = trans.pos;
	}
	else
	{
		render.DebugLine2D(Vector2(trans.pos.x, trans.pos.y - 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN, Vector2(trans.pos.x, trans.pos.y + 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN);
		render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN, Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN);
		render.DebugLine2D(Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN);
		render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN);
		render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale - Sprite::ed_cam_pos, COLOR_GREEN);
	}
}

void CharacterCamera2D::Play()
{
	SceneObject::Play();

	Sprite::use_ed_cam = false;
	target = (SimpleCharacter2D*)owner->FindInGroup("SimpleCharacter2D", target_name.c_str());
}

void CharacterCamera2D::Stop()
{
	SceneObject::Stop();

	Sprite::use_ed_cam = true;
	target = nullptr;
}

#ifdef EDITOR
void CharacterCamera2D::SetEditMode(bool ed)
{
	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
	}
}
#endif