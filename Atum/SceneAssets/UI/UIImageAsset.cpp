#include "UIImageAsset.h"
#include "SceneAssets/SpriteWindow.h"
#include "Editor/Gizmo.h"
#include "SceneObjects/RenderLevels.h"

#ifdef EDITOR
void StartEditUIImageAsset(void* owner)
{
	UIImageAsset* sprite = (UIImageAsset*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

CLASSREG(UIWidgetAsset, UIImageAsset)

META_DATA_DESC(UIImageAsset)
FLOAT_PROP(UIImageAsset, trans.pos.x, 100.0f, "Prop", "x")
FLOAT_PROP(UIImageAsset, trans.pos.y, 100.0f, "Prop", "y")
ENUM_PROP(UIImageAsset, horzAlign, 0, "Prop", "horz_align")
	ENUM_ELEM("Left", 0)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Right", 2)
ENUM_END
ENUM_PROP(UIImageAsset, vertAlign, 0, "Prop", "vert_align")
	ENUM_ELEM("Top", 3)
	ENUM_ELEM("Center", 1)
	ENUM_ELEM("Bottom", 4)
ENUM_END
FLOAT_PROP(UIImageAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(UIImageAsset, trans.size.y, 100.0f, "Prop", "height")
ENUM_PROP(UIImageAsset, horzSize, 0, "Prop", "horz_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
ENUM_PROP(UIImageAsset, vertSize, 0, "Prop", "vert_size")
	ENUM_ELEM("Fixed", 0)
	ENUM_ELEM("Fill parent", 1)
	ENUM_ELEM("Wrap content", 2)
ENUM_END
FLOAT_PROP(UIImageAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UIImageAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UIImageAsset, right_padding.x, 100.0f, "Prop", "right_padding")
FLOAT_PROP(UIImageAsset, right_padding.y, 100.0f, "Prop", "bottom_padding")
FLOAT_PROP(UIImageAsset, anchor.x, 0.5f, "Prop", "anchor_x")
FLOAT_PROP(UIImageAsset, anchor.y, 0.5f, "Prop", "anchor_y")
BOOL_PROP(UIImageAsset, abs_anchor, true, "Prop", "anchor_abs")
FLOAT_PROP(UIImageAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UIImageAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UIImageAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIImageAsset, clipChilds, false, "Prop", "clip_childs")
#ifdef EDITOR
CALLBACK_PROP(UIImageAsset, StartEditUIImageAsset, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

Sprite::FrameState UIImageAsset::state;

UIImageAsset::~UIImageAsset()
{

}

void UIImageAsset::Init()
{
}

void UIImageAsset::Load(JSONReader& loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);
	Sprite::Load(loader, &sprite);

	UIWidgetAsset::Load(loader);
}

void UIImageAsset::Save(JSONWriter& saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);
	Sprite::Save(saver, &sprite);

	UIWidgetAsset::Save(saver);
}

void UIImageAsset::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		//GetMetaData()->UpdateWidgets();
	}
#endif

	CalcState();

	Sprite::UpdateFrame(&sprite, &state, dt);

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

	Sprite::Draw(&trans, cur_color, &sprite, &state, false);

	for (auto child : childs)
	{
		child->Draw(dt);
	}
}

#ifdef EDITOR
void UIImageAsset::Copy(SceneObject* src)
{
	Sprite::Copy(&((UIImageAsset*)src)->sprite, &sprite);
	SceneAsset::Copy(src);
}

void UIImageAsset::SetEditMode(bool ed)
{
	UIWidgetAsset::SetEditMode(ed);

	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
		SpriteWindow::StopEdit();
	}
}
#endif

CLASSREG(UIWidgetAsset, UIImageAssetInst)

#ifdef EDITOR
void StartEditUIImageAssetInst(void* owner)
{
	UIImageAssetInst* sprite = (UIImageAssetInst*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

META_DATA_DESC(UIImageAssetInst)
COLOR_PROP(UIImageAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAsset, color.a, 1.0f, "Prop", "alpha")
#ifdef EDITOR
CALLBACK_PROP(UIImageAsset, StartEditUIImageAssetInst, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

#ifdef EDITOR
UIImageAssetInst* UIImageAssetInst::temp = nullptr;

void UIImageAssetInst::SetEditMode(bool ed)
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
		SpriteWindow::StopEdit();
	}

	Gizmo::inst->allow_transform = !ed;
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
