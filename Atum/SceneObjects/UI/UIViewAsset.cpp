#include "UIViewAsset.h"
#include "UIViewInstanceAsset.h"

#ifdef EDITOR
#include "Editor/project.h"
#endif

CLASSREG(SceneAsset, UIViewAsset, "UIView")

META_DATA_DESC(UIViewAsset)
STRING_PROP(UIViewAsset, name, "", "Common", "Name")\
FLOAT_PROP(UIViewAsset, trans.pos.x, 300.0f, "Prop", "x")
FLOAT_PROP(UIViewAsset, trans.pos.y, 300.0f, "Prop", "y")
FLOAT_PROP(UIViewAsset, trans.size.x, 500.0f, "Prop", "width")
FLOAT_PROP(UIViewAsset, trans.size.y, 500.0f, "Prop", "height")
BOOL_PROP(UIViewAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIViewAsset, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

#ifdef EDITOR
UIWidgetAsset* UIViewAsset::sel_ui_asset = nullptr;
UIWidgetAsset* UIViewAsset::asset_to_copy = nullptr;
int            UIViewAsset::popup_child_index = -1;
UIWidgetAsset* UIViewAsset::popup_item = nullptr;
#endif

void UIViewAsset::Init()
{
#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::GUI, this, (Object::Delegate)&UIViewAsset::Draw);
#endif
}

void UIViewAsset::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	trans.offset = 0.0f;
	trans.BuildMatrices();

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

void UIViewAsset::Release()
{
#ifdef EDITOR
	for (auto inst : instances)
	{
		UIWidgetAsset* ui_inst = (UIWidgetAsset*)inst.GetObject();
		ui_inst->DeleteChilds();
		if (ui_inst->parent)
		{
			ui_inst->parent->DeleteChild(ui_inst);
		}
		delete ui_inst;
	}
#endif

	DeleteChilds();

	UIWidgetAsset::Release();
}

#ifdef EDITOR
SceneObject* UIViewAsset::CreateInstance(Scene* scene)
{
	UIViewInstanceAsset* child = (UIViewInstanceAsset*)scene->CreateObject("UIViewInstance", false);

	child->SetSource(this, true);
	child->ApplyProperties();

	child->SetName(GetName());

	ReCreteChilds(this, child, true, false);

	return child;
}

bool UIViewAsset::UseAseetsTree()
{
	return true;
}

void UIViewAsset::SetEditMode(bool ed)
{
	UIWidgetAsset::SetEditMode(ed);

	if (!ed)
	{
		if (sel_ui_asset && sel_ui_asset != this)
		{
			sel_ui_asset->GetMetaData()->HideWidgets();
			sel_ui_asset->SetEditMode(false);
		}
	}
	else
	{
		sel_ui_asset = this;
	}
}

void UIViewAsset::PreapreAssetTree()
{
	AddWidgetToTreeView(this, nullptr);
}

bool UIViewAsset::UIViewAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index)
{
	if (item_from_assets)
	{
		if (StringUtils::IsEqual(item->class_name, "UIViewAsset") && target && !IsInstance((UIViewAsset*)item, this))
		{
			UIWidgetAsset* parent = (UIWidgetAsset*)target;

			if (!parent->source)
			{
				auto decl = ClassFactoryUIWidgetAsset::Find("UIViewInstanceAsset");
				UIViewInstanceAsset* child = (UIViewInstanceAsset*)decl->Create();

				child->SetSource((UIViewAsset*)item, true);

				child->owner = owner;
				child->class_name = decl->GetName();
				child->Init();

				child->GetMetaData()->Prepare(child);
				child->GetMetaData()->SetDefValues();
				child->ApplyProperties();

				parent->AddChild(child, child_index);
				child->SetName("View");

				Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
				tree_item->item = ed_asset_treeview->AddItem("View", 1, tree_item, parent->item, child_index, true, "UIViewInstanceAsset");
				child->item = tree_item->item;

				ReCreteChilds((UIWidgetAsset*)item, child, true, true);
			}
		}

		return false;
	}

	UIWidgetAsset* parent_widget = (UIWidgetAsset*)target;

	if (!parent_widget)
	{
		return false;
	}

	UIWidgetAsset* widget = (UIWidgetAsset*)item;

	if (widget->source && !StringUtils::IsEqual(widget->class_name, "UIViewInstanceAsset"))
	{
		return false;
	}

	widget->parent->childs.erase(widget->parent->childs.begin() + prev_child_index);

	widget->parent = parent_widget;

	if (widget->parent == parent_widget && child_index != 0)
	{
		child_index--;
	}

	parent_widget->childs.insert(parent_widget->childs.begin() + child_index, widget);

	for (int i=0; i<widget->instances.size(); i++)
	{
		UIWidgetAsset* widget_inst = (UIWidgetAsset*)widget->instances[i].GetObject();
		UIWidgetAsset* parent_widget_inst = (UIWidgetAsset*)parent_widget->instances[i].GetObject();

		widget_inst->parent->childs.erase(widget_inst->parent->childs.begin() + prev_child_index);

		widget_inst->parent = parent_widget_inst;
		parent_widget_inst->childs.insert(parent_widget_inst->childs.begin() + child_index, widget_inst);
	}

	return true;
}

