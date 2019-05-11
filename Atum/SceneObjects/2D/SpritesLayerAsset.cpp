
#include "SpritesLayerAsset.h"
#include "Services/Render/Render.h"
#include "SpritesLayerInst.h"
#include "SpriteWindow.h"

CLASSREG(SceneAsset, SpritesLayerAsset, "SpritesLayer")

META_DATA_DESC(SpritesLayerAsset::LayerSprite)
FLOAT_PROP(SpritesLayerAsset::LayerSprite, pos.x, 0.0f, "Prop", "x", "X coordinate of a camera position")
FLOAT_PROP(SpritesLayerAsset::LayerSprite, pos.y, 0.0f, "Prop", "y", "Y coordinate of a camera position")
FLOAT_PROP(SpritesLayerAsset::LayerSprite, size.x, 100.0f, "Prop", "width", "Width of a sprite")
FLOAT_PROP(SpritesLayerAsset::LayerSprite, size.y, 100.0f, "Prop", "height", "Height of a sprite")
SPRITE_PROP(SpritesLayerAsset::LayerSprite, sprite, "Prop", "sprite")
META_DATA_DESC_END()

META_DATA_DESC(SpritesLayerAsset)
BASE_SCENE_ASSET_PROP(SpritesLayerAsset)
ARRAY_PROP_INST(SpritesLayerAsset, sprites, LayerSprite, "Prop", "sprites", SpritesLayerAsset, sel_sprite)
META_DATA_DESC_END()

SpritesLayerAsset::SpritesLayerAsset() : SceneAsset()
{
	inst_class_name = "SpritesLayerInst";
}

void SpritesLayerAsset::Init()
{
#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpritesLayerAsset::Draw);
#endif
}

void SpritesLayerAsset::Draw(float dt)
{
	Transform2D tmp_trans;

	for (auto sprite : sprites)
	{
		Sprite::UpdateFrame(&sprite.sprite, &sprite.state, dt);
		tmp_trans.size = sprite.size;
		tmp_trans.pos = sprite.pos;
		tmp_trans.BuildMatrices();
		Sprite::Draw(&tmp_trans, COLOR_WHITE, &sprite.sprite, &sprite.state, true, false);
	}
}

#ifdef EDITOR
void SpritesLayerAsset::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		SetGizmo();
	}
}

void SpritesLayerAsset::OnLeftMouseDown(Vector2 ms)
{
	if (!core.controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
	{
		return;
	}

	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	sel_sprite = -1;
	for (int i = 0; i < sprites.size(); i++)
	{
		LayerSprite& sprite = sprites[i];

		Vector2 pos = (sprite.pos - 0.5f * sprite.size) * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

		if (pos.x < ms.x && ms.x < pos.x + sprite.size.x * scale &&
			pos.y < ms.y && ms.y < pos.y + sprite.size.y * scale)
		{
			sel_sprite = i;

			break;
		}
	}

	SetGizmo();
}

void SpritesLayerAsset::SetGizmo()
{
	if (sel_sprite != -1)
	{
		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(&sprites[sel_sprite].pos, &sprites[sel_sprite].size), Gizmo::trans_2d_move | Gizmo::trans_2d_scale);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}
#endif