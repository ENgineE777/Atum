
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "../SceneObjects/quad.h"

class SpriteAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteAsset)

	float length;

	Texture* tex;
	Quad quad;

	CLASSDECLDIF(SceneAsset, SpriteAsset)
	SpriteAsset();
	virtual ~SpriteAsset();

	virtual void Init();
	void Draw(float dt);

	void Play();
	void Stop();
};
