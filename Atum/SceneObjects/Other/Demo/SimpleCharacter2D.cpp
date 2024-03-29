
#include "SimpleCharacter2D.h"
#include "Services/Render/Render.h"
#include "SceneObjects/2D/SpriteAsset.h"

CLASSREG(SceneObject, SimpleCharacter2D, "SimpleCharacter2D")

META_DATA_DESC(SimpleCharacter2D)
BASE_SCENE_OBJ_PROP(SimpleCharacter2D)
FLOAT_PROP(SimpleCharacter2D, trans.pos.x, 100.0f, "Geometry", "x", "X coordinate of position of a character")
FLOAT_PROP(SimpleCharacter2D, trans.pos.y, 100.0f, "Geometry", "y", "X coordinate of position of a character")
SCENEOBJECT_PROP(SimpleCharacter2D, vjoy_ref, "Prop", "VJoy")
STRING_PROP(SimpleCharacter2D, asset_name, "", "Prop", "Asset")
FLOAT_PROP(SimpleCharacter2D, speed, 120.0f, "Prop", "Speed", "Speed of a charater")
BOOL_PROP(SimpleCharacter2D, is_enemy, true, "Prop", "IsEnemy", "Definig if charcter is a enemy")
INT_PROP(SimpleCharacter2D, max_hp, 100, "Prop", "HP", "Max Health of a character")
FLOAT_PROP(SimpleCharacter2D, floor_width, 500.0f, "Prop", "FloorWidth", "Width of a level floor")
FLOAT_PROP(SimpleCharacter2D, floor_height, 200.0f, "Prop", "FloorHeight", "Higth of a level floor")
META_DATA_DESC_END()

void SimpleCharacter2D::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SimpleCharacter2D, "SimpleCharacter2D")
	core.scripts.RegisterObjectProperty(script_class_name, "int cur_hp", memberOFFSET(SimpleCharacter2D, cur_hp), "Health of acharacter");
	core.scripts.RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(SimpleCharacter2D, Reset), "Setting postion to initial position on start scene");
	core.scripts.RegisterObjectMethod(script_class_name, "void SetAnimGraph(string&in)", WRAP_MFN(SimpleCharacter2D, SetAnimGraph), "Setting anim graph asset by name");
}

void SimpleCharacter2D::Init()
{
	trans.offset.y = 1.0f;

	Tasks(false)->AddTask(10, this, (Object::Delegate)&SimpleCharacter2D::Update);
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SimpleCharacter2D::Draw);

	GetScene()->AddToGroup(this, "SimpleCharacter2D");
}

void SimpleCharacter2D::ApplyProperties()
{
	SetAnimGraph(asset_name);

	vjoy = (VirtualJoystick*)vjoy_ref.object;
}

void SimpleCharacter2D::Update(float dt)
{
	if (state != Active)
	{
		return;
	}

	if (!asset || !graph_instance.cur_node)
	{
		return;
	}

	if (GetScene()->Playing())
	{
		if (arraive > 0.0f)
		{
			arraive -= dt;

			if (arraive < 0.0f)
			{
				arraive = -1.0f;
				graph_instance.ActivateLink("Idle");
			}
			else
			{
				return;
			}
		}

		if (!target)
		{
			target = FindTarget();
		}

		if (!is_enemy)
		{
			ControlPlayer(dt);
		}
		else
		{
			ControlEnemy(dt);
		}

		if (cur_time_to_kick > 0.0f)
		{
			cur_time_to_kick -= dt;

			if (cur_time_to_kick < 0.0f)
			{
				cur_time_to_kick = -1.0f;

				Vector2 kick_pos = trans.pos;
				kick_pos.x += flipped ? -120.0f : 120.0f;

				MakeHit(kick_pos, 25);

				if (target && target->cur_hp == 0)
				{
					target = nullptr;
				}
			}
		}

		if (target && allow_move)
		{
			flipped = trans.pos.x > target->trans.pos.x;
		}

		if (dir_horz == 0 && dir_vert == 0)
		{
			if (allow_move)
			{
				graph_instance.ActivateLink("Idle");
				allow_move = false;
			}
		}
		else
		{
			trans.pos.x += dt * speed * dir_horz;

			trans.pos.x = fmax(-floor_width, trans.pos.x);
			trans.pos.x = fmin( floor_width, trans.pos.x);

			trans.pos.y += dt * speed * 0.75f * dir_vert;

			trans.pos.y = fmax(floor_margin.x, trans.pos.y);
			trans.pos.y = fmin(floor_margin.y, trans.pos.y);
		}

		if (resp_time > 0.0f)
		{
			resp_time -= dt;

			if (resp_time < 0.0f)
			{
				resp_time = -1.0f;

				Respawn();
			}
		}

		if (vanish_time > 0.0f)
		{
			vanish_time -= dt;

			if (vanish_time < 0.0f)
			{
				vanish_time = -1.0f;

				resp_time = 3.0f;
			}
		}

		if (death_fly > 0.0f)
		{
			death_fly -= dt;

			trans.pos.x += dt * (flipped ? 380.0f : -380.0f);

			if (death_fly < 0.0f)
			{
				death_fly = -1.0f;
				vanish_time = 2.0f;
			}
		}
	}
}

