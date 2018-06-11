#include "UIViewInstanceAsset.h"
#include "Editor/Gizmo.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(UIWidgetAsset, UIViewInstanceAsset)

META_DATA_DESC(UIViewInstanceAsset)
FLOAT_PROP(UIViewInstanceAsset, trans.pos.x, 00.0f, "Prop", "x")
FLOAT_PROP(UIViewInstanceAsset, trans.pos.y, 00.0f, "Prop", "y")
FLOAT_PROP(UIViewInstanceAsset, trans.size.x, 500.0f, "Prop", "width")
FLOAT_PROP(UIViewInstanceAsset, trans.size.y, 500.0f, "Prop", "height")
FLOAT_PROP(UIViewInstanceAsset, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UIViewInstanceAsset, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UIViewInstanceAsset, abs_anchor, true, "Prop", "anchor_abs")
BOOL_PROP(UIViewInstanceAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIViewInstanceAsset, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

void UIViewInstanceAsset::Init()
{
	source_is_asset = true;
}

void UIViewInstanceAsset::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewInstanceAsset::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	CalcState();

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

#ifdef EDITOR
void UIViewInstanceAsset::SetEditMode(bool ed)
{
	UIWidgetAsset::SetEditMode(ed);

	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
	}
}
#endif