
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
	Sprite::FrameState state;
	
	CLASSDECLDIF(SceneObject, SpriteObject)
	SpriteObject();
	virtual ~SpriteObject();

	virtual void Init();
	virtual void ApplyProperties();
	void Draw(float dt);

#ifdef EDITOR
	void SetEditMode(bool ed);
#endif
};
