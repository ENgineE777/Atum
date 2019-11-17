
#include "SpriteGraphInst.h"
#include "SpriteAsset.h"
#include "Services/Render/Render.h"
#include "Phys2DComp.h"
#include "SpriteGraphInst.h"

CLASSREG(SceneObject, SpriteGraphInst, "SpriteGraph")

META_DATA_DESC(SpriteGraphInst)
	BASE_SCENE_OBJ_PROP(SpriteGraphInst)
	FLOAT_PROP(SpriteGraphInst, trans.depth, 0.5f, "Geometry", "Depth", "Z depth")
	FLOAT_PROP(SpriteGraphInst, hack_height, 0.0f, "Geometry", "hack_height", "Addition height applyed fr every instance")
	INT_PROP(SpriteGraphInst, draw_level, 0, "Geometry", "draw_level", "Draw level")
	ARRAY_PROP_INST_CALLGIZMO(SpriteGraphInst, instances, Instance, "Prop", "inst", SpriteGraphInst, sel_inst, SetGizmo)
META_DATA_DESC_END()

void SpriteGraphInst::BindClassToScript()
{
	const char* brief = "Representation of instance of sprite graph asset\n"
		"\n"
		"Instance holds array of sprite graph instances. This array can be binded into a script array.\n"
		"\n"
		"This class ::SpriteGraphInst is a representation on C++ side.\n";

	core.scripts.RegisterObjectType("Graph2D", sizeof(SpriteInst::Instance), "gr_script_scene_objects", brief);
	core.scripts.RegisterObjectMethod("Graph2D", "bool ActivateLink(string&in)", WRAP_MFN(SpriteInst::Instance, ActivateLink), "Activate Link by name");
	core.scripts.RegisterObjectMethod("Graph2D", "void GotoNode(string&in)", WRAP_MFN(SpriteInst::Instance, GotoNode), "Goto to certein node");
}

void SpriteGraphInst::Init()
{
	script_callbacks.push_back(ScriptCallback(GetScene(), "OnContact", "void ", "%i%s%i"));
	script_callbacks.push_back(ScriptCallback(GetScene(), "AnimEvent", "void ", "%s%i%s%s"));
}

void SpriteGraphInst::PrepereInstaces()
{
	for (auto& inst : instances)
	{
		((SpriteGraphAsset*)asset)->PrepareInstance(&inst.graph_instance);
	}
}

void SpriteGraphInst::ApplyProperties()
{
#ifdef EDITOR
	RenderTasks(false)->DelAllTasks(this);
#endif

	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + draw_level, this, (Object::Delegate)&SpriteGraphInst::Draw);

	PrepereInstaces();
}

void SpriteGraphInst::OnResize(int at, int delta)
{
	SpriteInst::OnResize(at, delta);

	PrepereInstaces();
}

void SpriteGraphInst::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

#ifdef EDITOR
	if (edited)
	{
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

			((SpriteGraphAsset*)asset)->PrepareInstance(&inst.graph_instance);
			inst.graph_instance.Reset();

			instances.push_back(inst);

			for (auto comp : components)
			{
				comp->InstAdded(add_copy ? sel_inst : -1);
			}

			sel_inst = (int)instances.size() - 1;

			SetGizmo();
		}
	}
#endif

	int index = 0;

	auto* callback = GetScene()->Playing() ? FindScriptCallback("AnimEvent") : nullptr;

	for (auto& inst : instances)
	{
		if (!inst.IsVisible())
		{
			continue;
		}

		if (GetState() == Active)
		{
			inst.graph_instance.Update(GetName(), index, Script(), callback, dt);
		}

		if (inst.GetAlpha() < 0.01f)
		{
			continue;
		}

		trans.pos = inst.GetPos();
		trans.pos.y += hack_height;
		trans.offset = inst.graph_instance.cur_node->asset->trans.offset;
		trans.rotation = inst.GetAngle();

		trans.size = inst.graph_instance.cur_node->asset->trans.size;
		trans.offset = inst.graph_instance.cur_node->asset->trans.offset;

		trans.BuildMatrices();

		inst.graph_instance.state.horz_flipped = inst.GetFlipped();
		inst.color.r = inst.GetR();
		inst.color.g = inst.GetG();
		inst.color.b = inst.GetB();
		inst.color.a = inst.GetAlpha();

		Sprite::Draw(&trans, inst.color, &inst.graph_instance.cur_node->asset->sprite, &inst.graph_instance.state, true, false);

		index++;
	}
}
