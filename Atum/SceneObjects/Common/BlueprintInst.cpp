
#include "editor/Project.h"
#include "BlueprintInst.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, BlueprintInst, "Blueprint")

META_DATA_DESC(BlueprintInst)
BASE_SCENE_OBJ_PROP(BlueprintInst)
FLOAT_PROP(BlueprintInst, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of position of a trigger")
FLOAT_PROP(BlueprintInst, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of position of a trigger")
META_DATA_DESC_END()

void BlueprintInst::Init()
{
}

void BlueprintInst::Load(JSONReader& loader)
{
	SceneObjectInst::Load(loader);

	while (loader.EnterBlock("Childs"))
	{
		char type[512];
		loader.Read("type", type, 512);

		SceneObject* inst = GetScene()->CreateObject(type, false);
		inst->parent_uid = uid;

		GetScene()->DeleteObject(inst, false, false);

		inst->parent_trans = &trans;
		childs.push_back(inst);

		loader.Read("uid", inst->uid);

		inst->Load(loader);

		while (loader.EnterBlock("components"))
		{
			loader.Read("type", type, 512);

			SceneObjectComp* comp = inst->AddComponent(type);

			if (comp)
			{
				comp->Load(loader);
			}

			loader.LeaveBlock();
		}


		loader.LeaveBlock();
	}

	for (auto child : childs)
	{
		child->GetMetaData()->Prepare(child);
		child->GetMetaData()->PostLoad();

		child->ApplyProperties();

		for (auto comp : child->components)
		{
			comp->GetMetaData()->Prepare(comp);
			comp->GetMetaData()->PostLoad();

			comp->ApplyProperties();
		}
	}
}

void BlueprintInst::Save(JSONWriter& saver)
{
	SceneObjectInst::Save(saver);

	saver.StartArray("Childs");

	for (auto child : childs)
	{
		saver.StartBlock(nullptr);

		saver.Write("type", child->class_name);
		saver.Write("uid", child->GetUID());

		child->Save(saver);

		if (child->components.size() > 0)
		{
			saver.StartArray("components");

			for (auto& comp : child->components)
			{
				saver.StartBlock(nullptr);

				saver.Write("type", comp->class_name);
				comp->Save(saver);

				saver.FinishBlock();
			}

			saver.FinishArray();
		}

		saver.FinishBlock();
	}

	saver.FinishArray();
}

SceneObject* BlueprintInst::GetChild(uint32_t uid)
{
	for (auto child : childs)
	{
		if (child->GetUID() == uid)
		{
			return child;
		}
	}

	return nullptr;
}

void BlueprintInst::ApplyProperties()
{
	SceneObjectInst::ApplyProperties();

	for (auto child : childs)
	{
		child->group_name = group_name;
		child->layer_name = layer_name;
	}
}

void BlueprintInst::SetState(State set_state)
{
	state = set_state;

	for (auto child : childs)
	{
		child->SetState(set_state);
	}
}

bool BlueprintInst::Play()
{
	for (auto child : childs)
	{
		if (!child->Play())
		{
			return false;
		}

		for (auto comp : child->components)
		{
			comp->Play();
		}
	}

	return true;
}

#ifdef EDITOR
void BlueprintInst::Copy(SceneObject* src)
{
	SceneObjectInst::Copy(src);

	BlueprintInst* src_asset = (BlueprintInst*)src;

	for (auto child : src_asset->childs)
	{
		auto* childs_inst = GetScene()->CreateObject(child->class_name, false);
		GetScene()->DeleteObject(childs_inst, false, false);

		childs_inst->parent_trans = &trans;

		childs_inst->SetName(child->GetName());
		childs_inst->parent_uid = uid;
		childs_inst->uid = child->uid;

		childs_inst->Copy(child);

		childs.push_back(childs_inst);
	}

	for (auto child : childs)
	{
		child->CorrectRefToParent();
	}
}

void BlueprintInst::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	Gizmo::inst->SetTrans2D(Gizmo::Transform2D(trans), Gizmo::trans_2d_move);
}

void BlueprintInst::AddChildsToTree(EUITreeView* treeview)
{
	for (auto child : childs)
	{
		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = treeview->AddItem(child->GetName(), 1, tree_item, item, -1, true, child->class_name);
		child->asset_item = tree_item->item;
	}
}
#endif