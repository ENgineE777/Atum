#include "UIImageAsset.h"
#include "SceneAssets/SpriteWindow.h"
#include "SceneObjects/RenderLevels.h"
#include "SceneAssets/SpriteAsset.h"
#include "Services/Core/Core.h"

#ifdef EDITOR
void StartEditUIImageAsset(void* owner)
{
	UIImageAsset* sprite = (UIImageAsset*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

CLASSREG(UIWidgetAsset, UIImageAsset, "Image")

META_DATA_DESC(UIImageAsset)
BASE_SCENE_OBJ_NAME_PROP(UIImageAsset)
BASE_SCENE_OBJ_STATE_PROP(UIImageAsset)
FLOAT_PROP(UIImageAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(UIImageAsset, trans.pos.y, 0.0f, "Prop", "y")
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
FLOAT_PROP(UIImageAsset, trans.offset.x, 0.0f, "Prop", "anchorn_x")
FLOAT_PROP(UIImageAsset, trans.offset.y, 0.0f, "Prop", "anchorn_y")
FLOAT_PROP(UIImageAsset, left_padding.x, 0.0f, "Prop", "left_padding")
FLOAT_PROP(UIImageAsset, left_padding.y, 0.0f, "Prop", "top_padding")
FLOAT_PROP(UIImageAsset, right_padding.x, 0.0f, "Prop", "right_padding")
FLOAT_PROP(UIImageAsset, right_padding.y, 0.0f, "Prop", "bottom_padding")
FLOAT_PROP(UIImageAsset, rotate, 0.0f, "Prop", "rotate")
COLOR_PROP(UIImageAsset, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAsset, color.a, 1.0f, "Prop", "alpha")
BOOL_PROP(UIImageAsset, scaleChilds, false, "Prop", "scale_childs")
BOOL_PROP(UIImageAsset, clipChilds, false, "Prop", "clip_childs")
#ifdef EDITOR
CALLBACK_PROP(UIImageAsset, StartEditUIImageAsset, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

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

	if (state == Invisible)
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
			parent_size.x = render.GetDevice()->GetWidth() * 1024.0f / render.GetDevice()->GetHeight();
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
#endif

CLASSREG(UIWidgetAsset, UIImageAssetInst, "UIImage")

#ifdef EDITOR
void StartEditUIImageAssetInst(void* owner)
{
	UIImageAssetInst* sprite = (UIImageAssetInst*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

META_DATA_DESC(UIImageAssetInst)
BASE_SCENE_OBJ_STATE_PROP(UIImageAssetInst)
COLOR_PROP(UIImageAssetInst, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(UIImageAssetInst, color.a, 1.0f, "Prop", "alpha")
#ifdef EDITOR
CALLBACK_PROP(UIImageAssetInst, StartEditUIImageAssetInst, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

void UIImageAssetInst::BindClassToScript()
{
	BIND_INST_TYPE_TO_SCRIPT(UIImageAssetInst, UIImageAsset)
	scripts.engine->RegisterObjectMethod(script_class_name, "void SetSpriteFromAsset(string&in)", WRAP_MFN(UIImageAssetInst, SetSpriteFromAsset), asCALL_GENERIC);
}

void UIImageAssetInst::SetSpriteFromAsset(string& asset_name)
{
	SpriteAsset* asset = (SpriteAsset*)owner->FindByName(asset_name.c_str(), true);
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
