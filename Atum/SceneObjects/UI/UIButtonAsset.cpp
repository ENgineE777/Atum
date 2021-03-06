#include "UIButtonAsset.h"

CLASSREG(UIWidgetAsset, UIButtonAsset, "Button")

META_DATA_DESC(UIButtonAsset)
BASE_WIDGET_ASSET_PROP(UIButtonAsset)
FILENAME_PROP(UIButtonAsset, sound_name, "", "Prop", "SoundName")
FLOAT_PROP(UIButtonAsset, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
FLOAT_PROP(UIButtonAsset, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
ENUM_PROP(UIButtonAsset, horzAlign, 0, "Prop", "horz_align", "Horizontal aligment of a widget")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIButtonAsset, vertAlign, 0, "Prop", "vert_align", "Vertical aligment of a widget")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIButtonAsset, trans.size.x, 100.0f, "Prop", "width", "Width of a widget")
FLOAT_PROP(UIButtonAsset, trans.size.y, 100.0f, "Prop", "height", "Height of a widget")
ENUM_PROP(UIButtonAsset, horzSize, 0, "Prop", "horz_size", "Type of width of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIButtonAsset, vertSize, 0, "Prop", "vert_size", "Type of height of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIButtonAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
FLOAT_PROP(UIButtonAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
FLOAT_PROP(UIButtonAsset, left_padding.x, 0.0f, "Prop", "left_padding", "Left padding of a widget")
FLOAT_PROP(UIButtonAsset, left_padding.y, 0.0f, "Prop", "top_padding", "Top padding of a widget")
FLOAT_PROP(UIButtonAsset, right_padding.x, 0.0f, "Prop", "right_padding", "Right padding of a widget")
FLOAT_PROP(UIButtonAsset, right_padding.y, 0.0f, "Prop", "bottom_padding", "Bottom padding of a widget")
FLOAT_PROP(UIButtonAsset, trans.rotation, 0.0f, "Prop", "rotate", "Rotation a widget")
COLOR_PROP(UIButtonAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIButtonAsset, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
BOOL_PROP(UIButtonAsset, scaleChilds, false, "Prop", "scale_childs", "Should be childs sacled in case size of a widget was changed")
BOOL_PROP(UIButtonAsset, clipChilds, false, "Prop", "clip_childs", "Should be childs clipped by size of a widget")
META_DATA_DESC_END()

void UIButtonAsset::Init()
{
}

void UIButtonAsset::Release()
{
	RELEASE(sound_click)

	UIWidgetAsset::Release();
}

void UIButtonAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	CalcState();

	UIWidgetAsset::Draw(dt);
}

CLASSREG(UIWidgetAsset, UIButtonAssetInst, "UIButton")

META_DATA_DESC(UIButtonAssetInst)
BASE_WIDGET_INST_PROP(UIButtonAssetInst)
COLOR_PROP(UIButtonAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIButtonAssetInst, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
META_DATA_DESC_END()

void UIButtonAssetInst::BindClassToScript()
{
	const char* brief = "UI Button instance\n"
		"\n"
		"UI button that calls script method OnDown when button is clicked.\n"
		"Also a button do not have any graphical representation.\n"
		"\n"
		"This class ::UIButtonAssetInst is a representation on C++ side.\n";

	BIND_INST_TYPE_TO_SCRIPT(UIButtonAssetInst, UIButtonAsset, brief)
}

void UIButtonAssetInst::Init()
{
	alias_move_x = core.controls.GetAlias("UIButton.Move_X");
	alias_move_y = core.controls.GetAlias("UIButton.Move_Y");
	alias_click = core.controls.GetAlias("UIButton.Click");

	script_callbacks.push_back(ScriptCallback(GetScene(), "OnDown", "void", ""));
}

void UIButtonAssetInst::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	if (GetScene()->Playing())
	{
		if (core.controls.GetAliasState(alias_click))
		{
			float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
			Matrix mat = trans.mat_global;

			Vector2 pos = trans.offset * trans.size * -1.0f;
			pos.x += mat.Pos().x;
			pos.y += mat.Pos().y;

			pos *= scale;

			Vector2 size = trans.size * scale;

			float mx = core.controls.GetAliasValue(alias_move_x, false);
			float my = core.controls.GetAliasValue(alias_move_y, false);

			if (pos.x < mx && mx < pos.x + size.x &&
				pos.y < my && my < pos.y + size.y)
			{
				if (!sound_name.empty() && !sound_click)
				{
					sound_click = core.sounds.CreateSound(GetScene(), sound_name.c_str());
				}

				if (sound_click)
				{
					sound_click->Play(SoundBase::Once);
				}

				core.controls.SupressAlias(alias_click);

				SceneObject::ScriptCallback* callabck = FindScriptCallback("OnDown");

				if (callabck)
				{
					callabck->Call(Script());
				}
			}
		}
	}

	UIButtonAsset::Draw(dt);
}

#ifdef EDITOR
UIButtonAssetInst* UIButtonAssetInst::temp = nullptr;

bool UIButtonAssetInst::AddedToTreeByParent()
{
	return true;
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