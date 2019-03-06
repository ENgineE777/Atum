#include "tankclient.h"

CLASSREG(SceneObject, TankClient, "TankClient")

META_DATA_DESC(TankClient)
BASE_SCENE_OBJ_PROP(TankClient)
FILENAME_PROP(TankClient, base_model_name, "", "Prop", "BaseModel")
FILENAME_PROP(TankClient, tower_model_name, "", "Prop", "TowerModel")
FILENAME_PROP(TankClient, gun_model_name, "", "Prop", "GunModel")
STRING_PROP(TankClient, vjoy_name, "", "Prop", "VJoy")
META_DATA_DESC_END()

void TankClient::Init()
{
	owner->AddToGroup(this, "TankClient");

	angles.y = -HALF_PI;

	alias_forward = core.controls.GetAlias("Tank.MOVE_FORWARD");
	alias_strafe = core.controls.GetAlias("Tank.MOVE_STRAFE");
	alias_rotate_active = core.controls.GetAlias("Tank.ROTATE_ACTIVE");
	alias_rotate_x = core.controls.GetAlias("Tank.ROTATE_X");
	alias_rotate_y = core.controls.GetAlias("Tank.ROTATE_Y");
	alias_fire = core.controls.GetAlias("Tank.FIRE");

	Tasks(false)->AddTask(0, this, (Object::Delegate)&TankClient::Update);
}

bool TankClient::Is3DObject()
{
	return true;
}

void TankClient::ApplyProperties()
{
	base_model.LoadModelMS3D(base_model_name.c_str());
	tower_model.LoadModelMS3D(tower_model_name.c_str());
	gun_model.LoadModelMS3D(gun_model_name.c_str());

	vjoy = (VirtualJoystick*)owner->FindByName(vjoy_name.c_str(), false);
}

bool TankClient::Play()
{
	return true;
}

void TankClient::Stop()
{
}

void TankClient::SendClientState(float dt)
{

}

void TankClient::AddIsntance(int id, bool is_contralable)
{
	Instance inst;
	
	inst.id = id;
	inst.is_contralable = is_contralable;

	inst.base_drawer = new Model::Drawer;
	inst.base_drawer->Init(&base_model, RenderTasks(false));

	inst.tower_drawer = new Model::Drawer;
	inst.tower_drawer->Init(&tower_model, RenderTasks(false));

	inst.gun_drawer = new Model::Drawer;
	inst.gun_drawer->Init(&gun_model, RenderTasks(false));

	instances.push_back(inst);
}

