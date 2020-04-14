#include "FreeCamera.h"
#include "Services/Controls/Controls.h"
#include "Services/Render/Render.h"
#include "SceneObjects/2D/Sprite.h"
#include "Services/Core/Core.h"
#include "Editor.h"

void FreeCamera::Init()
{
	proj.BuildProjection(45.0f * Math::Radian, 600.0f / 800.0f, 1.0f, 1000.0f);

	angles = Vector2(0.0f, -0.5f);
	pos = Vector3(0.0f, 6.0f, 0.0f);

	alias_forward = core.controls.GetAlias("FreeCamera.MOVE_FORWARD");
	alias_strafe = core.controls.GetAlias("FreeCamera.MOVE_STRAFE");
	alias_fast = core.controls.GetAlias("FreeCamera.MOVE_FAST");
	alias_move2d_active = core.controls.GetAlias("FreeCamera.MOVE2D_ACTIVE");
	alias_move2d_zoom = core.controls.GetAlias("FreeCamera.MOVE2D_ZOOM");
	alias_rotate_active = core.controls.GetAlias("FreeCamera.ROTATE_ACTIVE");
	alias_rotate_x = core.controls.GetAlias("FreeCamera.ROTATE_X");
	alias_rotate_y = core.controls.GetAlias("FreeCamera.ROTATE_Y");
	alias_reset_view = core.controls.GetAlias("FreeCamera.RESET_VIEW");
}

void FreeCamera::Update(float dt)
{
	if (mode_2d)
	{
		if (core.controls.GetAliasState(alias_move2d_active, Controls::Active))
		{
			auto scale = 1.0f / Sprite::ed_cam_zoom;
			Sprite::ed_cam_pos.x -= core.controls.GetAliasValue(alias_rotate_x, true) * scale;
			Sprite::ed_cam_pos.y -= core.controls.GetAliasValue(alias_rotate_y, true) * scale;
		}

		if (editor.viewport->IsHowered())
		{
			prev_ed_zoom = Sprite::ed_cam_zoom;
			Sprite::ed_cam_zoom += core.controls.GetAliasValue(alias_move2d_zoom, true) * 0.025f;
			Sprite::ed_cam_zoom = Math::Clamp(Sprite::ed_cam_zoom, 0.2f, 2.0f);
		}

		return;
	}

	if (core.controls.GetAliasState(alias_reset_view))
	{
		angles = Vector2(0.0f, -0.5f);
		pos = Vector3(0.0f, 6.0f, 0.0f);
	}

	if (core.controls.GetAliasState(alias_rotate_active, Controls::Active))
	{
		angles.x -= core.controls.GetAliasValue(alias_rotate_x, true) * 0.01f;
		angles.y -= core.controls.GetAliasValue(alias_rotate_y, true) * 0.01f;

		if (angles.y > Math::HalfPI)
		{
			angles.y = Math::HalfPI;
		}

		if (angles.y < -Math::HalfPI)
		{
			angles.y = -Math::HalfPI;
		}
	}

	float forward = core.controls.GetAliasValue(alias_forward, false);
	float strafe = core.controls.GetAliasValue(alias_strafe, false);
	float fast = core.controls.GetAliasValue(alias_fast, false);

	float speed = (100.0f + 350.0f * fast) * dt;

	Vector3 dir = Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x));
	pos += dir * speed * forward;
	
	Vector3 dir_strafe = Vector3(dir.z, 0,-dir.x);
	pos += dir_strafe * speed * strafe;

	view.BuildView(pos, pos + Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x)), Vector3(0, 1, 0));

	core.render.SetTransform(Render::View, view);

	proj.BuildProjection(45.0f * Math::Radian, (float)core.render.GetDevice()->GetHeight() / (float)core.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
	core.render.SetTransform(Render::Projection, proj);
}