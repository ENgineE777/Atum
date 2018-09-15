
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

class SpritesLayerAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpritesLayerAsset)

	Transform2D trans;

	struct LayerSprite
	{
		META_DATA_DECL(Sprite)

		Vector2 pos;
		Vector2 size;
		Sprite::Data sprite;
		Sprite::FrameState state;
	};

	vector<LayerSprite> sprites;

	SpritesLayerAsset();
	virtual ~SpritesLayerAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	int sel_sprite = -1;
	SceneObject* CreateInstance() override;
	void SetEditMode(bool ed) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void SetGizmo();
#endif
};
