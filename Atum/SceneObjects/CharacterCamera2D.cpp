
#include "CharacterCamera2D.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, CharacterCamera2D, "CharacterCamera2D")

META_DATA_DESC(CharacterCamera2D)
BASE_SCENE_OBJ_NAME_PROP(CharacterCamera2D)
BASE_SCENE_OBJ_STATE_PROP(CharacterCamera2D)
FLOAT_PROP(CharacterCamera2D, trans.pos.x, 100.0f, "Geometry", "x")
FLOAT_PROP(CharacterCamera2D, trans.pos.y, 100.0f, "Geometry", "y")
FLOAT_PROP(CharacterCamera2D, screen_border, 200.0f, "Prop", "ScreenBorder")
FLOAT_PROP(CharacterCamera2D, screen_vert_border, 100.0f, "Prop", "ScreenVertBorder")

BOOL_PROP(CharacterCamera2D, use_lr_borders, false, "Prop", "UseLRBorder")
FLOAT_PROP(CharacterCamera2D, left_border, -300.0f, "Prop", "LeftBorder")
FLOAT_PROP(CharacterCamera2D, right_border, 300.0f, "Prop", "RightBorder")
META_DATA_DESC_END()

CharacterCamera2D::CharacterCamera2D() : SceneObject()
{
}

CharacterCamera2D::~CharacterCamera2D()
{
}

void CharacterCamera2D::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(CharacterCamera2D)
	scripts.engine->RegisterObjectProperty(script_class_name, "float target_pos_x", memberOFFSET(CharacterCamera2D, target_pos.x));
	scripts.engine->RegisterObjectProperty(script_class_name, "float target_pos_y", memberOFFSET(CharacterCamera2D, target_pos.y));
	scripts.engine->RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(CharacterCamera2D, Reset), asCALL_GENERIC);
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
		if (use_lr_borders)
		{
			if (target_pos.x < left_border)
			{
				target_pos.x = left_border;
			}

			if (right_border < target_pos.x)
			{
				target_pos.x = right_border;
			}
		}

		float half_screen = (float)render.GetDevice()->GetWidth() / scale * 0.5f;
		float margin = 50.0f;
		float border = half_screen - screen_border;

		if (target_pos.x < trans.pos.x - screen_border)
		{
			trans.pos.x = target_pos.x + screen_border;
		}

		if (trans.pos.x + screen_border < target_pos.x)
		{
			trans.pos.x = target_pos.x - screen_border;
		}

		float half_vert_screen = (float)render.GetDevice()->GetHeight() / scale * 0.5f;
		float border_vert = half_vert_screen - screen_vert_border;

		if (target_pos.y < trans.pos.y - border_vert)
		{
			trans.pos.y = target_pos.y + border_vert;
		}

		if (trans.pos.y + border_vert < target_pos.y)
		{
			trans.pos.y = target_pos.y - border_vert;
		}

		if (use_lr_borders)
		{
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

bool CharacterCamera2D::Play()
{
	init_pos = trans.pos;
	target_pos = init_pos;
	Sprite::use_ed_cam = false;

	return true;
}

void CharacterCamera2D::Stop()
{
	SceneObject::Stop();

	Sprite::use_ed_cam = true;
}

void CharacterCamera2D::Reset()
{
	trans.pos = init_pos;
}

#ifdef EDITOR
void CharacterCamera2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		trans.size = 100.0f;
		Gizmo::inst->SetTrans2D(&trans, Gizmo::trans_2d_move);
	}
}
#endif