void TankClient::Update(float dt)
{
	if (!Playing())
	{
		return;
	}

	for (auto& inst : instances)
	{
		Matrix mat;
		mat.RotateY(inst.serverState.angle);
		mat.Pos() = inst.serverState.pos;

		Vector target_pt = 0.0f;
		PhysScene::RaycastDesc rcdesc;

		if (inst.is_contralable)
		{
			/*if (core.controls.GetAliasState(alias_rotate_active, Controls::Active))
			{
				angles.x -= core.controls.GetAliasValue(alias_rotate_x, true);

				if (angles.y > HALF_PI)
				{
					angles.y = HALF_PI;
				}

				if (angles.y < -HALF_PI)
				{
					angles.y = -HALF_PI;
				}
			}*/

			view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 55, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));
			proj.BuildProjection(45.0f * RADIAN, (float)core.render.GetDevice()->GetHeight() / (float)core.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);

			core.render.SetTransform(Render::View, view);
			core.render.SetTransform(Render::Projection, proj);

			inst.clientState.needed_tower_angel = inst.serverState.angle;

			Vector2 screepos = Vector2((float)core.controls.GetAliasValue(alias_rotate_x, false) / (float)core.render.GetDevice()->GetWidth(),
			                           (float)core.controls.GetAliasValue(alias_rotate_y, false) / (float)core.render.GetDevice()->GetHeight());

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
			dir.Normalize();

			rcdesc.origin = camPos;
			rcdesc.dir = dir;
			rcdesc.length = 500;

			if (PScene()->RayCast(rcdesc))
			{
				target_pt = rcdesc.hitPos;
				core.render.DebugSphere(target_pt, COLOR_RED, 0.5f);

				dir = target_pt - mat.Pos();
				dir.Normalize();

				inst.clientState.needed_tower_angel = acosf(dir.Dot(Vector(1,0,0)));

				if (dir.Dot(Vector(0, 0, 1)) > 0.0f)
				{
					inst.clientState.needed_tower_angel = PI * 2 - inst.clientState.needed_tower_angel;
				}
			}

			if (vjoy)
			{
				inst.clientState.up = 0;

				if (vjoy->stick_asb_delta.y > 0.5f)
				{
					inst.clientState.up = 1;
				}
				else
				if (vjoy->stick_asb_delta.y <-0.5f)
				{
					inst.clientState.up =-1;
				}

				inst.clientState.rotate = 0;

				if (vjoy->stick_asb_delta.x > 0.5f)
				{
					inst.clientState.rotate = 1;
				}
				else
				if (vjoy->stick_asb_delta.x <-0.5f)
				{
					inst.clientState.rotate =-1;
				}

				inst.clientState.fired = vjoy->button_a_pressed;

				inst.clientState.needed_tower_angel = -1000.0f;
			}
			else
			{
				inst.clientState.up = (int)core.controls.GetAliasValue(alias_forward, false);
				inst.clientState.rotate = (int)core.controls.GetAliasValue(alias_strafe, false);

				inst.clientState.fired = core.controls.GetAliasState(alias_fire);
				inst.clientState.special_fired = core.controls.GetAliasState(alias_rotate_active);
			}
		}

		float under = 1.0f;

		rcdesc.dir = Vector(0, -1, 0);
		rcdesc.length = under + 2.0f;

		Vector org = mat.Pos();
		org.y += under;

		Vector scr_pos = core.render.TransformToScreen(mat.Pos(), 2);
		Vector2 bar_size(60.0f, 5.0f);

		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 60.0f), bar_size, COLOR_RED);
		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 60.0f), Vector2(bar_size.x * inst.serverState.hp * 0.01f, bar_size.y), COLOR_GREEN);

		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 50.0f), bar_size, COLOR_RED);
		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 50.0f), Vector2(bar_size.x * inst.serverState.ammo * 0.01f, bar_size.y), COLOR_CYAN);

		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 40.0f), bar_size, COLOR_RED);
		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 40.0f), Vector2(bar_size.x * inst.serverState.special * 0.01f, bar_size.y), COLOR_YELLOW);

		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 30.0f), bar_size, COLOR_RED);
		core.render.DebugSprite(nullptr, Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 30.0f), Vector2(bar_size.x * (1.0f - inst.serverState.shoot_cooldown / 1.5f), bar_size.y), COLOR_BLUE);

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

		core.render.DebugSphere(p1, COLOR_RED, 0.5f);
		core.render.DebugSphere(p2, COLOR_RED, 0.5f);
		core.render.DebugSphere(p3, COLOR_RED, 0.5f);

		Matrix mdl = mat;
		inst.base_drawer->SetTransform(mdl);

		mdl = Matrix().RotateY(inst.serverState.tower_angel - inst.serverState.angle) * Matrix().Move(base_model.locator) * mdl;
		inst.tower_drawer->SetTransform(mdl);

		Vector tower = mdl.Pos();

		mdl = Matrix().Move(tower_model.locator) * mdl;
		inst.gun_drawer->SetTransform(mdl);

		mdl = Matrix().Move(gun_model.locator) * mdl;

		if (inst.is_contralable)
		{
			inst.clientState.gun_pos = mdl.Pos();

			Vector dr = target_pt - tower;
			float hgt = dr.y;
			dr.y = 0;
			float len = dr.Length();

			dr = mdl.Vx();
			dr.y = 0;
			dr.Normalize();
			dr *= len;
			dr.y = hgt;

			Vector trg = tower + dr;

			inst.clientState.gun_dir = trg - mdl.Pos();
			inst.clientState.gun_dir.Normalize();

			rcdesc.origin = inst.clientState.gun_pos;
			rcdesc.dir = inst.clientState.gun_dir;
			rcdesc.length = 500;

			if (PScene()->RayCast(rcdesc))
			{
				trg = rcdesc.hitPos;
			}

			core.render.DebugSphere(trg, COLOR_CYAN, 0.5f);
		}
	}
}