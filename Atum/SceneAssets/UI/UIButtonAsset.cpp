#include "UIButtonAsset.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(UIWidgetAsset, UIButtonAsset, "Button")

META_DATA_DESC(UIButtonAsset)
FLOAT_PROP(UIButtonAsset, trans.pos.x, 100.0f, "Prop", "x")
FLOAT_PROP(UIButtonAsset, trans.pos.y, 100.0f, "Prop", "y")
ENUM_PROP(UIButtonAsset, horzAlign, 0, "Prop", "horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIButtonAsset, vertAlign, 0, "Prop", "vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIButtonAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(UIButtonAsset, trans.size.y, 100.0f, "Prop", "height")
ENUM_PROP(UIButtonAsset, horzSize, 0, "Prop", "horz_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIButtonAsset, vertSize, 0, "Prop", "vert_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIButtonAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UIButtonAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UIButtonAsset, right_padding.x, 100.0f, "Prop", "right_padding")
FLOAT_PROP(UIButtonAsset, right_padding.y, 100.0f, "Prop", "bottom_padding")
FLOAT_PROP(UIButtonAsset, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UIButtonAsset, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UIButtonAsset, abs_anchor, true, "Prop", "anchor_abs")
FLOAT_PROP(UIButtonAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UIButtonAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIButtonAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UIButtonAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIButtonAsset, clipChilds, false, "Prop", "clip_childs")
META_DATA_DESC_END()

void UIButtonAsset::Init()
{
}

void UIButtonAsset::Draw(float dt)
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

CLASSREG(UIWidgetAsset, UIButtonAssetInst, "ButtonInst")

META_DATA_DESC(UIButtonAssetInst)
COLOR_PROP(UIButtonAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIButtonAssetInst, color.a, 1.0f, "Prop", "alpha")
META_DATA_DESC_END()

#ifdef EDITOR
UIButtonAssetInst* UIButtonAssetInst::temp = nullptr;

void UIButtonAssetInst::Init()
{
	alias_rotate_x = controls.GetAlias("Tank.ROTATE_X");
	alias_rotate_y = controls.GetAlias("Tank.ROTATE_Y");
	alias_fire = controls.GetAlias("Tank.FIRE");
}

void UIButtonAssetInst::Draw(float dt)
{
	if (Playing())
	{
		if (controls.GetAliasState(alias_fire))
		{
			float scale = render.GetDevice()->GetHeight() / 1024.0f;
			Matrix mat = trans.mat_global;

			Vector2 pos = trans.offset * trans.size * -1.0f;
			pos.x += mat.Pos().x;
			pos.y += mat.Pos().y;

			pos *= scale;

			Vector2 size = trans.size * scale;

			float mx = controls.GetAliasValue(alias_rotate_x, false);
			float my = controls.GetAliasValue(alias_rotate_y, false);

			if (pos.x < mx && mx < pos.x + size.x &&
				pos.y < my && my < pos.y + size.y)
			{
				fired = 1;
			}
		}
	}

	UIButtonAsset::Draw(dt);
}

void UIButtonAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UIButtonAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UIButtonAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif