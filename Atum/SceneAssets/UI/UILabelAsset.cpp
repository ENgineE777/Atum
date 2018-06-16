#include "UILabelAsset.h"
#include "Editor/Gizmo.h"
#include "SceneObjects/RenderLevels.h"
#include "Services/Font/Fonts.h"

CLASSREG(UIWidgetAsset, UILabelAsset, "Label")

META_DATA_DESC(UILabelAsset)
FLOAT_PROP(UILabelAsset, trans.pos.x, 100.0f, "Prop", "x")
FLOAT_PROP(UILabelAsset, trans.pos.y, 100.0f, "Prop", "y")
ENUM_PROP(UILabelAsset, horzAlign, 0, "Prop", "horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UILabelAsset, vertAlign, 0, "Prop", "vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UILabelAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(UILabelAsset, trans.size.y, 100.0f, "Prop", "height")
ENUM_PROP(UILabelAsset, horzSize, 0, "Prop", "horz_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UILabelAsset, vertSize, 0, "Prop", "vert_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UILabelAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UILabelAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UILabelAsset, right_padding.x, 100.0f, "Prop", "right_padding")
FLOAT_PROP(UILabelAsset, right_padding.y, 100.0f, "Prop", "bottom_padding")
FLOAT_PROP(UILabelAsset, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UILabelAsset, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UILabelAsset, abs_anchor, true, "Prop", "anchor_abs")
FLOAT_PROP(UILabelAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UILabelAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UILabelAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UILabelAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UILabelAsset, clipChilds, false, "Prop", "clip_childs")
STRING_PROP(UILabelAsset, font_name, "settings/helvetica", "Prop", "font_name")
INT_PROP(UILabelAsset, font_height, 15, "Prop", "font_height")
STRING_PROP(UILabelAsset, text, "Label", "Prop", "text")
META_DATA_DESC_END()

void UILabelAsset::Init()
{
}

void UILabelAsset::ApplyProperties()
{
	RELEASE(font)
	font = fonts.LoadFont(font_name.c_str(), false, false, font_height);
}

void UILabelAsset::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	CalcState();

	float scale = render.GetDevice()->GetHeight() / 1024.0f;
	Matrix mat = trans.mat_global;
	mat.Pos().x *= scale;
	mat.Pos().y *= scale;

	font->Print(mat, 1.0f, cur_color, text.c_str());

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

#ifdef EDITOR
void UILabelAsset::SetEditMode(bool ed)
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

CLASSREG(UIWidgetAsset, UILabelAssetInst, "LabelInst")

META_DATA_DESC(UILabelAssetInst)
COLOR_PROP(UILabelAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UILabelAssetInst, color.a, 1.0f, "Prop", "alpha")
STRING_PROP(UILabelAssetInst, text, "Label", "Prop", "text")
META_DATA_DESC_END()

#ifdef EDITOR
UILabelAssetInst* UILabelAssetInst::temp = nullptr;

void UILabelAssetInst::SetEditMode(bool ed)
{
	UIWidgetAsset::SetEditMode(ed);

	Gizmo::inst->allow_transform = !ed;
	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
	}

	Gizmo::inst->allow_transform = !ed;
}

void UILabelAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UILabelAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UILabelAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif