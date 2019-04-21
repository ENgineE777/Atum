
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

class SpritesLayerAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpritesLayerAsset)

	struct LayerSprite
	{
		META_DATA_DECL(Sprite)

		Vector2 pos;
		Vector2 size;
		Sprite::Data sprite;
		Sprite::FrameState state;
	};

	int sel_sprite = -1;
	vector<LayerSprite> sprites;

	SpritesLayerAsset();
	virtual ~SpritesLayerAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void SetGizmo();
#endif
};
