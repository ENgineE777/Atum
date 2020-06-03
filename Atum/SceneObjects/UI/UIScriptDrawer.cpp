#include "UIScriptDrawer.h"
#include "SceneObjects/2D/SpriteWindow.h"
#include "SceneObjects/2D/SpriteAsset.h"
#include "Services/Core/Core.h"

CLASSREG(UIWidgetAsset, UIScriptDrawerAsset, "ScriptDrawer")

META_DATA_DESC(UIScriptDrawerAsset)
	BASE_WIDGET_ASSET_PROP(UIScriptDrawerAsset)
	FLOAT_PROP(UIScriptDrawerAsset, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
	FLOAT_PROP(UIScriptDrawerAsset, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
	ENUM_PROP(UIScriptDrawerAsset, horzAlign, 0, "Prop", "horz_align", "Horizontal aligment of a widget")
		ENUM_ELEM("Left", 0)
		ENUM_ELEM("Center", 1)
		ENUM_ELEM("Right", 2)
	ENUM_END
	ENUM_PROP(UIScriptDrawerAsset, vertAlign, 0, "Prop", "vert_align", "Vertical aligment of a widget")
		ENUM_ELEM("Top", 3)
		ENUM_ELEM("Center", 1)
		ENUM_ELEM("Bottom", 4)
	ENUM_END
	FLOAT_PROP(UIScriptDrawerAsset, trans.size.x, 100.0f, "Prop", "width", "Width of a widget")
	FLOAT_PROP(UIScriptDrawerAsset, trans.size.y, 100.0f, "Prop", "height", "Height of a widget")
	FLOAT_PROP(UIScriptDrawerAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
	FLOAT_PROP(UIScriptDrawerAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
	FLOAT_PROP(UIScriptDrawerAsset, trans.rotation, 0.0f, "Prop", "rotate", "Rotation a widget")
	COLOR_PROP(UIScriptDrawerAsset, color, COLOR_WHITE, "Prop", "color")
	FLOAT_PROP(UIScriptDrawerAsset, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
	SPRITE_PROP(UIScriptDrawerAsset, sprite, "Prop", "image")
META_DATA_DESC_END()

UIScriptDrawerAsset::~UIScriptDrawerAsset()
{

}

void UIScriptDrawerAsset::Init()
{
}

void UIScriptDrawerAsset::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
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

CLASSREG(UIWidgetAsset, UIScriptDrawerAssetInst, "UIScriptDrawer")

#ifdef EDITOR
void StartEditUIScriptDrawerInst(void* owner)
{
	UIScriptDrawerAssetInst* sprite = (UIScriptDrawerAssetInst*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

META_DATA_DESC(UIScriptDrawerAssetInst)
	BASE_WIDGET_INST_PROP(UIScriptDrawerAssetInst)
	COLOR_PROP(UIScriptDrawerAssetInst, color, COLOR_WHITE, "Prop", "color")
	FLOAT_PROP(UIScriptDrawerAssetInst, color.a, 1.0f, "Prop", "alpha", "Transparancy of a widget")
#ifdef EDITOR
	CALLBACK_PROP(UIScriptDrawerAssetInst, StartEditUIScriptDrawerInst, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

void UIScriptDrawerAssetInst::BindClassToScript()
{
	const char* brief = "UI script drawer instance\n"
		"\n"
		"UI script drawed widget.\n"
		"\n"
		"This class ::UIScriptDrawerInst is a representation on C++ side.\n";

	BIND_INST_TYPE_TO_SCRIPT(UIScriptDrawerAssetInst, UIScriptDrawerAsset, brief)
	core.scripts.RegisterObjectMethod(script_class_name, "void SetSpriteFromAsset(string&in)", WRAP_MFN(UIScriptDrawerAssetInst, SetSpriteFromAsset), "Setting sprite from asset by name");
	core.scripts.RegisterObjectMethod(script_class_name, "void Draw(Vector2&in, float, float)", WRAP_MFN(UIScriptDrawerAssetInst, AddDraw), "Add drawer");
}

void UIScriptDrawerAssetInst::SetSpriteFromAsset(string& asset_name)
{
	SpriteAsset* asset = dynamic_cast<SpriteAsset*>(GetScene()->FindInGroup("SpriteAsset", asset_name.c_str()));

	if (asset)
	{
		Sprite::Copy(&asset->sprite, &sprite);
	}
}

void UIScriptDrawerAssetInst::AddDraw(Vector2& pos, float rotation, float scale)
{
	drawers.push_back(Drawer());

	Drawer& drawer = drawers[drawers.size() - 1];
	drawer.pos = pos;
	drawer.rotation = rotation;
	drawer.scale = scale;
}

void UIScriptDrawerAssetInst::Draw(float dt)
{
	if (GetState() == Invisible)
	{
		return;
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

#ifdef EDITOR
	if (!GetScene()->Playing())
	{
		Sprite::Draw(&trans, cur_color, &sprite, &frame_state, false, true);
	}
	else
#endif
	{
		auto init_pos = trans.pos;
		auto init_size = trans.size;

		for (auto& drawer : drawers)
		{
			trans.pos.x = init_pos.x + drawer.pos.x;
			trans.pos.y = init_pos.y + drawer.pos.y;
			trans.size = init_size * drawer.scale;
			trans.rotation = drawer.rotation;

			trans.BuildMatrices();

			Sprite::Draw(&trans, cur_color, &sprite, &frame_state, false, true);
		}

		drawers.clear();

		trans.pos = init_pos;
		trans.size = init_size;
	}

	UIWidgetAsset::Draw(dt);
}

#ifdef EDITOR
UIScriptDrawerAssetInst* UIScriptDrawerAssetInst::temp = nullptr;

bool UIScriptDrawerAssetInst::AddedToTreeByParent()
{
	return true;
}

void UIScriptDrawerAssetInst::StoreProperties()
{
	if (!temp)
	{
		temp = new UIScriptDrawerAssetInst();
	}

	GetMetaData()->Prepare(temp);
	GetMetaData()->Copy(this);
}

void UIScriptDrawerAssetInst::RestoreProperties()
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Copy(temp);
}
#endif
