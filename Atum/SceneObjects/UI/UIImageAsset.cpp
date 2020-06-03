#include "UIImageAsset.h"
#include "SceneObjects/2D/SpriteWindow.h"
#include "SceneObjects/2D/SpriteAsset.h"
#include "Services/Core/Core.h"

CLASSREG(UIWidgetAsset, UIImageAsset, "Image")

META_DATA_DESC(UIImageAsset)
BASE_WIDGET_ASSET_PROP(UIImageAsset)
FLOAT_PROP(UIImageAsset, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
FLOAT_PROP(UIImageAsset, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
ENUM_PROP(UIImageAsset, horzAlign, 0, "Prop", "horz_align", "Horizontal aligment of a widget")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIImageAsset, vertAlign, 0, "Prop", "vert_align", "Vertical aligment of a widget")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIImageAsset, trans.size.x, 100.0f, "Prop", "width", "Width of a widget")
FLOAT_PROP(UIImageAsset, trans.size.y, 100.0f, "Prop", "height", "Height of a widget")
ENUM_PROP(UIImageAsset, horzSize, 0, "Prop", "horz_size", "Type of width of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIImageAsset, vertSize, 0, "Prop", "vert_size", "Type of height of a widget")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIImageAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
FLOAT_PROP(UIImageAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
FLOAT_PROP(UIImageAsset, left_padding.x, 0.0f, "Prop", "left_padding", "Left padding of a widget")
FLOAT_PROP(UIImageAsset, left_padding.y, 0.0f, "Prop", "top_padding", "Top padding of a widget")
FLOAT_PROP(UIImageAsset, right_padding.x, 0.0f, "Prop", "right_padding", "Right padding of a widget")
FLOAT_PROP(UIImageAsset, right_padding.y, 0.0f, "Prop", "bottom_padding", "Bottom padding of a widget")
FLOAT_PROP(UIImageAsset, rotate, 0.0f, "Prop", "rotate", "Rotation a widget")
COLOR_PROP(UIImageAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAsset, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
BOOL_PROP(UIImageAsset, scaleChilds, false, "Prop", "scale_childs", "Should be childs sacled in case size of a widget was changed")
BOOL_PROP(UIImageAsset, clipChilds, false, "Prop", "clip_childs", "Should be childs clipped by size of a widget")
SPRITE_PROP(UIImageAsset, sprite, "Prop", "image")
META_DATA_DESC_END()

UIImageAsset::~UIImageAsset()
{

}

void UIImageAsset::Init()
{
}

void UIImageAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
	}

	if (sprite.texture)
	{
		Vector2 parent_size;

		if (parent)
		{
			parent_size = parent->trans.size;
		}
		else
		{
			parent_size.x = core.render.GetDevice()->GetWidth() * 1024.0f / core.render.GetDevice()->GetHeight();
			parent_size.y = 1024.0f;
		}

		parent_size.x -= (left_padding.x + right_padding.x);
		parent_size.y -= (left_padding.y + right_padding.y);

		if (horzSize == wrap_context && vertSize == wrap_context)
		{
			float k = (float)sprite.rects[0].size.x / (float)sprite.rects[0].size.y;

			if (k > 1.0f)
			{
				k = 1.0f / k;
				trans.size.x = parent_size.x;
				trans.size.y = parent_size.y * k;

				trans.pos.x = 0.0f;
				trans.pos.y = (parent_size.y - trans.size.y) * 0.5f;
			}
			else
			{
				trans.size.x = parent_size.x * k;
				trans.size.y = parent_size.y;

				trans.pos.x = (parent_size.x - trans.size.x) * 0.5f;
				trans.pos.y = 0.0f;
			}
		}
		else
		if (horzSize == wrap_context)
		{
		}
		else
		if (vertSize == wrap_context)
		{
			//trans.pos.y = 0;
			//trans.size.y = parent->trans.size.y - right_padding.y - left_padding.y;
		}
	}

	CalcState();

	Sprite::UpdateFrame(&sprite, &frame_state, dt);

#ifdef EDITOR
	/*if (Gizmo::inst->trans2D == &trans && SpriteWindow::instance && !SpriteWindow::instance->show_anim)
	{
		state.cur_frame = SpriteWindow::instance->cur_frame;
	}

	if (SpriteWindow::instance)
	{
		SpriteWindow::instance->CheckStateOfBorder();
	}*/
#endif

	Sprite::Draw(&trans, cur_color, &sprite, &frame_state, false, true);

	UIWidgetAsset::Draw(dt);
}

CLASSREG(UIWidgetAsset, UIImageAssetInst, "UIImage")

#ifdef EDITOR
void StartEditUIImageAssetInst(void* owner)
{
	UIImageAssetInst* sprite = (UIImageAssetInst*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

META_DATA_DESC(UIImageAssetInst)
BASE_WIDGET_INST_PROP(UIImageAssetInst)
COLOR_PROP(UIImageAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAssetInst, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
#ifdef EDITOR
CALLBACK_PROP(UIImageAssetInst, StartEditUIImageAssetInst, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

void UIImageAssetInst::BindClassToScript()
{
	const char* brief = "UI Button instance\n"
		"\n"
		"UI image widget.\n"
		"\n"
		"This class ::UIImageAssetInst is a representation on C++ side.\n";

	BIND_INST_TYPE_TO_SCRIPT(UIImageAssetInst, UIImageAsset, brief)
	core.scripts.RegisterObjectMethod(script_class_name, "void SetSpriteFromAsset(string&in)", WRAP_MFN(UIImageAssetInst, SetSpriteFromAsset), "Setting sprite from asset by name");
}

void UIImageAssetInst::SetSpriteFromAsset(string& asset_name)
{
	SpriteAsset* asset = dynamic_cast<SpriteAsset*>(GetScene()->FindInGroup("SpriteAsset", asset_name.c_str()));

	if (asset)
	{
		Sprite::Copy(&asset->sprite, &sprite);
	}
}

#ifdef EDITOR
UIImageAssetInst* UIImageAssetInst::temp = nullptr;

bool UIImageAssetInst::AddedToTreeByParent()
{
	return true;
}

void UIImageAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UIImageAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UIImageAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif
