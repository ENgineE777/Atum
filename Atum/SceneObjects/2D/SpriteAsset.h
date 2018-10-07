
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

class SpriteAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteAsset)

	Transform2D trans;

	Sprite::Data sprite;
	static Sprite::FrameState state;

	SpriteAsset();
	virtual ~SpriteAsset() = default;

	void Init() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};