void SimpleCharacter2D::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	if (!asset || !graph_instance.cur_node)
	{
		return;
	}

	if (resp_time > 0.0f || (vanish_time > 0.0f && ((int)(vanish_time / 0.1f) % 2 == 0)))
	{
		return;
	}

	trans.size = graph_instance.cur_node->asset->trans.size;
	trans.depth = 0.74f - ((trans.pos.y - floor_margin.x) / floor_height) * 0.5f;
	graph_instance.state.horz_flipped = flipped;
	trans.BuildMatrices();

	if (arraive > 0.0f)
	{
		trans.mat_global.Pos().y -= 1024.0f * arraive;
	}

	if (GetState() == Active)
	{
		graph_instance.Update(GetName(), 0, Script(), nullptr, dt);
	}

	Sprite::Draw(&trans, COLOR_WHITE, &graph_instance.cur_node->asset->sprite, &graph_instance.state, true, false);
}

SimpleCharacter2D* SimpleCharacter2D::FindTarget()
{
	vector<Scene::Group*> out_group;
	GetScene()->GetGroup(out_group, "SimpleCharacter2D");

	for (auto group : out_group)
	{
		for (auto& object : group->objects)
		{
			SimpleCharacter2D* chraracter = (SimpleCharacter2D*)object;

			if (chraracter->cur_hp <= 0)
			{
				continue;
			}

			if (chraracter->is_enemy == !is_enemy)
			{
				return chraracter;
			}
		}
	}

	return nullptr;
}

void SimpleCharacter2D::ControlPlayer(float dt)
{
	dir_horz = 0;

	if (core.controls.DebugKeyPressed("KEY_A", Controls::Active) || (vjoy && vjoy->stick_delta.x < 0.0f))
	{
		if (!allow_move)
		{
			allow_move = graph_instance.ActivateLink(target ? "WalkBack" : "Walk");
		}

		if (allow_move)
		{
			if (!target)
			{
				flipped = true;
			}

			dir_horz = -1;
		}
	}
	else
	if (core.controls.DebugKeyPressed("KEY_D", Controls::Active) || (vjoy && vjoy->stick_delta.x > 0.0f))
	{
		if (!allow_move)
		{
			allow_move = graph_instance.ActivateLink("Walk");
		}

		if (allow_move)
		{
			if (!target)
			{
				flipped = false;
			}

			dir_horz = 1;
		}
	}

	dir_vert = 0;

	if (core.controls.DebugKeyPressed("KEY_W", Controls::Active) || (vjoy && vjoy->stick_delta.y < 0.0f))
	{
		if (!allow_move)
		{
			allow_move = graph_instance.ActivateLink("WalkBack");
		}

		if (allow_move)
		{
			dir_vert = -1;
		}
	}
	else
	if (core.controls.DebugKeyPressed("KEY_S", Controls::Active) || (vjoy && vjoy->stick_delta.y > 0.0f))
	{
		if (!allow_move)
		{
			allow_move = graph_instance.ActivateLink("Walk");
		}

		if (allow_move)
		{
			dir_vert = 1;
		}
	}

	if (core.controls.DebugKeyPressed("KEY_E") || (vjoy && vjoy->button_a_pressed == 2))
	{
		if (graph_instance.ActivateLink("LegKick1"))
		{
			cur_time_to_kick = 0.45f;
			allow_move = false;
		}
	}

	if (core.controls.DebugKeyPressed("KEY_R") || (vjoy && vjoy->button_b_pressed == 2))
	{
		if (graph_instance.ActivateLink("LegKick2"))
		{
			cur_time_to_kick = 0.55f;
			allow_move = false;
		}
	}
}

