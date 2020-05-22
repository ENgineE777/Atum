
#include "BlueprintAsset.h"
#include "BlueprintInst.h"
#include "Services/Core/Core.h"

#ifdef EDITOR
#include "Editor/Project.h"
#include "Editor/Editor.h"
#endif

CLASSREG(SceneAsset, BlueprintAsset, "Blueprint")

META_DATA_DESC(BlueprintAsset)
	BASE_SCENE_ASSET_PROP(BlueprintAsset)
META_DATA_DESC_END()

SceneObject* BlueprintAsset::sel_inst = nullptr;

BlueprintAsset::BlueprintAsset()
{
	inst_class_name = "BlueprintInst";
}

void BlueprintAsset::Init()
{
	sub_scene.inc_scenes.push_back(GetScene());
	sub_scene.Init();
	StringUtils::Copy(sub_scene.project_scene_path, 512, GetScene()->project_scene_path);
	sub_scene.EnableTasks(false);

#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&BlueprintAsset::Work);
#endif
}

void BlueprintAsset::Load(JSONReader& loader)
{
	SceneAsset::Load(loader);

	while (loader.EnterBlock("Childs"))
	{
		char type[512];
		loader.Read("type", type, 512);

		auto inst = CreateObject(type);
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

void BlueprintAsset::Save(JSONWriter& saver)
{
	SceneAsset::Save(saver);

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

SceneObject* BlueprintAsset::GetChild(uint32_t uid)
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

#ifdef EDITOR
void BlueprintAsset::Copy(SceneObject* src)
{
	SceneAsset::Copy(src);

	BlueprintAsset* src_asset = (BlueprintAsset*)src;

	for (auto child : src_asset->childs)
	{
		auto* childs_inst = CreateObject(child->class_name);

		childs_inst->SetName(child->GetName());
		childs_inst->uid = child->uid;

		childs_inst->Copy(child);
	}

	for (auto child : childs)
	{
		child->CorrectRefToParent();
	}
}

SceneObject* BlueprintAsset::CreateInstance(Scene* scene)
{
	BlueprintInst* inst = (BlueprintInst*)SceneAsset::CreateInstance(scene);

	for (auto child : childs)
	{
		auto* childs_inst = scene->CreateObject(child->class_name, false);
		scene->DeleteObject(childs_inst, false, false);

		childs_inst->parent_trans = &inst->trans;

		childs_inst->SetName(child->GetName());
		childs_inst->uid = child->uid;

		childs_inst->Copy(child);

		childs_inst->parent_uid = inst->uid;

		inst->childs.push_back(childs_inst);
	}

	for (auto child : inst->childs)
	{
		child->CorrectRefToParent();
	}

	return inst;
}

bool BlueprintAsset::UseAseetsTree()
{
	return true;
}

void BlueprintAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);

	if (!ed)
	{
		if (sel_inst && sel_inst != this)
		{
			OnAssetTreeSelChange(nullptr);
		}
	}
	else
	{
		sel_inst = this;
	}

	sub_scene.EnableTasks(ed);
}

bool BlueprintAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneObject* item, int prev_child_index, SceneObject* target, int child_index)
{
	if (item_from_assets)
	{
		auto asset = (SceneAsset*)item;

		SceneObject* asset_inst = asset->CreateInstance(&sub_scene);

		for (auto comp : asset->components)
		{
			SceneObjectInstComp* comp_inst = (SceneObjectInstComp*)asset_inst->AddComponent(((SceneAssetComp*)comp)->inst_class_name);
			comp_inst->asset_comp = comp;
		}

		asset_inst->parent_uid = uid;
		childs.push_back(asset_inst);

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(asset_inst);
		tree_item->item = editor.asset_treeview->AddItem(asset_inst->GetName(), 1, tree_item, asset_item, -1, true, asset_inst->class_name);
		asset_inst->asset_item = tree_item->item;
		asset_inst->treeview = editor.asset_treeview;

		for (auto& inst : instances)
		{
			BlueprintInst* blueprint_inst = (BlueprintInst*)inst.GetObject();

			auto* childs_inst = scene->CreateObject(asset_inst->class_name, false);
			scene->DeleteObject(childs_inst, false, false);

			childs_inst->parent_trans = &blueprint_inst->trans;

			childs_inst->SetName(asset_inst->GetName());
			childs_inst->uid = asset_inst->uid;

			childs_inst->Copy(asset_inst);

			childs_inst->parent_uid = blueprint_inst->uid;

			blueprint_inst->childs.push_back(childs_inst);

			Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(childs_inst);
			tree_item->item = editor.scene_treeview->AddItem(childs_inst->GetName(), 1, tree_item, blueprint_inst->item, -1, true, childs_inst->class_name);
			childs_inst->asset_item = tree_item->item;
			childs_inst->treeview = editor.scene_treeview;
		}
	}

	return false;
}

void BlueprintAsset::PreapreAssetTree()
{
	Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(this);
	tree_item->item = editor.asset_treeview->AddItem(GetName(), 1, tree_item, nullptr, -1, true, class_name);
	asset_item = tree_item->item;

	for (auto child : childs)
	{
		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = editor.asset_treeview->AddItem(child->GetName(), 1, tree_item, asset_item, -1, true, child->class_name);
		child->asset_item = tree_item->item;
		child->treeview = editor.asset_treeview;
	}
}

