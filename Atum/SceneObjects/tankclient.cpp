#include "TankClient.h"
#include "Services/Controls/Controls.h"

CLASSDECLDECL(SceneObject, TankClient)

META_DATA_DESC(TankClient)
META_DATA_DESC_END()

void TankClient::Init()
{
	angles.y = -HALF_PI;

	alias_forward = controls.GetAlias("MOVE_FORWARD");
	alias_strafe = controls.GetAlias("MOVE_STRAFE");
	alias_fast = controls.GetAlias("MOVE_FAST");
	alias_rotate_active = controls.GetAlias("ROTATE_ACTIVE");
	alias_rotate_x = controls.GetAlias("ROTATE_X");
	alias_rotate_y = controls.GetAlias("ROTATE_Y");

	hover_model.LoadModelMS3D("Media//tank_base.ms3d");

	hover_drawer = new Model::Drawer;
	hover_drawer->Init(&hover_model, RenderTasks());

	Vector4 color(0.0f, 1.0f, 1.0f);
	hover_drawer->SetColor(color);

	tower_model.LoadModelMS3D("Media//tank_tower.ms3d");

	tower_drawer = new Model::Drawer;
	tower_drawer->Init(&tower_model, RenderTasks());

	tower_drawer->SetColor(color);

	gun_model.LoadModelMS3D("Media//tank_gun.ms3d");

	gun_drawer = new Model::Drawer;
	gun_drawer->Init(&gun_model, RenderTasks());

	gun_drawer->SetColor(color);

	Tasks()->AddTask(0, this, (Object::Delegate)&TankClient::Update);
}

void TankClient::Play()
{
	Scene::Group& group = owner->GetGroup("Tank");

	if (group.objects.size() > 0)
	{
		state = &((Tank*)group.objects[0])->state;
	}
}

void TankClient::Stop()
{
	hover_drawer->Show(true);
	tower_drawer->Show(true);
	gun_drawer->Show(true);
}

void TankClient::Update(float dt)
{
	if (!Playing())
	{
		hover_drawer->SetTransform(transform);
		tower_drawer->SetTransform(transform);
		gun_drawer->SetTransform(transform);

		return;
	}

	if (controls.GetAliasState(alias_rotate_active, false, Controls::Active))
	{
		angles.x -= controls.GetAliasValue(alias_rotate_x, true) * 0.01f;

		if (angles.y > HALF_PI)
		{
			angles.y = HALF_PI;
		}

		if (angles.y < -HALF_PI)
		{
			angles.y = -HALF_PI;
		}
	}

	float speed = 25.0f;

	Matrix mat;
	mat.RotateY(state->angle);
	mat.Pos() = state->pos;


	view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 55, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));
	proj.BuildProjection(45.0f * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 1.0f, 1000.0f);

	render.SetTransform(Render::View, view);
	render.SetTransform(Render::Projection, proj);

	float tower_angel = state->angle;

	Vector target_pt = 0.0f;

	PhysScene::RaycastDesc rcdesc;

	{
		Vector2 screepos = Vector2((float)controls.GetAliasValue(alias_rotate_x, false) / (float)render.GetDevice()->GetWidth(),
									(float)controls.GetAliasValue(alias_rotate_y, false) / (float)render.GetDevice()->GetHeight());

		Vector v;
		v.x = (2.0f * screepos.x - 1) / proj._11;
		v.y = -(2.0f * screepos.y - 1) / proj._22;
		v.z = 1.0f;

		Matrix inv_view = view;
		inv_view.InverseComplette();

		Vector camPos = inv_view.Pos();

		Vector dir;
		dir.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		dir.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		dir.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		
		Vector screen = camPos + dir;
		dir.Normalize();

		rcdesc.origin = camPos;
		rcdesc.dir = dir;
		rcdesc.length = 500;

		if (PScene()->RayCast(rcdesc))
		{
			target_pt = rcdesc.hitPos;
			render.DebugSphere(target_pt, COLOR_RED, 0.5f);

			dir = target_pt - mat.Pos();
			dir.Normalize();

			tower_angel = acosf(dir.Dot(Vector(1,0,0)));

			if (dir.Dot(Vector(0, 0, 1)) > 0.0f)
			{
				tower_angel = PI * 2 - tower_angel;
			}
		}
	}

	Vector dir = mat.Vx();
	dir.y = 0;
	dir.Normalize();

	if (controls.DebugKeyPressed("KEY_W", Controls::Active))
	{
		state->up = 1;
	}
	else
	if (controls.DebugKeyPressed("KEY_S", Controls::Active))
	{
		state->up = -1;
	}
	else
	{
		state->up = 0;
	}

	if (controls.DebugKeyPressed("KEY_D", Controls::Active))
	{
		state->rotate = 1;
	}
	else
	if (controls.DebugKeyPressed("KEY_A", Controls::Active))
	{
		state->rotate = -1;
	}
	else
	{
		state->rotate = 0;
	}

	state->fired = controls.DebugKeyPressed("MS_BTN0", Controls::Active);

	{
		float under = 1.0f;

		rcdesc.dir = Vector(0, -1, 0);
		rcdesc.length = under + 2.0f;

		Vector org = mat.Pos();
		org.y += under;

		Vector p1 = org + mat.Vx() * 1.75f;
		rcdesc.origin = p1;

		if (PScene()->RayCast(rcdesc))
		{
			p1 = rcdesc.hitPos;
		}
		else
		{
			p1.y -= under;
		}

		Vector p2 = org - mat.Vx() * 1.75f - mat.Vz();
		rcdesc.origin = p2;

		if (PScene()->RayCast(rcdesc))
		{
			p2 = rcdesc.hitPos;
		}
		else
		{
			p2.y -= under;
		}

		Vector p3 = org - mat.Vx() * 1.75f + mat.Vz();
		rcdesc.origin = p3;

		if (PScene()->RayCast(rcdesc))
		{
			p3 = rcdesc.hitPos;
		}
		else
		{
			p3.y -= under;
		}

		mat.Vz() = p3 - p2;
		mat.Vz().Normalize();

		mat.Vx() = p1 - (p3 + p2)*0.5f;
		mat.Vx().Normalize();

		mat.Vy() = mat.Vz().Cross(mat.Vx());

		render.DebugSphere(p1, COLOR_RED, 0.5f);
		render.DebugSphere(p2, COLOR_RED, 0.5f);
		render.DebugSphere(p3, COLOR_RED, 0.5f);

		Matrix mdl = mat;
		hover_drawer->SetTransform(mdl);

		mdl = Matrix().RotateY(tower_angel - state->angle) * Matrix().Move(hover_model.locator) * mdl;
		tower_drawer->SetTransform(mdl);

		mdl = Matrix().Move(tower_model.locator) * mdl;
		gun_drawer->SetTransform(mdl);

		mdl = Matrix().Move(gun_model.locator) * mdl;

		state->gun_pos = mdl.Pos();
		state->gun_dir = target_pt - mdl.Pos();
		state->gun_dir.Normalize();
	}
}