void UIViewAsset::ReCreteChilds(UIWidgetAsset* source, UIWidgetAsset* dest, bool childs_as_inst, bool create_item)
{
	for (auto src_child : source->childs)
	{
		string className = src_child->class_name;

		if (childs_as_inst && !src_child->source)
		{
			className += "Inst";
		}

		auto decl = ClassFactoryUIWidgetAsset::Find(className.c_str());

		UIWidgetAsset* dest_child = decl->Create();

		if (childs_as_inst)
		{
			if (src_child->source)
			{
				dest_child->SetSource(src_child->source, true);
			}
			else
			{
				dest_child->SetSource(src_child, true);
			}
		}

		dest_child->owner = owner;
		dest_child->class_name = decl->GetName();
		dest_child->Init();

		if (src_child->source)
		{
			src_child->source->GetMetaData()->Prepare(dest_child);
			dest_child->Copy(src_child->source);
		}

		src_child->GetMetaData()->Prepare(dest_child);
		dest_child->Copy(src_child);

		dest->AddChild(dest_child);
		dest_child->SetName(src_child->GetName());

		if (create_item)
		{
			Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(dest_child);
			tree_item->item = ed_asset_treeview->AddItem(dest_child->GetName(), 1, tree_item, dest->item, -1, true, src_child->class_name);
			dest_child->item = tree_item->item;
		}

		for (auto dest_inst : dest->instances)
		{
			string inst_className = src_child->class_name;

			if (!src_child->source)
			{
				inst_className += "Inst";
			}

			auto decl = ClassFactoryUIWidgetAsset::Find(inst_className.c_str());
			UIWidgetAsset* dest_child_inst = decl->Create();

			dest_child_inst->owner = owner;
			dest_child_inst->class_name = decl->GetName();
			dest_child_inst->Init();

			dest_child_inst->SetSource(dest_child, true);

			dest_child->GetMetaData()->Prepare(dest_child_inst);
			dest_child_inst->Copy(dest_child);

			dest_child_inst->GetMetaData()->Prepare(dest_child_inst);
			dest_child_inst->GetMetaData()->SetDefValues();
			dest_child_inst->ApplyProperties();

			((UIWidgetAsset*)dest_inst.GetObject())->AddChild(dest_child_inst);
			dest_child_inst->SetName(dest_child->GetName());
		}

		ReCreteChilds(src_child, dest_child, childs_as_inst, create_item);
	}
}

void UIViewAsset::OnAssetTreeSelChange(SceneAsset* item)
{
	if (sel_ui_asset)
	{
		sel_ui_asset->GetMetaData()->HideWidgets();
		sel_ui_asset->SetEditMode(false);
	}

	sel_ui_asset = (UIWidgetAsset*)item;

	if (sel_ui_asset)
	{
		sel_ui_asset->GetMetaData()->Prepare(sel_ui_asset);
		sel_ui_asset->GetMetaData()->PrepareWidgets(ed_obj_cat);
		sel_ui_asset->SetEditMode(true);
	}
}

void UIViewAsset::AddWidgetToTreeView(UIWidgetAsset* widget, void* parent_item)
{
	Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(widget);
	tree_item->item = ed_asset_treeview->AddItem(widget->GetName(), 1, tree_item, parent_item, -1, true, widget->class_name);
	widget->item = tree_item->item;

	for (auto child : widget->childs)
	{
		AddWidgetToTreeView(child, widget->item);
	}
}

