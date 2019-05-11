#include "UIViewInstanceAsset.h"

#ifdef EDITOR
#include "Editor/project.h"
#endif

CLASSREG(UIWidgetAsset, UIViewInstanceAsset, "UIViewInst")

META_DATA_DESC(UIViewInstanceAsset)
BASE_WIDGET_ASSET_PROP(UIViewInstanceAsset)
FLOAT_PROP(UIViewInstanceAsset, trans.pos.x, 00.0f, "Prop", "x", "X coordinate of a position")
FLOAT_PROP(UIViewInstanceAsset, trans.pos.y, 00.0f, "Prop", "y", "Y coordinate of a position")
FLOAT_PROP(UIViewInstanceAsset, trans.size.x, 500.0f, "Prop", "width", "Width of a widget")
FLOAT_PROP(UIViewInstanceAsset, trans.size.y, 500.0f, "Prop", "height", "Height of a widget")
ENUM_PROP(UIViewInstanceAsset, horzSize, 0, "Prop", "horz_size", "Type of width of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIViewInstanceAsset, vertSize, 0, "Prop", "vert_size", "Type of height of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIViewInstanceAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
FLOAT_PROP(UIViewInstanceAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
BOOL_PROP(UIViewInstanceAsset, scaleChilds, false, "Prop", "scale_childs", "Should be childs sacled in case size of a widget was changed")
BOOL_PROP(UIViewInstanceAsset, clipChilds, false, "Prop", "clip_childs", "Should be childs clipped by size of a widget")
META_DATA_DESC_END()


void UIViewInstanceAsset::Init()
{
	source_is_asset = true;
}

void UIViewInstanceAsset::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewInstanceAsset::BindClassToScript()
{
	const char* brief = "UIViewInst\n"
		"\n"
		"UI view which is always a root widget.\n"
		"\n"
		"This class ::UIViewInstanceAsset is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(UIViewInstanceAsset, brief)
}

void UIViewInstanceAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	CalcState();

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

void UIViewInstanceAsset::Release()
{
	UIWidgetAsset::Release();
}

#ifdef EDITOR
void UIViewInstanceAsset::SaveAssetData(JSONWriter& writer)
{
	writer.StartBlock(nullptr);

	writer.Write("asset_uid", source->GetUID());
	writer.Write("asset_name", source->GetName());

	writer.StartArray("instances");

	writer.StartBlock(nullptr);

	writer.Write("scene", GetOwner()->project_scene_path);

	uint32_t inst_uid = 0;
	uint32_t inst_child_uid = 0;

	GetUIDs(inst_uid, inst_child_uid);

	writer.Write("inst_uid", inst_uid);
	writer.Write("inst_child_uid", inst_child_uid);
	writer.Write("is_asset", IsAsset());

	writer.Write("inst_name", GetName());

	writer.FinishBlock();

	writer.FinishArray();

	writer.FinishBlock();

	UIWidgetAsset::SaveAssetData(writer);
}
#endif

CLASSREG(SceneObject, UIViewInstance, "UIView")

META_DATA_DESC(UIViewInstance)
STRING_PROP(UIViewInstance, name, "", "Common", "Name")
BASE_SCENE_OBJ_STATE_PROP(UIViewInstance)
STRING_PROP(UIViewInstance, group_name, "", "Common", "Group")
FLOAT_PROP(UIViewInstance, trans.pos.x, 00.0f, "Prop", "x", "X coordinate of a position")
FLOAT_PROP(UIViewInstance, trans.pos.y, 00.0f, "Prop", "y", "Y coordinate of a position")
ENUM_PROP(UIViewInstance, horzAlign, 0, "Prop", "horz_align", "Horizontal aligment of a widget")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIViewInstance, vertAlign, 0, "Prop", "vert_align", "Vertical aligment of a widget")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIViewInstance, trans.size.x, 500.0f, "Prop", "width", "Width of a widget")
FLOAT_PROP(UIViewInstance, trans.size.y, 500.0f, "Prop", "height", "Height of a widget")
ENUM_PROP(UIViewInstance, horzSize, 0, "Prop", "horz_size", "Horizontal aligment of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIViewInstance, vertSize, 0, "Prop", "vert_size", "Vertical aligment of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIViewInstance, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
FLOAT_PROP(UIViewInstance, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
BOOL_PROP(UIViewInstance, scaleChilds, false, "Prop", "scale_childs", "Transparancy of a widget")
BOOL_PROP(UIViewInstance, clipChilds, false, "Prop", "clip_childs", "Should be childs sacled in case size of a widget was changed")
META_DATA_DESC_END()

void UIViewInstance::Init()
{
	source_is_asset = true;
	RenderTasks(false)->AddTask(ExecuteLevels::GUI, this, (Object::Delegate)&UIViewInstance::Draw);
}

void UIViewInstance::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewInstance::BindClassToScript()
{
	const char* brief = "UIView\n"
		"\n"
		"UI view which is always a root widget.\n"
		"\n"
		"This class ::UIViewInstance is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(UIViewInstance, brief)

	for (const auto& decl : ClassFactoryUIWidgetAsset::Decls())
	{
		SceneObject* obj = decl->Create();
		obj->script_class_name = decl->GetShortName();
		obj->BindClassToScript();
		obj->Release();
	}
}

#ifdef EDITOR
bool UIViewInstance::IsAsset()
{
	return false;
}

bool UIViewInstance::UsingCamera2DPos()
{
	return false;
}

void UIViewInstance::AddWidgetToTreeView(EUITreeView* treeview, UIWidgetAsset* widget, void* parent_item)
{
	Project::SceneTreeItem* tree_item = new Project::SceneTreeItem(widget);
	tree_item->item = treeview->AddItem(widget->GetName(), 1, tree_item, parent_item, -1, true, widget->class_name);
	widget->item = tree_item->item;
	treeview->SetABSortChilds(widget->item, false);

	for (auto child : widget->childs)
	{
		AddWidgetToTreeView(treeview, child, widget->item);
	}
}

void UIViewInstance::AddChildsToTree(EUITreeView* treeview)
{
	treeview->SetABSortChilds(item, false);

	for (auto child : childs)
	{
		AddWidgetToTreeView(treeview, child, item);
	}
}
#endif