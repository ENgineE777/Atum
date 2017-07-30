#include "FreeCamera.h"
#include "Services/Controls/Controls.h"

void FreeCamera::Init()
{
	proj.BuildProjection(45.0f * RADIAN, 600.0f / 800.0f, 1.0f, 1000.0f);

	angles = Vector2(0.0f, 0.0f);
	pos = Vector(0.0f, 8.0f, 0.0f);
	//render.AddDelegate("camera", this, (Object::Delegate)&FreeCamera::Update, 0);
}

void FreeCamera::Update(float dt)
{
	if (controls.GetAliasState(1001, false, Controls::Active))
	{
		angles.x -= controls.GetAliasValue(1500, true) * 0.01f;
		angles.y -= controls.GetAliasValue(1501, true) * 0.01f;

		if (angles.y > HALF_PI)
		{
			angles.y = HALF_PI;
		}

		if (angles.y < -HALF_PI)
		{
			angles.y = -HALF_PI;
		}
	}

	Vector dir = Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x));

	float speed = 3.0f;

	if (controls.GetAliasState(DIK_LSHIFT, false, Controls::Active))
	{
		speed = 15.0f;
	}

	if (controls.GetAliasState(DIK_W, false, Controls::Active))
	{
		pos += dir * dt * speed;
	}
	else
	if (controls.GetAliasState(DIK_S, false, Controls::Active))
	{
		pos -= dir * dt * speed;
	}
	else
	{
		Vector dir_strafe = Vector(dir.z, 0,-dir.x);

		if (controls.GetAliasState(DIK_A, false, Controls::Active))
		{
			pos -= dir_strafe * dt * speed;
		}
		else
		if (controls.GetAliasState(DIK_D, false, Controls::Active))
		{
			pos += dir_strafe * dt * speed;
		}
	}

	view.BuildView(pos, pos + Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)), Vector(0, 1, 0));

	render.SetTransform(Render::View, view);
	render.SetTransform(Render::Projection, proj);
}