
#include "SpritesLayerInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpritesLayerInst, "SpritesLayer")

META_DATA_DESC(SpritesLayerInst)
BASE_SCENE_OBJ_PROP(SpritesLayerInst)
FLOAT_PROP(SpritesLayerInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpritesLayerInst, trans.depth, 0.5f, "Geometry", "Depth")
FLOAT_PROP(SpritesLayerInst, trans.pos.y, 0.0f, "Geometry", "y")
META_DATA_DESC_END()

void SpritesLayerInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpritesLayerInst::Draw);
}

void SpritesLayerInst::Draw(float dt)
{
	if (state == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	SpritesLayerAsset* layer_asset = (SpritesLayerAsset*)asset;

	if (layer_asset->sprites.size() == 0)
	{
		return;
	}

	Transform2D tmp_trans;
	tmp_trans.depth = trans.depth;

	Vector2 min_pos = layer_asset->sprites[0].pos - layer_asset->sprites[0].size * 0.5f;
	Vector2 max_pos = layer_asset->sprites[0].pos + layer_asset->sprites[0].size * 0.5f;

	for (auto& sprite : layer_asset->sprites)
	{
		Vector2 tmp_min_pos = sprite.pos - sprite.size * 0.5f;
		Vector2 tmp_max_pos = sprite.pos + sprite.size * 0.5f;

		min_pos.x = fmin(min_pos.x, tmp_min_pos.x);
		min_pos.y = fmin(min_pos.y, tmp_min_pos.y);

		max_pos.x = fmax(max_pos.x, tmp_max_pos.x);
		max_pos.y = fmax(max_pos.y, tmp_max_pos.y);
	}

	Vector2 size = max_pos - min_pos;

	Vector2 cam_pos = 0.0f;
	int from = 0;
	int to = 0;

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	if (Sprite::use_ed_cam)
	{
		cam_pos = Sprite::ed_cam_pos;
		Sprite::ed_cam_pos *= axis_scale;
		from = (int)((Sprite::ed_cam_pos.x / scale) / size.x);
		to = (int)(((Sprite::ed_cam_pos.x + render.GetDevice()->GetWidth())/ scale) / size.x);
	}
	else
	{
		cam_pos = Sprite::cam_pos;
		Sprite::cam_pos *= axis_scale;
		from = (int)((Sprite::cam_pos.x) / size.x);
		to = (int)(((Sprite::cam_pos.x + render.GetDevice()->GetWidth())) / size.x);
	}

	for (int x = from - 1; x < to + 1; x++)
	{
		for (auto sprite : layer_asset->sprites)
		{
			Sprite::UpdateFrame(&sprite.sprite, &sprite.state, dt);
			tmp_trans.size = sprite.size;
			tmp_trans.pos = sprite.pos;
			tmp_trans.pos.x -= min_pos.x;
			tmp_trans.pos.x += (float)x * size.x;
			tmp_trans.pos.y -= min_pos.y + size.y * 0.5f;
			tmp_trans.pos.y += trans.pos.y * axis_scale;
			tmp_trans.BuildMatrices();
			Sprite::Draw(&tmp_trans, COLOR_WHITE, &sprite.sprite, &sprite.state, true, false);
		}
	}

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}