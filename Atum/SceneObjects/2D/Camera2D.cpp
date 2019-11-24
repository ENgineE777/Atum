
#include "Camera2D.h"
#include "Sprite.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, Camera2D, "Camera2D")

META_DATA_DESC(Camera2D)
BASE_SCENE_OBJ_PROP(Camera2D)
FLOAT_PROP(Camera2D, trans.pos.x, 100.0f, "Geometry", "x", "X coordinate of a camera position")
FLOAT_PROP(Camera2D, trans.pos.y, 100.0f, "Geometry", "y", "Y coordinate of a camera position")
FLOAT_PROP(Camera2D, screen_border, 200.0f, "Prop", "ScreenBorder", "Size of border at left and right of the screen where camera is starting to follow object ")
FLOAT_PROP(Camera2D, screen_vert_border, 100.0f, "Prop", "ScreenVertBorder", "Size of border at top and bottom of the screen where camera is starting to follow object")

BOOL_PROP(Camera2D, use_lr_borders, false, "Prop", "UseLRBorder", "This parameter controls useage of left and right border which limits movement of the camera")
FLOAT_PROP(Camera2D, left_border, -300.0f, "Prop", "LeftBorder", "This parameter set up left border after which camera will not follow a target")
FLOAT_PROP(Camera2D, right_border, 300.0f, "Prop", "RightBorder", "This parameter set up right border after which camera will not follow a target")
META_DATA_DESC_END()

Camera2D::Camera2D() : SceneObject()
{
}

Camera2D::~Camera2D()
{
}

void Camera2D::BindClassToScript()
{
	const char* brief = "Representation of camera in 2D space\n"
	"\n"
	"Currently 2D space is a special mode and this class represents a camera in that special 2D mode.\n"
	"Camera auto follow a target. Position of a target should be set via script.\n"
	"\n"
	"This class ::Camera2D is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(Camera2D, brief)
	core.scripts.RegisterObjectProperty(script_class_name, "float target_pos_x", memberOFFSET(Camera2D, target_pos.x), "X coordite of a target position");
	core.scripts.RegisterObjectProperty(script_class_name, "float target_pos_y", memberOFFSET(Camera2D, target_pos.y), "Y coordite of a target position");
	core.scripts.RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(Camera2D, Reset), "Reseting position of camera to intinal position on scene start");
}

void Camera2D::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Camera, this, (Object::Delegate)&Camera2D::Update);
}

void Camera2D::Update(float dt)
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	if (GetScene()->Playing())
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
		Vector2 half_screen = Sprite::ed_cam_pos * Sprite::ed_cam_zoom - Sprite::half_screen;

		core.render.DebugLine2D(Vector2(trans.pos.x, trans.pos.y - 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN,
		                        Vector2(trans.pos.x, trans.pos.y + 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN,
		                        Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN,
		                        Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y - 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN,
		                        Vector2(trans.pos.x + screen_border, trans.pos.y - 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN);
		core.render.DebugLine2D(Vector2(trans.pos.x - screen_border, trans.pos.y + 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN,
		                        Vector2(trans.pos.x + screen_border, trans.pos.y + 512.0f) * scale * Sprite::ed_cam_zoom - half_screen, COLOR_GREEN);
	}
}

bool Camera2D::Play()
{
	init_pos = trans.pos;
	target_pos = init_pos;

	return true;
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