void BlueprintAsset::Work(float dt)
{
	if (!IsEditMode())
	{
		return;
	}

	if (sel_inst && sel_inst->HasOwnTasks())
	{
		sel_inst->Tasks(true)->Execute(dt);
	}
}

void BlueprintAsset::OnAssetTreeSelChange(SceneObject* item)
{
	if (sel_inst)
	{
		sel_inst->GetMetaData()->HideWidgets();

		if (sel_inst != this)
		{
			sel_inst->EnableTasks(false);
			sel_inst->SetEditMode(false);
		}

		sub_scene.EnableTasks(true);
	}

	sel_inst = item;

	if (sel_inst)
	{
		sel_inst->GetMetaData()->Prepare(sel_inst);
		sel_inst->GetMetaData()->PrepareWidgets(editor.obj_cat);

		sel_inst->SetEditMode(true);
		sel_inst->EnableTasks(true);

		if (sel_inst != this)
		{
			sub_scene.EnableTasks(!sel_inst->HasOwnTasks());
		}
	}

	editor.obj_cmp_wgt.Prepare(sel_inst);
}

void BlueprintAsset::OnAssetTreePopupItem(int id)
{
	if (id == 2400)
	{
		auto* inst = CreateObject(popup_item->class_name);
		scene->GenerateUID(inst, false);
		inst->Copy(popup_item);

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(inst);
		tree_item->item = editor.asset_treeview->AddItem(inst->GetName(), 1, tree_item, asset_item, -1, true, inst->class_name);
		inst->asset_item = tree_item->item;
		inst->treeview = editor.asset_treeview;
	}

	if (id >= 2600)
	{
		const char* name = ClassFactorySceneObject::Decls()[id - 2600]->GetName();
		auto* inst = CreateObject(name);
		inst->SetName(name);
		scene->GenerateUID(inst, false);

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(inst);
		tree_item->item = editor.asset_treeview->AddItem(inst->GetName(), 1, tree_item, asset_item, -1, true, inst->class_name);
		inst->asset_item = tree_item->item;
	}

	if (id == 2401)
	{
		OnAssetTreeSelChange(nullptr);

		int index = 0;

		for (int i = 0; i < childs.size(); i++)
		{
			if ((SceneAsset*)childs[i] == popup_item)
			{
				childs.erase(childs.begin() + i);
				index = i;
				break;
			}
		}

		editor.asset_treeview->DeleteItem(popup_item->asset_item);
		sub_scene.DeleteObject(popup_item, false, true);

		for (auto& inst : instances)
		{
			BlueprintInst* blueprint_inst = (BlueprintInst*)inst.GetObject();
			auto* child = blueprint_inst->childs[index];
			blueprint_inst->childs.erase(blueprint_inst->childs.begin() + index);
			child->treeview->DeleteItem(child->item);
			child->Release();
		}
	}

	popup_item = nullptr;
}

void BlueprintAsset::OnAssetTreeRightClick(int x, int y, SceneObject* item, int child_index)
{
	if (!item)
	{
		return;
	}

	popup_item = item;

	editor.popup_menu->StartMenu(true);

	editor.popup_menu->StartSubMenu("Create");

	int id = 2600;

	for (auto decl : ClassFactorySceneObject::Decls())
	{
		if (!strstr(decl->GetName(), "Inst"))
		{
			editor.popup_menu->AddItem(id, decl->GetShortName());
		}

		id++;
	}

	editor.popup_menu->EndSubMenu();

	editor.popup_menu->AddSeparator();

	editor.popup_menu->AddItem(2400, "Duplicate");
	editor.popup_menu->AddItem(2401, "Delete");

	editor.popup_menu->ShowAsPopup(editor.asset_treeview, x, y);
}

void BlueprintAsset::CheckProperties()
{
	if (sel_inst && sel_inst != this)
	{
		sel_inst->CheckProperties();
	}
	else
	{
		SceneAsset::CheckProperties();
	}
}

void BlueprintAsset::OnDragObjectFromTreeView(bool is_assets_tree, SceneObject* object, Vector2 ms)
{
	if (sel_inst && sel_inst != this)
	{
		sel_inst->OnDragObjectFromTreeView(is_assets_tree, object, ms);
	}
}

void BlueprintAsset::OnMouseMove(Vector2 delta_ms)
{
	if (sel_inst && sel_inst != this)
	{
		sel_inst->OnMouseMove(delta_ms);
	}
}

void BlueprintAsset::OnLeftMouseDown(Vector2 ms)
{
	if (sel_inst && sel_inst != this)
	{
		sel_inst->OnLeftMouseDown(ms);
	}
}

void BlueprintAsset::OnLeftMouseUp()
{
	if (sel_inst && sel_inst != this)
	{
		sel_inst->OnLeftMouseUp();
	}
}

SceneObject* BlueprintAsset::CreateObject(const char* type)
{
	auto inst = sub_scene.CreateObject(type, false);
	inst->parent_uid = uid;
	childs.push_back(inst);

	return inst;
}
#endif

void BlueprintAsset::Release()
{
	for (auto child : childs)
	{
		child->Release();
	}

	SceneAsset::Release();
}
