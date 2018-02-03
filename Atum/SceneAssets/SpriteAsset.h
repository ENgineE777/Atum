
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "../SceneObjects/quad.h"
#include "Sprite.h"

class SpriteAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteAsset)

	Transform2D trans;

	Sprite::Data sprite;
	Quad quad;

	CLASSDECLDIF(SceneAsset, SpriteAsset)
	SpriteAsset();
	virtual ~SpriteAsset();

	virtual void Init();
	virtual void Load(JSONReader* loader);
	virtual void Save(JSONWriter* saver);
	void Draw(float dt);

	virtual void SetEditMode(bool ed);

	void Play();
	void Stop();
};
