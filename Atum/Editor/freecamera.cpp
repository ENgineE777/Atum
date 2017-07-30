#include "FreeCamera.h"
#include "Services/Controls/Controls.h"
#include "Services/Render/Render.h"

void FreeCamera::Init()
{
	proj.BuildProjection(45.0f * RADIAN, 600.0f / 800.0f, 1.0f, 1000.0f);

	angles = Vector2(0.0f, -0.5f);
	pos = Vector(0.0f, 6.0f, 0.0f);

	alias_forward = controls.GetAlias("MOVE_FORWARD");
	alias_strafe = controls.GetAlias("MOVE_STRAFE");
	alias_fast = controls.GetAlias("MOVE_FAST");
	alias_rotate_active = controls.GetAlias("ROTATE_ACTIVE");
	alias_rotate_x = controls.GetAlias("ROTATE_X");
	alias_rotate_y = controls.GetAlias("ROTATE_Y");
}

void FreeCamera::Update(float dt)
{
	if (controls.GetAliasState(alias_rotate_active, false, Controls::Active))
	{
		angles.x -= controls.GetAliasValue(alias_rotate_x, true) * 0.01f;
		angles.y -= controls.GetAliasValue(alias_rotate_y, true) * 0.01f;

		if (angles.y > HALF_PI)
		{
			angles.y = HALF_PI;
		}

		if (angles.y < -HALF_PI)
		{
			angles.y = -HALF_PI;
		}
	}

	float forward = controls.GetAliasValue(alias_forward, false);
	float strafe = controls.GetAliasValue(alias_strafe, false);
	float fast = controls.GetAliasValue(alias_fast, false);

	float speed = (3.0f + 12.0f * fast) * dt;

	Vector dir = Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x));
	pos += dir * speed * forward;
	
	Vector dir_strafe = Vector(dir.z, 0,-dir.x);
	pos += dir_strafe * speed * strafe;

	view.BuildView(pos, pos + Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)), Vector(0, 1, 0));

	render.SetTransform(Render::View, view);

	proj.BuildProjection(45.0f * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
	render.SetTransform(Render::Projection, proj);
}