void UIViewAsset::OnAssetTreePopupItem(int id)
{
	const char* item_classname = nullptr;

	if (id >= 2500 && id < 2600)
	{
		item_classname = ClassFactoryUIWidgetAsset::Decls()[id - 2500]->GetName();
	}

	if (id >= 2600 && id < 2700)
	{
		item_classname = ClassFactoryUIWidgetAsset::Decls()[id - 2600]->GetName();
	}

	if (item_classname)
	{
		int child_index = (id >= 2600) ? popup_child_index + 1 : -1;

		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(item_classname);

		child->owner = owner;
		child->class_name = item_classname;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->GetMetaData()->SetDefValues();
		child->ApplyProperties();

		if (child_index == -1)
		{
			popup_item->AddChild(child);
		}
		else
		{
			popup_item->parent->AddChild(child, child_index);
		}

		const char* name = (id >= 2600) ? ClassFactoryUIWidgetAsset::Decls()[id - 2600]->GetShortName() : ClassFactoryUIWidgetAsset::Decls()[id - 2500]->GetShortName();

		child->SetName(name);

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = ed_asset_treeview->AddItem(name, 1, tree_item, (child_index == -1) ? popup_item->item : popup_item->parent->item, child_index, true, item_classname);
		child->item = tree_item->item;

		string inst_className = item_classname + string("Inst");

		for (auto inst : popup_item->instances)
		{
			auto decl = ClassFactoryUIWidgetAsset::Find(inst_className.c_str());
			UIWidgetAsset* child_inst = decl->Create();

			child_inst->owner = owner;
			child_inst->class_name = decl->GetName();
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child->GetMetaData()->SetDefValues();

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValues();
			child_inst->ApplyProperties();

			if (child_index == -1)
			{
				((UIWidgetAsset*)inst.GetObject())->AddChild(child_inst);
			}
			else
			{
				((UIWidgetAsset*)inst.GetObject())->parent->AddChild(child_inst, child_index);
			}

			child_inst->SetName(name);
		}
	}

	if (id == 2400)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(popup_item->class_name);

		child->owner = owner;
		child->class_name = popup_item->class_name;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(popup_item);

		popup_item->parent->AddChild(child, popup_child_index + 1);

		child->SetName(popup_item->GetName());

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = ed_asset_treeview->AddItem(popup_item->GetName(), 1, tree_item, popup_item->parent->item, popup_child_index + 1, true, child->class_name);
		child->item = tree_item->item;

		string inst_className = popup_item->class_name + string("Inst");

		for (auto inst : popup_item->instances)
		{
			auto decl = ClassFactoryUIWidgetAsset::Find(inst_className.c_str());
			UIWidgetAsset* child_inst = decl->Create();


			child_inst->owner = owner;
			child_inst->class_name = decl->GetName();
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValues();
			child_inst->ApplyProperties();

			((UIWidgetAsset*)inst.GetObject())->parent->AddChild(child_inst, popup_child_index + 1);

			child_inst->SetName(popup_item->GetName());
		}

		ReCreteChilds(popup_item, child, false, true);
	}

	if (id == 2401)
	{
		asset_to_copy = popup_item;
	}

	if (id == 2402 && asset_to_copy)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(asset_to_copy->class_name);

		child->owner = owner;
		child->class_name = asset_to_copy->class_name;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(asset_to_copy);

		popup_item->AddChild(child);

		child->SetName(asset_to_copy->GetName());

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = ed_asset_treeview->AddItem(child->GetName(), 1, tree_item, popup_item->item, -1, true, child->class_name);
		child->item = tree_item->item;

		string inst_className = asset_to_copy->class_name + string("Inst");

		for (auto inst : popup_item->instances)
		{
			auto decl = ClassFactoryUIWidgetAsset::Find(inst_className.c_str());
			UIWidgetAsset* child_inst = decl->Create();

			child_inst->owner = owner;
			child_inst->class_name = decl->GetName();
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValues();
			child_inst->ApplyProperties();

			((UIWidgetAsset*)inst.GetObject())->AddChild(child_inst);

			child_inst->SetName(asset_to_copy->GetName());
		}

		ReCreteChilds(asset_to_copy, child, false, true);
	}

	if (id == 2403 && asset_to_copy)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(asset_to_copy->class_name);

		child->owner = owner;
		child->class_name = asset_to_copy->class_name;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(asset_to_copy);

		popup_item->parent->AddChild(child, popup_child_index + 1);

		child->SetName(asset_to_copy->GetName());

		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(child);
		tree_item->item = ed_asset_treeview->AddItem(asset_to_copy->GetName(), 1, tree_item, popup_item->parent->item, popup_child_index + 1, true, child->class_name);
		child->item = tree_item->item;

		string inst_className = asset_to_copy->class_name + string("Inst");

		for (auto inst : popup_item->instances)
		{
			auto decl = ClassFactoryUIWidgetAsset::Find(inst_className.c_str());

			UIWidgetAsset* child_inst = decl->Create();

			child_inst->owner = owner;
			child_inst->class_name = decl->GetName();
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValues();
			child_inst->ApplyProperties();

			((UIWidgetAsset*)inst.GetObject())->parent->AddChild(child_inst, popup_child_index + 1);

			child_inst->SetName(asset_to_copy->GetName());
		}

		ReCreteChilds(asset_to_copy, child, false, true);
	}

	if (id == 2404)
	{
		for (auto inst : popup_item->instances)
		{
			UIWidgetAsset* ui_inst = (UIWidgetAsset*)inst.GetObject();
			ui_inst->DeleteChilds();
			ui_inst->parent->DeleteChild(ui_inst);
			delete ui_inst;
		}

		popup_item->SetSource(nullptr, true);
		popup_item->DeleteChilds();
		popup_item->parent->DeleteChild(popup_item);
		ed_asset_treeview->DeleteItem(popup_item->item);
		delete popup_item;
	}
}

