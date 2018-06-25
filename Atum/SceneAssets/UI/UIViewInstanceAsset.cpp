#include "UIViewInstanceAsset.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(UIWidgetAsset, UIViewInstanceAsset, "UIViewInst")

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

void UIViewInstanceAsset::Release()
{

}

CLASSREG(SceneObject, UIViewInstance, "UIViewInstance")

META_DATA_DESC(UIViewInstance)
FLOAT_PROP(UIViewInstance, trans.pos.x, 00.0f, "Prop", "x")
FLOAT_PROP(UIViewInstance, trans.pos.y, 00.0f, "Prop", "y")
FLOAT_PROP(UIViewInstance, trans.size.x, 500.0f, "Prop", "width")
FLOAT_PROP(UIViewInstance, trans.size.y, 500.0f, "Prop", "height")
FLOAT_PROP(UIViewInstance, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UIViewInstance, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UIViewInstance, abs_anchor, true, "Prop", "anchor_abs")
BOOL_PROP(UIViewInstance, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIViewInstance, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

void UIViewInstance::Init()
{
	source_is_asset = true;
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&UIViewInstance::Draw);
}

void UIViewInstance::ApplyProperties()
{
	UIWidgetAsset::ApplyProperties();
}

void UIViewInstance::Draw(float dt)
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