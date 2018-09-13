
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

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
	SceneObject* CreateInstance() override;
	void SetEditMode(bool ed) override;
#endif
};