void SimpleCharacter2D::ControlEnemy(float dt)
{
	if (target)
	{
		//if (next_kick == -1)
		{
			//next_kick
		}

		if (fabs(trans.pos.x - target->trans.pos.x) > 200.0f)
		{
			if (!allow_move)
			{
				allow_move = graph_instance.ActivateLink("Walk");
			}

			if (allow_move)
			{
				dir_horz = (trans.pos.x - target->trans.pos.x) > 0.0f ? -1 : 1;
			}
		}
		else
		{
			dir_horz = 0;
		}

		if (fabs(trans.pos.x - target->trans.pos.x) < 500.0f && fabs(trans.pos.y - target->trans.pos.y) > 5.0f)
		{
			if (!allow_move)
			{
				allow_move = graph_instance.ActivateLink("Walk");
			}

			if (allow_move)
			{
				dir_vert = (trans.pos.y - target->trans.pos.y) > 0.0f ? -1 : 1;
			}
		}
		else
		{
			dir_vert = 0;
		}

		if (fabs(trans.pos.x - target->trans.pos.x) < 200.0f && fabs(trans.pos.y - target->trans.pos.y) < 5.0f)
		{
			cur_time_2_kuck += dt;
			if (cur_time_2_kuck > time_2_kick)
			{
				cur_time_2_kuck = 0.0f;

				bool sec_kick = Math::Rand() > 0.5;
				if (graph_instance.ActivateLink(sec_kick ? "LegKick1" : "LegKick2"))
				{
					cur_time_to_kick = sec_kick ? 0.55f : 0.85f;
					flipped = trans.pos.x > target->trans.pos.x;
					allow_move = false;
				}
			}
		}
		else
		{
			cur_time_2_kuck = 0.0f;
		}
	}
}

void SimpleCharacter2D::MakeHit(Vector2 pos, int damage)
{
	vector<Scene::Group*> out_group;
	GetScene()->GetGroup(out_group, "SimpleCharacter2D");

	for (auto group : out_group)
	{
		for (auto& object : group->objects)
		{
			SimpleCharacter2D* chraracter = (SimpleCharacter2D*)object;

			if (chraracter->is_enemy == !is_enemy && chraracter->cur_hp > 0)
			{
				if ((chraracter->trans.pos.x - 85.0f) < pos.x && pos.x < (chraracter->trans.pos.x + 85.0f) && fabs(chraracter->trans.pos.y - pos.y) < 15.0f)
				{
					if (chraracter->graph_instance.ActivateLink("Hit"))
					{
						chraracter->cur_hp -= damage;

						chraracter->cur_time_to_kick = -1.0f;
						chraracter->allow_move = false;

						if (chraracter->cur_hp <= 0)
						{
							chraracter->cur_hp = 0;
							chraracter->target = nullptr;
							chraracter->graph_instance.GotoNode("Death");
							chraracter->death_fly = 0.75f;
						}
					}
				}
			}
		}
	}
}

void SimpleCharacter2D::Respawn()
{
	cur_hp = max_hp;

	arraive = 1.0f;

	graph_instance.GotoNode("Resp");
}

void SimpleCharacter2D::Reset()
{
	trans.pos = init_pos;

	cur_hp = max_hp;

	allow_move = false;
	flipped = false;
	dir_horz = 0;
	dir_vert = 0;
	time_2_kick = 0.5f;
	cur_time_2_kuck = 0;
	cur_time_to_kick = -1.0f;
	death_fly = -1.0f;
	vanish_time = -1.0f;
	arraive = -1.0f;
	resp_time = -1.0f;
	next_kick = -1;

	graph_instance.Reset();

	if (is_enemy)
	{
		Respawn();
	}
}

void SimpleCharacter2D::SetAnimGraph(string& graph)
{
	asset = (SpriteGraphAsset*)GetScene()->FindInGroup("SpriteGraphAsset", graph.c_str());

	if (asset)
	{
		asset->PrepareInstance(&graph_instance);

		if (graph_instance.cur_node)
		{
			floor_margin.y = 1024.0f;
			floor_margin.x = floor_margin.y - floor_height;
		}
	}
}

bool SimpleCharacter2D::Play()
{
	init_pos = trans.pos;

	Reset();

	return true;
}

#ifdef EDITOR
void SimpleCharacter2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);
	Gizmo::inst->SetTrans2D(Gizmo::Transform2D(trans), Gizmo::trans_2d_move);
}
#endif