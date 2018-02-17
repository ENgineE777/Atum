
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "../SceneObjects/quad.h"
#include "Sprite.h"

class SpriteGraphAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteGraphAsset)

	Transform2D trans;

	Sprite::Data sprite;
	Quad quad;

	CLASSDECLDIF(SceneAsset, SpriteGraphAsset)
		SpriteGraphAsset();
	virtual ~SpriteGraphAsset();

	virtual void Init();
	virtual void Load(JSONReader* loader);
	virtual void Save(JSONWriter* saver);
	void Draw(float dt);

#ifdef EDITOR
	virtual void SetEditMode(bool ed);
#endif
};
