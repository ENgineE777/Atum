
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

/**
\page scene_assets_2D_SpriteTileAsset Asset

Sprite Tile

*/


/**
\ingroup gr_code_scene_assets_2D
*/

class SpriteTileAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteTileAsset)

	Transform2D trans;

	Sprite::Data sprite[9];
	static Sprite::FrameState state[9];

	SpriteTileAsset();
	virtual ~SpriteTileAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};
