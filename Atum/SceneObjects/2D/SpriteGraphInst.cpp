
#include "SpriteGraphInst.h"
#include "SpriteAsset.h"
#include "Services/Render/Render.h"
#include "Phys2DComp.h"
#include "SpriteGraphInst.h"

CLASSREG(SceneObject, SpriteGraphInst, "SpriteGraph")

META_DATA_DESC(SpriteGraphInst)
BASE_SCENE_OBJ_PROP(SpriteGraphInst)
FLOAT_PROP(SpriteGraphInst, trans.depth, 0.5f, "Geometry", "Depth")
ARRAY_PROP(SpriteGraphInst, instances, Instance, "Prop", "inst")
META_DATA_DESC_END()

void SpriteGraphInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteGraphInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void ActivateLink(int index, string&in)", WRAP_MFN(SpriteGraphInst, ActivateLink), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void GotoNode(int index, string&in)", WRAP_MFN(SpriteGraphInst, GotoNode), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "bool CheckColission(int index, bool under)", WRAP_MFN(SpriteGraphInst, CheckColission), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void Move(int index, float dx, float dy)", WRAP_MFN(SpriteGraphInst, MoveController), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void MoveTo(int index, float x, float y)", WRAP_MFN(SpriteGraphInst, MoveControllerTo), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void SetActiveTrack(int index, bool set)", WRAP_MFN(SpriteGraphInst, SetActiveTrack), asCALL_GENERIC);
}

void SpriteGraphInst::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpriteGraphInst::Draw);

	script_callbacks.push_back(ScriptCallback("OnContact", "int ", "%i%s%i"));
}

void SpriteGraphInst::ApplyProperties()
{
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
	trans.size = ((SpriteGraphAsset*)asset)->trans.size;
	trans.offset = ((SpriteGraphAsset*)asset)->trans.offset;

	return true;
}

void SpriteGraphInst::Draw(float dt)
{
	if (state == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	trans.size = ((SpriteGraphAsset*)asset)->trans.size;
	trans.offset = ((SpriteGraphAsset*)asset)->trans.offset;

#ifdef EDITOR
	if (edited)
	{
		if (sel_inst != -1)
		{
			instances[sel_inst].SetPos(trans.pos);
		}

		if (controls.DebugKeyPressed("KEY_I") && sel_inst != -1)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(sel_inst);
			}

			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
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

		if (state == Active)
		{
			inst.graph_instance.Update(dt);
		}

		if (inst.GetAlpha() < 0.01f)
		{
			continue;
		}

		trans.pos = inst.GetPos();
		trans.offset = inst.graph_instance.cur_node->asset->trans.offset;
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

void SpriteGraphInst::ActivateLink(int index, string& link)
{
	instances[index].graph_instance.ActivateLink(link.c_str());
}

void SpriteGraphInst::GotoNode(int index, string& node)
{
	instances[index].graph_instance.GotoNode(node.c_str());
}

PhysController* SpriteGraphInst::HackGetController(int index)
{
	for (auto comp : components)
	{
		Phys2DCompInst* phys_comp = dynamic_cast<Phys2DCompInst*>(comp);

		if (phys_comp)
		{
			return phys_comp->bodies[index].controller;
		}
	}

	return nullptr;
}

bool SpriteGraphInst::CheckColission(int index, bool under)
{
	PhysController* controller = HackGetController(index);

	if (controller)
	{
		return controller->IsColliding(under ? PhysController::CollideDown : PhysController::CollideUp);
	}

	return false;
}

void SpriteGraphInst::MoveController(int index, float dx, float dy)
{
	instances[index].dir.x += dx;
	instances[index].dir.y += dy;
}

void SpriteGraphInst::MoveControllerTo(int index, float x, float y)
{
	PhysController* controller = HackGetController(index);

	if (controller)
	{
		controller->SetPosition({ x / 50.0f, -y / 50.0f, 0.0f });
	}
}