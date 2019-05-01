
#include "Camera2D.h"
#include "Sprite.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, Camera2D, "Camera2D")

META_DATA_DESC(Camera2D)
BASE_SCENE_OBJ_PROP(Camera2D)
FLOAT_PROP(Camera2D, trans.pos.x, 100.0f, "Geometry", "x")
FLOAT_PROP(Camera2D, trans.pos.y, 100.0f, "Geometry", "y")
FLOAT_PROP(Camera2D, screen_border, 200.0f, "Prop", "ScreenBorder")
FLOAT_PROP(Camera2D, screen_vert_border, 100.0f, "Prop", "ScreenVertBorder")

BOOL_PROP(Camera2D, use_lr_borders, false, "Prop", "UseLRBorder")
FLOAT_PROP(Camera2D, left_border, -300.0f, "Prop", "LeftBorder")
FLOAT_PROP(Camera2D, right_border, 300.0f, "Prop", "RightBorder")
META_DATA_DESC_END()

Camera2D::Camera2D() : SceneObject()
{
}

Camera2D::~Camera2D()
{
}

void Camera2D::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(Camera2D)
	core.scripts.RegisterObjectProperty(script_class_name, "float target_pos_x", memberOFFSET(Camera2D, target_pos.x));
	core.scripts.RegisterObjectProperty(script_class_name, "float target_pos_y", memberOFFSET(Camera2D, target_pos.y));
	core.scripts.RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(Camera2D, Reset));
}

void Camera2D::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Camera, this, (Object::Delegate)&Camera2D::Update);
}

void Camera2D::Update(float dt)
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

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

		float half_screen = (float)core.render.GetDevice()->GetWidth() / scale * 0.5f;
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

		float half_vert_screen = (float)core.render.GetDevice()->GetHeight() / scale * 0.5f;
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
		Vector2 half_screen = Vector2(Sprite::ed_cam_pos.x - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - core.render.GetDevice()->GetHeight() * 0.5f);

		core.render.DebugLine2D(Vector2(trans.pos.x, trans.pos.y - 512.0f) * scale - half_screen, COLOR_GREEN, Vector2(trans.pos.x, trans.pos.y + 512.0f) * scale - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale - half_screen, COLOR_GREEN, Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale - half_screen, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale - half_screen, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale - half_screen, COLOR_GREEN, Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale - half_screen, COLOR_GREEN);
	}
}

bool Camera2D::Play()
{
	init_pos = trans.pos;
	target_pos = init_pos;
	Sprite::use_ed_cam = false;

	return true;
}

void Camera2D::Stop()
{
	SceneObject::Stop();

	Sprite::use_ed_cam = true;
}

void Camera2D::Reset()
{
	trans.pos = init_pos;
}

#ifdef EDITOR
void Camera2D::Set2DPos(Vector2 pos)
{
	trans.pos = pos;
}

void Camera2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		trans.size = 100.0f;
		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(trans), Gizmo::trans_2d_move);
	}
}
#endif