
#include "SpriteAsset.h"
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SpriteWindow.h"

CLASSREG(SceneAsset, SpriteAsset, "Sprite")

META_DATA_DESC(SpriteAsset)
	BASE_SCENE_ASSET_PROP(SpriteAsset)
	FLOAT_PROP(SpriteAsset, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of a camera position")
	FLOAT_PROP(SpriteAsset, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of a camera position")
	FLOAT_PROP(SpriteAsset, trans.size.x, 100.0f, "Prop", "width", "Width of a sprite")
	FLOAT_PROP(SpriteAsset, trans.size.y, 100.0f, "Prop", "height", "Height of a sprite")
	FLOAT_PROP(SpriteAsset, trans.offset.x, 0.5f, "Prop", "anchorn_x", "X coordinate of anchorn in absolute units")
	FLOAT_PROP(SpriteAsset, trans.offset.y, 0.5f, "Prop", "anchorn_y", "Y coordinate of anchorn in absolute units")
	BOOL_PROP(SpriteAsset, use_source_size, false, "Prop", "use_source_size", "Use size of a source")
	FLOAT_PROP(SpriteAsset, source_scale.x, 1.0f, "Prop", "source_scaleX", "Horizontal scale of a source")
	FLOAT_PROP(SpriteAsset, source_scale.y, 1.0f, "Prop", "source_scaleY", "Vertical scale of a source")
	SPRITE_PROP(SpriteAsset, sprite, "Prop", "sprite")
META_DATA_DESC_END()

Sprite::FrameState SpriteAsset::state;

SpriteAsset::SpriteAsset() : SceneAsset()
{
	inst_class_name = "SpriteInst";
}

void SpriteAsset::Init()
{
#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpriteAsset::Draw);
#endif

	GetScene()->AddToGroup(this, "SpriteAsset");
}

void SpriteAsset::ApplyProperties()
{
	if (use_source_size && sprite.texture)
	{
		trans.size = sprite.rects[0].size * source_scale;
	}
}

void SpriteAsset::Draw(float dt)
{
	trans.BuildMatrices();
	Sprite::UpdateFrame(&sprite, &state, dt);

#ifdef EDITOR
	if (edited && use_source_size && sprite.texture)
	{
		trans.size = sprite.rects[0].size * source_scale;
	}

	if (edited && SpriteWindow::instance && !SpriteWindow::instance->show_anim)
	{
		state.cur_frame = SpriteWindow::instance->cur_frame;
	}

	if (SpriteWindow::instance)
	{
		SpriteWindow::instance->CheckStateOfBorder();
	}
#endif

	Sprite::Draw(&trans, COLOR_WHITE, &sprite, &state, true, false);
}

#ifdef EDITOR
void SpriteAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);

	if (ed)
	{
		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(trans));
	}
	else
	{
		SpriteWindow::StopEdit();
	}
}
#endif