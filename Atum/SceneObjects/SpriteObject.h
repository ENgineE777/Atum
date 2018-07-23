
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteAsset.h"

class SpriteObject : public SceneObject
{
public:
	META_DATA_DECL(SpriteObject)

	SpriteAsset* asset = nullptr;
	string asset_name; 
	Transform2D trans;
	Sprite::FrameState frame_state;

	virtual ~SpriteObject() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};
