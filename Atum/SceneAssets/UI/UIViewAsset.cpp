#include "UIViewAsset.h"
#include "SceneObjects/RenderLevels.h"
#include "UIViewInstanceAsset.h"

CLASSREG(SceneAsset, UIViewAsset, "UIView")

META_DATA_DESC(UIViewAsset)
FLOAT_PROP(UIViewAsset, trans.pos.x, 300.0f, "Prop", "x")
FLOAT_PROP(UIViewAsset, trans.pos.y, 300.0f, "Prop", "y")
FLOAT_PROP(UIViewAsset, trans.size.x, 500.0f, "Prop", "width")
FLOAT_PROP(UIViewAsset, trans.size.y, 500.0f, "Prop", "height")
FLOAT_PROP(UIViewAsset, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UIViewAsset, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UIViewAsset, abs_anchor, true, "Prop", "anchor_abs")
BOOL_PROP(UIViewAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIViewAsset, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

EUITreeView *  UIViewAsset::asset_treeview = nullptr;
EUICategories* UIViewAsset::cat = nullptr;
EUIEditBox*    UIViewAsset::objName = nullptr;
UIWidgetAsset* UIViewAsset::sel_ui_asset = nullptr;
UIWidgetAsset* UIViewAsset::asset_to_copy = nullptr;
int            UIViewAsset::popup_child_index = -1;
UIWidgetAsset* UIViewAsset::popup_item = nullptr;

void UIViewAsset::Init()
{
	RenderTasks(true)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&UIViewAsset::Draw);
}

void UIViewAsset::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewAsset::Draw(float dt)
{
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
	for (auto inst : instances)
	{
		inst->DeleteChilds();
		if (inst->parent)
		{
			inst->parent->DeleteChild(inst);
		}
		delete inst;
	}

	DeleteChilds();

	UIWidgetAsset::Release();
}

SceneObject* UIViewAsset::CreateInstance()
{
	UIViewInstanceAsset* child = (UIViewInstanceAsset*)owner->AddObject("UIViewInstance", false);

	child->SetSource(this, true);
	child->ApplyProperties();

	child->SetName(GetName());

	ReCreteChilds(this, child, true);

	return child;
}

#ifdef EDITOR
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
}

bool UIViewAsset::PrepareWidgets(EUITreeView* set_asset_treeview, EUICategories* set_cat, EUIEditBox* set_obj_name)
{
	asset_treeview = set_asset_treeview;
	cat = set_cat;
	objName = set_obj_name;
	sel_ui_asset = this;

	AddWidgetToTreeView(this, nullptr);

	return true;
}