void UIViewAsset::FillPopupCreateMenu(const char* name, int id)
{
	ed_popup_menu->StartSubMenu(name);

	for (auto decl : ClassFactoryUIWidgetAsset::Decls())
	{
		if (!strstr(decl->GetName(), "Inst"))
		{
			ed_popup_menu->AddItem(id, decl->GetShortName());
		}
		id++;
	}

	ed_popup_menu->EndSubMenu();
}

void UIViewAsset::OnAssetTreeRightClick(int x, int y, SceneAsset* item, int child_index)
{
	if (item == nullptr)
	{
		return;
	}

	popup_child_index = child_index;
	popup_item = (UIWidgetAsset*)item;

	if (popup_item->parent && popup_item->parent->source)
	{
		return;
	}

	ed_popup_menu->StartMenu(true);

	if (popup_item != this)
	{
		FillPopupCreateMenu("Create", 2600);
	}

	if (!popup_item->source)
	{
		FillPopupCreateMenu("Create as child", 2500);
	}

	if (popup_item != this)
	{
		ed_popup_menu->AddSeparator();
		ed_popup_menu->AddItem(2400, "Duplicate");
		ed_popup_menu->AddItem(2401, "Copy");

		if (asset_to_copy)
		{
			ed_popup_menu->AddItem(2403, "Paste");
			ed_popup_menu->AddItem(2402, "Paste as child");
		}

		ed_popup_menu->AddItem(2404, "Delete");
	}

	ed_popup_menu->ShowAsPopup(ed_asset_treeview, x, y);
}

void UIViewAsset::CheckProperties()
{
	if (sel_ui_asset && sel_ui_asset != this && !StringUtils::IsEqual(sel_ui_asset->GetName(), "UIViewInstanceAsset"))
	{
		if (sel_ui_asset->GetMetaData()->IsValueWasChanged())
		{
			for (auto inst : sel_ui_asset->instances)
			{
				UIWidgetAsset* ui_inst = (UIWidgetAsset*)inst.GetObject();

				ui_inst->StoreProperties();

				sel_ui_asset->GetMetaData()->Prepare(ui_inst);
				ui_inst->Copy(sel_ui_asset);

				ui_inst->RestoreProperties();
				ui_inst->ApplyProperties();
			}

			ApplyProperties();
		}
	}
	else
	{
		UIWidgetAsset::CheckProperties();
	}
}

bool UIViewAsset::IsInstance(UIWidgetAsset* node, UIWidgetAsset* source)
{
	if (node->source == source)
	{
		return true;
	}

	for (auto child : node->childs)
	{
		if (IsInstance(child, source))
		{
			return true;
		}
	}

	return false;
}
#endif