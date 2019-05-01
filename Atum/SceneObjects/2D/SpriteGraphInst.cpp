
#include "SpriteGraphInst.h"
#include "SpriteAsset.h"
#include "Services/Render/Render.h"
#include "Phys2DComp.h"
#include "SpriteGraphInst.h"

CLASSREG(SceneObject, SpriteGraphInst, "SpriteGraph")

META_DATA_DESC(SpriteGraphInst)
	BASE_SCENE_OBJ_PROP(SpriteGraphInst)
	FLOAT_PROP(SpriteGraphInst, trans.depth, 0.5f, "Geometry", "Depth")
	FLOAT_PROP(SpriteGraphInst, hack_height, 0.0f, "Geometry", "hack_height")
	INT_PROP(SpriteGraphInst, draw_level, 0, "Geometry", "draw_level")
	ARRAY_PROP_INST(SpriteGraphInst, instances, Instance, "Prop", "inst", SpriteGraphInst, sel_inst)
META_DATA_DESC_END()

void SpriteGraphInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteGraphInst)

	core.scripts.RegisterObjectType("Graph2D", sizeof(SpriteInst::Instance), "gr_script_scene_object_components");
	core.scripts.RegisterObjectMethod("Graph2D", "bool ActivateLink(string&in)", WRAP_MFN(SpriteInst::Instance, ActivateLink));
	core.scripts.RegisterObjectMethod("Graph2D", "void GotoNode(string&in)", WRAP_MFN(SpriteInst::Instance, GotoNode));
}

void SpriteGraphInst::Init()
{
	script_callbacks.push_back(ScriptCallback("OnContact", "void ", "%i%s%i"));
}

void SpriteGraphInst::ApplyProperties()
{
#ifdef EDITOR
	RenderTasks(false)->DelAllTasks(this);
#endif

	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + draw_level, this, (Object::Delegate)&SpriteGraphInst::Draw);

	if (asset)
	{
		for (auto& inst : instances)
		{
			((SpriteGraphAsset*)asset)->PrepareInstance(&inst.graph_instance);
			inst.graph_instance.Reset();
		}
	}
}

bool SpriteGraphInst::Play()
{
	trans.size = ((SpriteGraphAsset*)asset)->GetDefaultSize();
	trans.offset = ((SpriteGraphAsset*)asset)->GetDefaultOffset();

	return true;
}

void SpriteGraphInst::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	trans.size = ((SpriteGraphAsset*)asset)->GetDefaultSize();
	trans.offset = ((SpriteGraphAsset*)asset)->GetDefaultOffset();

#ifdef EDITOR
	if (edited)
	{
		if (sel_inst != -1)
		{
			instances[sel_inst].SetPos(trans.pos);
		}

		if (core.controls.DebugKeyPressed("KEY_I") && sel_inst != -1)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(sel_inst);
			}

			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
		}

		bool add_center = core.controls.DebugKeyPressed("KEY_O");
		bool add_after = core.controls.DebugKeyPressed("KEY_P");

		if (add_center || add_after)
		{
			Instance inst;

			if (sel_inst != -1 && add_after)
			{
				inst.SetPos(instances[sel_inst].GetPos() + 20.0f);
			}
			else
			{
				float scale = 1024.0f / core.render.GetDevice()->GetHeight();
				inst.SetPos({ Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale });
			}

			((SpriteGraphAsset*)asset)->PrepareInstance(&inst.graph_instance);
			inst.graph_instance.Reset();

			instances.push_back(inst);

			sel_inst = (int)instances.size() - 1;

			for (auto comp : components)
			{
				comp->InstAdded();
			}

			SetGizmo();
		}
	}
#endif

	int index = 0;

	for (auto& inst : instances)
	{
		if (!inst.IsVisible())
		{
			continue;
		}

		if (GetState() == Active)
		{
			inst.graph_instance.Update(dt);
		}

		if (inst.GetAlpha() < 0.01f)
		{
			continue;
		}

		trans.pos = inst.GetPos();
		trans.pos.y += hack_height;
		trans.offset = inst.graph_instance.cur_node->asset->trans.offset;
		trans.rotation = inst.GetAngle();
		trans.BuildMatrices();

		inst.graph_instance.state.horz_flipped = inst.GetFlipped();
		inst.color.a = inst.GetAlpha();

		Sprite::Draw(&trans, inst.color, &inst.graph_instance.cur_node->asset->sprite, &inst.graph_instance.state, true, false);
	}

#ifdef EDITOR
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
}