bool UIViewAsset::UIViewAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index)
{
	if (item_from_assets)
	{
		if (StringUtils::IsEqual(item->className.c_str(), "UIViewAsset") && target && !IsInstance((UIViewAsset*)item, this))
		{
			UIWidgetAsset* parent = (UIWidgetAsset*)target;

			if (!parent->source)
			{
				UIViewInstanceAsset* child = new UIViewInstanceAsset();

				child->SetSource((UIViewAsset*)item, true);

				child->owner = owner;
				child->className = "UIViewInstanceAsset";
				child->Init();

				child->GetMetaData()->Prepare(child);
				child->GetMetaData()->SetDefValuesPrepare();
				child->ApplyProperties();

				parent->AddChild(child, child_index);
				child->SetName("View");

				child->item = asset_treeview->AddItem("View", 1, child, parent->item, child_index, true, "UIViewInstanceAsset");

				ReCreteChilds((UIWidgetAsset*)item, child, true);
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

	if (widget->source && !StringUtils::IsEqual(widget->className.c_str(), "UIViewInstanceAsset"))
	{
		return false;
	}

	widget->parent->childs.erase(widget->parent->childs.begin() + prev_child_index);

	widget->parent = parent_widget;
	parent_widget->childs.insert(parent_widget->childs.begin() + child_index, widget);

	for (int i=0; i<widget->instances.size(); i++)
	{
		UIWidgetAsset* widget_inst = widget->instances[i];
		UIWidgetAsset* parent_widget_inst = parent_widget->instances[i];

		widget_inst->parent->childs.erase(widget_inst->parent->childs.begin() + prev_child_index);

		widget_inst->parent = parent_widget_inst;
		parent_widget_inst->childs.insert(parent_widget_inst->childs.begin() + child_index, widget_inst);
	}

	return true;
}

void UIViewAsset::ReCreteChilds(UIWidgetAsset* source, UIWidgetAsset* dest, bool childs_as_inst)
{
	for (auto src_child : source->childs)
	{
		string className = src_child->className;

		if (childs_as_inst && !src_child->source)
		{
			className += "Inst";
		}

		UIWidgetAsset* dest_child = ClassFactoryUIWidgetAsset::Create(className.c_str());

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
		dest_child->className = className;
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

		//dest_child->item = asset_treeview->AddItem(dest_child->GetName(), 1, dest_child, dest->item, -1, true, src_child->className.c_str());

		for (auto dest_inst : dest->instances)
		{
			string inst_className = src_child->className;

			if (!src_child->source)
			{
				inst_className += "Inst";
			}

			UIWidgetAsset* dest_child_inst = ClassFactoryUIWidgetAsset::Create(inst_className.c_str());

			dest_child_inst->owner = owner;
			dest_child_inst->className = inst_className;
			dest_child_inst->Init();

			dest_child_inst->SetSource(dest_child, true);

			dest_child->GetMetaData()->Prepare(dest_child_inst);
			dest_child_inst->Copy(dest_child);

			dest_child_inst->GetMetaData()->Prepare(dest_child_inst);
			dest_child_inst->GetMetaData()->SetDefValuesPrepare();
			dest_child_inst->ApplyProperties();

			dest_inst->AddChild(dest_child_inst);
			dest_child_inst->SetName(dest_child->GetName());
		}

		ReCreteChilds(src_child, dest_child, childs_as_inst);
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
		objName->SetText(sel_ui_asset->GetName());

		sel_ui_asset->GetMetaData()->Prepare(sel_ui_asset);
		sel_ui_asset->GetMetaData()->PrepareWidgets(cat);
		sel_ui_asset->SetEditMode(true);
	}
	else
	{
		objName->SetText("");
	}
}

void UIViewAsset::AddWidgetToTreeView(UIWidgetAsset* widget, void* parent_item)
{
	widget->item = asset_treeview->AddItem(widget->GetName(), 1, widget, parent_item, -1, true, widget->className.c_str());

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
		child->className = item_classname;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->GetMetaData()->SetDefValuesPrepare();
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

		child->item = asset_treeview->AddItem(name, 1, child, (child_index == -1) ? popup_item->item : popup_item->parent->item, child_index, true, item_classname);

		string inst_className = item_classname + string("Inst");

		for (auto inst : popup_item->instances)
		{
			UIWidgetAsset* child_inst = ClassFactoryUIWidgetAsset::Create(inst_className.c_str());

			child_inst->owner = owner;
			child_inst->className = inst_className;
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child->GetMetaData()->SetDefValuesPrepare();

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValuesPrepare();
			child_inst->ApplyProperties();

			if (child_index == -1)
			{
				inst->AddChild(child_inst);
			}
			else
			{
				inst->parent->AddChild(child_inst, child_index);
			}

			child_inst->SetName(name);
		}
	}

	if (id == 2400)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(popup_item->className.c_str());

		child->owner = owner;
		child->className = popup_item->className;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(popup_item);

		popup_item->parent->AddChild(child, popup_child_index + 1);

		child->SetName(popup_item->GetName());

		child->item = asset_treeview->AddItem(popup_item->GetName(), 1, child, popup_item->parent->item, popup_child_index + 1, true, child->className.c_str());

		string inst_className = popup_item->className + string("Inst");

		for (auto inst : popup_item->instances)
		{
			UIWidgetAsset* child_inst = ClassFactoryUIWidgetAsset::Create(inst_className.c_str());

			child_inst->owner = owner;
			child_inst->className = inst_className;
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValuesPrepare();
			child_inst->ApplyProperties();

			inst->parent->AddChild(child_inst, popup_child_index + 1);

			child_inst->SetName(popup_item->GetName());
		}

		ReCreteChilds(popup_item, child, false);
	}

	if (id == 2401)
	{
		asset_to_copy = popup_item;
	}

	if (id == 2402 && asset_to_copy)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(asset_to_copy->className.c_str());

		child->owner = owner;
		child->className = asset_to_copy->className;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(asset_to_copy);

		popup_item->AddChild(child);

		child->SetName(asset_to_copy->GetName());

		child->item = asset_treeview->AddItem(child->GetName(), 1, child, popup_item->item, -1, true, child->className.c_str());

		string inst_className = asset_to_copy->className + string("Inst");

		for (auto inst : popup_item->instances)
		{
			UIWidgetAsset* child_inst = ClassFactoryUIWidgetAsset::Create(inst_className.c_str());

			child_inst->owner = owner;
			child_inst->className = inst_className;
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValuesPrepare();
			child_inst->ApplyProperties();

			inst->AddChild(child_inst);

			child_inst->SetName(asset_to_copy->GetName());
		}

		ReCreteChilds(asset_to_copy, child, false);
	}

	if (id == 2403 && asset_to_copy)
	{
		UIWidgetAsset* child = ClassFactoryUIWidgetAsset::Create(asset_to_copy->className.c_str());

		child->owner = owner;
		child->className = asset_to_copy->className;
		child->Init();

		child->GetMetaData()->Prepare(child);
		child->Copy(asset_to_copy);

		popup_item->parent->AddChild(child, popup_child_index + 1);

		child->SetName(asset_to_copy->GetName());

		child->item = asset_treeview->AddItem(asset_to_copy->GetName(), 1, child, popup_item->parent->item, popup_child_index + 1, true, child->className.c_str());

		string inst_className = asset_to_copy->className + string("Inst");

		for (auto inst : popup_item->instances)
		{
			UIWidgetAsset* child_inst = ClassFactoryUIWidgetAsset::Create(inst_className.c_str());

			child_inst->owner = owner;
			child_inst->className = inst_className;
			child_inst->Init();

			child_inst->SetSource(child, true);

			child->GetMetaData()->Prepare(child_inst);
			child_inst->Copy(child);

			child_inst->GetMetaData()->Prepare(child_inst);
			child_inst->GetMetaData()->SetDefValuesPrepare();
			child_inst->ApplyProperties();

			inst->parent->AddChild(child_inst, popup_child_index + 1);

			child_inst->SetName(asset_to_copy->GetName());
		}

		ReCreteChilds(asset_to_copy, child, false);
	}

	if (id == 2404)
	{
		for (auto inst : popup_item->instances)
		{
			inst->DeleteChilds();
			inst->parent->DeleteChild(inst);
			delete inst;
		}

		popup_item->SetSource(nullptr, true);
		popup_item->DeleteChilds();
		popup_item->parent->DeleteChild(popup_item);
		asset_treeview->DeleteItem(popup_item->item);
		delete popup_item;
	}
}

