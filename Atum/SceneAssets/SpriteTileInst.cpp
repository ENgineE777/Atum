
#include "SpriteTileInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpriteTileInst, "SpriteNT")

META_DATA_DESC(SpriteTileInst)
BASE_SCENE_OBJ_PROP(SpriteTileInst)
FLOAT_PROP(SpriteTileInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpriteTileInst, trans.depth, 0.5f, "Geometry", "Depth")
ARRAY_PROP(SpriteTileInst, instances, Instance, "Prop", "inst")
META_DATA_DESC_END()

void SpriteTileInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteTileInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void AddInstance(float x, float y)", WRAP_MFN(SpriteTileInst, AddInstance), asCALL_GENERIC);
}

void SpriteTileInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteTileInst::Draw);
}

void SpriteTileInst::Draw(float dt)
{
	if (state == Invisible)
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
		if (sel_inst != -1 && instances[sel_inst].GetPos().Distance(trans.pos) > 0.01f)
		{
			instances[sel_inst].SetPos(trans.pos);
			CalcIndices();
		}

		if (controls.DebugKeyPressed("KEY_I") && sel_inst !=-1)
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

		bool add_center = controls.DebugKeyPressed("KEY_O");
		bool add_after = controls.DebugKeyPressed("KEY_P");

		if (add_center || add_after)
		{
			Instance inst;

			if (sel_inst != -1 && add_after)
			{
				inst.SetPos(instances[sel_inst].GetPos() + 20.0f);
			}
			else
			{
				float scale = 1024.0f / render.GetDevice()->GetHeight();
				inst.SetPos({ Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale });
			}

			instances.push_back(inst);

			sel_inst = (int)instances.size() - 1;

			for (auto comp : components)
			{
				comp->InstAdded();
			}

			CalcIndices();

			SetGizmo();
		}
	}
#endif

	trans.offset = sprite_asset->trans.offset;
	trans.size = sprite_asset->trans.size;

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
		if (state == Active)
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
		inst.color.a = inst.alpha;

		Sprite::Draw(&trans, inst.color, &sprite_asset->sprite[inst.index], &inst.frame_state, true, false);
	}

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