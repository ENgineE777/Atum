
#include "SpriteTileAsset.h"
#include "SpriteTileInst.h"
#include "Services/Render/Render.h"
#include "SpriteWindow.h"

CLASSREG(SceneAsset, SpriteTileAsset, "SpriteNineTile")

META_DATA_DESC(SpriteTileAsset)
BASE_SCENE_ASSET_PROP(SpriteTileAsset)
FLOAT_PROP(SpriteTileAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteTileAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteTileAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteTileAsset, trans.size.y, 100.0f, "Prop", "height")
FLOAT_PROP(SpriteTileAsset, trans.offset.x, 0.5f, "Prop", "anchorn_x")
FLOAT_PROP(SpriteTileAsset, trans.offset.y, 0.5f, "Prop", "anchorn_y")
SPRITE_PROP(SpriteTileAsset, sprite[0], "Prop", "spriteWN")
SPRITE_PROP(SpriteTileAsset, sprite[1], "Prop", "spriteN")
SPRITE_PROP(SpriteTileAsset, sprite[2], "Prop", "spriteEN")
SPRITE_PROP(SpriteTileAsset, sprite[3], "Prop", "spriteW")
SPRITE_PROP(SpriteTileAsset, sprite[4], "Prop", "spriteC")
SPRITE_PROP(SpriteTileAsset, sprite[5], "Prop", "spriteE")
SPRITE_PROP(SpriteTileAsset, sprite[6], "Prop", "spriteWS")
SPRITE_PROP(SpriteTileAsset, sprite[7], "Prop", "spriteS")
SPRITE_PROP(SpriteTileAsset, sprite[8], "Prop", "spriteES")
META_DATA_DESC_END()

Sprite::FrameState SpriteTileAsset::state[9];

SpriteTileAsset::SpriteTileAsset() : SceneAsset()
{
	inst_class_name = "SpriteTileInst";
}

void SpriteTileAsset::Init()
{
#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpriteTileAsset::Draw);
#endif
}

void SpriteTileAsset::Draw(float dt)
{
	trans.BuildMatrices();

	Vector2 pos = trans.pos - trans.size - 20.0f;

	Transform2D tmp_trans = trans;

	for (int i = 0; i < 3; i++)
	{
		pos.x = trans.pos.x - trans.size.x - 20.0f;

		for (int j = 0; j < 3; j++)
		{
			int index = i * 3 + j;
			Sprite::UpdateFrame(&sprite[index], &state[index], dt);

			tmp_trans.pos = pos;
			tmp_trans.BuildMatrices();
			Sprite::Draw(&tmp_trans, COLOR_WHITE, &sprite[index], &state[index], true, false);

			pos.x += 20.0f + trans.size.x;
		}

		pos.y += 20.0f + trans.size.y;
	}
}

#ifdef EDITOR
void SpriteTileAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);

	if (ed)
	{
		Gizmo::inst->SetTrans2D(&trans);
	}
	else
	{
		SpriteWindow::StopEdit();
	}
}
#endif