void UIViewAsset::FillPopupCreateMenu(const char* name, int id)
{
	asset_treeview->PopupMenuStartSubMenu(name);

	for (auto decl : ClassFactoryUIWidgetAsset::Decls())
	{
		if (!strstr(decl->GetName(), "Inst"))
		{
			asset_treeview->PopupMenuAddItem(id, decl->GetShortName());
		}
		id++;
	}

	asset_treeview->PopupMenuEndSubMenu();
}

void UIViewAsset::OnAssetTreeRightClick(SceneAsset* item, int child_index)
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

	asset_treeview->StartPopupMenu();

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
		asset_treeview->PopupMenuAddSeparator();
		asset_treeview->PopupMenuAddItem(2400, "Duplicate");
		asset_treeview->PopupMenuAddItem(2401, "Copy");

		if (asset_to_copy)
		{
			asset_treeview->PopupMenuAddItem(2403, "Paste");
			asset_treeview->PopupMenuAddItem(2402, "Paste as child");
		}

		asset_treeview->PopupMenuAddItem(2404, "Delete");
	}
}

void UIViewAsset::CheckProperties()
{
	if (sel_ui_asset != this && !StringUtils::IsEqual(sel_ui_asset->GetName(), "UIViewInstanceAsset"))
	{
		if (sel_ui_asset->GetMetaData()->IsValueWasChanged())
		{
			for (auto inst : sel_ui_asset->instances)
			{
				inst->StoreProperties();

				sel_ui_asset->GetMetaData()->Prepare(inst);
				inst->Copy(sel_ui_asset);

				inst->RestoreProperties();
				inst->ApplyProperties();
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