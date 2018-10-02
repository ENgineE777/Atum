#include "UIContainerAsset.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(UIWidgetAsset, UIContainerAsset, "Container")

META_DATA_DESC(UIContainerAsset)
BASE_WIDGET_ASSET_PROP(UIContainerAsset)
FLOAT_PROP(UIContainerAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(UIContainerAsset, trans.pos.y, 0.0f, "Prop", "y")
ENUM_PROP(UIContainerAsset, horzAlign, 0, "Prop", "horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIContainerAsset, vertAlign, 0, "Prop", "vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIContainerAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(UIContainerAsset, trans.size.y, 100.0f, "Prop", "height")
ENUM_PROP(UIContainerAsset, horzSize, 0, "Prop", "horz_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIContainerAsset, vertSize, 0, "Prop", "vert_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIContainerAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x")
FLOAT_PROP(UIContainerAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y")
FLOAT_PROP(UIContainerAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UIContainerAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UIContainerAsset, right_padding.x, 0.0f, "Prop", "right_padding")
FLOAT_PROP(UIContainerAsset, right_padding.y, 0.0f, "Prop", "bottom_padding")
FLOAT_PROP(UIContainerAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UIContainerAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIContainerAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UIContainerAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIContainerAsset, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

void UIContainerAsset::Init()
{
}

void UIContainerAsset::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	if (state == Invisible)
	{
		return;
	}

	CalcState();

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

CLASSREG(UIWidgetAsset, UIContainerAssetInst, "UIContainer")

META_DATA_DESC(UIContainerAssetInst)
BASE_WIDGET_INST_PROP(UIContainerAssetInst)
COLOR_PROP(UIContainerAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIContainerAssetInst, color.a, 1.0f, "Prop", "alpha")
META_DATA_DESC_END()

void UIContainerAssetInst::BindClassToScript()
{
	BIND_INST_TYPE_TO_SCRIPT(UIContainerAssetInst, UIContainerAsset)
}

#ifdef EDITOR
UIContainerAssetInst* UIContainerAssetInst::temp = nullptr;

bool UIContainerAssetInst::AddedToTreeByParent()
{
	return true;
}

void UIContainerAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UIContainerAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UIContainerAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif