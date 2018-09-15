
#include "SpriteTileInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

META_DATA_DESC(SpriteTileInst::Instance)
FLOAT_PROP(SpriteTileInst::Instance, pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteTileInst::Instance, pos.y, 0.0f, "Prop", "y")
INT_PROP(SpriteTileInst::Instance, index, 0, "Prop", "sprite")
META_DATA_DESC_END()

CLASSREG(SceneObject, SpriteTileInst, "Sprite")

META_DATA_DESC(SpriteTileInst)
BASE_SCENE_OBJ_NAME_PROP(SpriteTileInst)
BASE_SCENE_OBJ_STATE_PROP(SpriteTileInst)
FLOAT_PROP(SpriteTileInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpriteTileInst, trans.depth, 0.5f, "Geometry", "Depth")
ARRAY_PROP(SpriteTileInst, instances, Instance, "Prop", "inst")
META_DATA_DESC_END()

void SpriteTileInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteTileInst::Draw);
}

void SpriteTileInst::Play()
{
	trans.size = ((SpriteTileAsset*)asset)->trans.size;
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
		if (sel_inst != -1 && instances[sel_inst].pos.Distance(trans.pos) > 0.01f)
		{
			CalcIndices();
			instances[sel_inst].pos = trans.pos;
		}

		if (controls.DebugKeyPressed("KEY_O") && sel_inst !=-1)
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

		if (controls.DebugKeyPressed("KEY_P"))
		{
			Instance inst;
			float scale = render.GetDevice()->GetHeight() / 1024.0f;
			inst.pos.x = ((Sprite::ed_cam_pos.x + render.GetDevice()->GetWidth()) * 0.5f) / scale;
			inst.pos.y = Sprite::ed_cam_pos.y / scale + 512.0f;
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

		trans.mat_global.Pos() = { inst.pos.x, inst.pos.y, trans.depth };

		Sprite::Draw(&trans, COLOR_WHITE, &sprite_asset->sprite[inst.index], &inst.frame_state, true, false);
	}

	trans.pos = pos;

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
bool SpriteTileInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Instance& inst = instances[i];

		Vector2 pos = (inst.pos + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos;

		if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
			pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
		{
			sel_inst = i;

			if (edited)
			{
				SetGizmo();
			}

			return true;
		}
	}

	return false;
}

void SpriteTileInst::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		SetGizmo();
	}
}

void SpriteTileInst::SetGizmo()
{
	Gizmo::inst->trans2D = sel_inst != -1 ? &trans : nullptr;

	if (sel_inst != -1)
	{
		trans.pos = instances[sel_inst].pos;
		Gizmo::inst->pos2d = instances[sel_inst].pos;
	}

	Gizmo::inst->enabled = (sel_inst != -1);
}

bool SpriteTileInst::IsOccupied(Vector2 pos)
{
	for (auto& inst : instances)
	{
		if (fabs(inst.pos.x - pos.x) < 0.01f && fabs(inst.pos.y - pos.y) < 0.01f)
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
		bool up = IsOccupied(inst.pos - Vector2(0.0f, trans.size.y));
		bool down = IsOccupied(inst.pos + Vector2(0.0f, trans.size.y));
		bool left = IsOccupied(inst.pos - Vector2(trans.size.x, 0.0f));
		bool right = IsOccupied(inst.pos + Vector2(trans.size.x, 0.0f));

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