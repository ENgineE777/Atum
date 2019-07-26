
#include "SpriteTileInst.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, SpriteTileInst, "SpriteNT")

META_DATA_DESC(SpriteTileInst)
	BASE_SCENE_OBJ_PROP(SpriteTileInst)
	FLOAT_PROP(SpriteTileInst, axis_scale, 1.0f, "Geometry", "axis_scale", "Scale of a axis")
	FLOAT_PROP(SpriteTileInst, trans.depth, 0.5f, "Geometry", "Depth", "Z depth")
	INT_PROP(SpriteTileInst, draw_level, 0, "Geometry", "draw_level", "Draw priority")
	ARRAY_PROP_INST(SpriteTileInst, instances, Instance, "Prop", "inst", SpriteTileInst, sel_inst)
META_DATA_DESC_END()

void SpriteTileInst::Init()
{
}

void SpriteTileInst::ApplyProperties()
{
#ifdef EDITOR
	RenderTasks(false)->DelAllTasks(this);
#endif

	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + draw_level, this, (Object::Delegate)&SpriteTileInst::Draw);
}

bool SpriteTileInst::Play()
{
	trans.size = ((SpriteTileAsset*)asset)->trans.size;
	trans.offset = ((SpriteTileAsset*)asset)->trans.offset;

	return true;
}

void SpriteTileInst::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	SpriteTileAsset* sprite_asset = (SpriteTileAsset*)asset;

#ifdef EDITOR
	if (edited)
	{
		if (rect_select)
		{
			if (core.controls.DebugKeyPressed("KEY_I"))
			{
				ClearRect();
				CalcIndices();
			}
			else
			if (core.controls.DebugKeyPressed("KEY_P"))
			{
				FillRect();
				CalcIndices();
			}
			else
			{
				if (Gizmo::inst->delta_move.Length() > 0.01f)
				{
					for (auto& index : sel_instances)
					{
						auto& inst = instances[index];
						inst.SetPos(inst.GetPos() + Gizmo::inst->delta_move);
					}

					rect_p1 += Gizmo::inst->delta_move;
					rect_p2 += Gizmo::inst->delta_move;

					Gizmo::inst->delta_move = 0.0f;

					CalcIndices();
				}
			}
		}
		else
		{
			if (sel_inst != -1 && instances[sel_inst].GetPos().Length(trans.pos) > 0.01f)
			{
				instances[sel_inst].SetPos(trans.pos);
				CalcIndices();
			}

			if (core.controls.DebugKeyPressed("KEY_I") && sel_inst !=-1)
			{
				for (auto comp : components)
				{
					comp->InstDeleted(sel_inst);
				}

				instances.erase(sel_inst + instances.begin());
				sel_inst = -1;
				SetGizmo();

				CalcIndices();
			}

			bool add_center = core.controls.DebugKeyPressed("KEY_P");
			bool add_copy = core.controls.DebugKeyPressed("KEY_O");

			if (add_center || (add_copy && sel_inst != -1))
			{
				Instance inst;

				if (add_copy)
				{
					inst = instances[sel_inst];
					inst.SetPos(instances[sel_inst].GetPos() + 20.0f);
				}
				else
				{
					float scale = 1024.0f / core.render.GetDevice()->GetHeight();
					inst.SetPos({ Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale });
				}

				instances.push_back(inst);

				for (auto comp : components)
				{
					comp->InstAdded(-1);
				}

				sel_inst = (int)instances.size() - 1;

				CalcIndices();

				SetGizmo();
			}
		}
	}
#endif

	trans.offset = sprite_asset->trans.offset;
	trans.size = sprite_asset->trans.size + 0.5f;

	Vector2 pos = trans.pos;
	trans.pos *= axis_scale;

	Vector2 cam_pos = 0.0f;
	if (Sprite::use_ed_cam)
	{
		cam_pos = Sprite::ed_cam_pos;
		Sprite::ed_cam_pos *= axis_scale;
	}
	else
	{
		cam_pos = Sprite::cam_pos;
		Sprite::cam_pos *= axis_scale;
	}

	for (auto inst : instances)
	{
		if (GetState() == Active)
		{
			Sprite::UpdateFrame(&sprite_asset->sprite[inst.index], &inst.frame_state, dt);
		}

		if (inst.color.a < 0.01f)
		{
			continue;
		}

		Vector2 pos = inst.GetPos();
		trans.mat_global.Pos() = { pos.x, pos.y, trans.depth };
		inst.frame_state.horz_flipped = inst.GetFlipped();
		inst.color.a = inst.GetAlpha();

		Sprite::Draw(&trans, inst.color, &sprite_asset->sprite[inst.index], &inst.frame_state, true, false);
	}

#ifdef EDITOR
	if (rect_select)
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

		Vector2 delta = Sprite::ed_cam_pos - Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

		for (auto& index : sel_instances)
		{
			auto& inst = instances[index];
			Vector2 pos = (inst.GetPos() - sprite_asset->trans.offset * sprite_asset->trans.size) * scale - delta;
			core.render.DebugRect2D(pos, pos + sprite_asset->trans.size * scale, COLOR_WHITE);
		}
	}

	if (edited)
	{
		if (sel_inst != -1)
		{
			trans.pos = instances[sel_inst].GetPos();
			trans.rotation = instances[sel_inst].GetAngle();
			trans.BuildMatrices();
		}
	}
#endif

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}

#ifdef EDITOR
bool SpriteTileInst::IsOccupied(Vector2 pos)
{
	for (auto& inst : instances)
	{
		Vector2 inst_pos = inst.GetPos();
		if (fabs(inst_pos.x - pos.x) < 0.01f && fabs(inst_pos.y - pos.y) < 0.01f)
		{
			return true;
		}
	}

	return false;
}

void SpriteTileInst::CalcIndices()
{
	for (auto& inst : instances)
	{
		Vector2 pos = inst.GetPos();
		inst.SetPos({ ((int)(pos.x / trans.size.x)) * trans.size.x, ((int)(pos.y / trans.size.y)) * trans.size.y });
	}

	for (auto& inst : instances)
	{
		Vector2 pos = inst.GetPos();
		bool up = IsOccupied(pos - Vector2(0.0f, trans.size.y));
		bool down = IsOccupied(pos + Vector2(0.0f, trans.size.y));
		bool left = IsOccupied(pos - Vector2(trans.size.x, 0.0f));
		bool right = IsOccupied(pos + Vector2(trans.size.x, 0.0f));

		inst.index = 3;

		if (!up)
		{
			inst.index = 0;
		}
		else
		if (!down)
		{
			inst.index = 6;
		}
		
		int index = 1;

		if (!left)
		{
			index = 0;
		}
		else
		if (!right)
		{
			index = 2;
		}

		inst.index += index;
	}
}
#endif