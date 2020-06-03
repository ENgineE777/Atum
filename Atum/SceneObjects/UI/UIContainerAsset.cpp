#include "UIContainerAsset.h"

CLASSREG(UIWidgetAsset, UIContainerAsset, "Container")

META_DATA_DESC(UIContainerAsset)
BASE_WIDGET_ASSET_PROP(UIContainerAsset)
FLOAT_PROP(UIContainerAsset, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
FLOAT_PROP(UIContainerAsset, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
ENUM_PROP(UIContainerAsset, horzAlign, 0, "Prop", "horz_align", "Horizontal aligment of a widget")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIContainerAsset, vertAlign, 0, "Prop", "vert_align", "Vertical aligment of a widget")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIContainerAsset, trans.size.x, 100.0f, "Prop", "width", "Width of a widget")
FLOAT_PROP(UIContainerAsset, trans.size.y, 100.0f, "Prop", "height", "Height of a widget")
ENUM_PROP(UIContainerAsset, horzSize, 0, "Prop", "horz_size", "Type of width of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIContainerAsset, vertSize, 0, "Prop", "vert_size", "Type of height of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIContainerAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
FLOAT_PROP(UIContainerAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
FLOAT_PROP(UIContainerAsset, left_padding.x, 0.0f, "Prop", "left_padding", "Left padding of a widget")
FLOAT_PROP(UIContainerAsset, left_padding.y, 0.0f, "Prop", "top_padding", "Top padding of a widget")
FLOAT_PROP(UIContainerAsset, right_padding.x, 0.0f, "Prop", "right_padding", "Right padding of a widget")
FLOAT_PROP(UIContainerAsset, right_padding.y, 0.0f, "Prop", "bottom_padding", "Bottom padding of a widget")
FLOAT_PROP(UIContainerAsset, rotate, 0.0f, "Prop", "rotate", "Rotation a widget")
COLOR_PROP(UIContainerAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIContainerAsset, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
BOOL_PROP(UIContainerAsset, scaleChilds, false, "Prop", "scale_childs", "Should be childs sacled in case size of a widget was changed")
BOOL_PROP(UIContainerAsset, clipChilds, false, "Prop", "clip_childs", "Should be childs clipped by size of a widget")
META_DATA_DESC_END()

void UIContainerAsset::Init()
{
}

void UIContainerAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	CalcState();

	UIWidgetAsset::Draw(dt);
}

CLASSREG(UIWidgetAsset, UIContainerAssetInst, "UIContainer")

META_DATA_DESC(UIContainerAssetInst)
BASE_WIDGET_INST_PROP(UIContainerAssetInst)
COLOR_PROP(UIContainerAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIContainerAssetInst, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
META_DATA_DESC_END()

void UIContainerAssetInst::BindClassToScript()
{
	const char* brief = "UI Button instance\n"
		"\n"
		"UI container that just holds another widgets as childs.\n"
		"\n"
		"This class ::UIContainerAssetInst is a representation on C++ side.\n";

	BIND_INST_TYPE_TO_SCRIPT(UIContainerAssetInst, UIContainerAsset, brief)
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