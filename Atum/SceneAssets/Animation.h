
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "../SceneObjects/quad.h"

class Animation : public SceneAsset
{
public:
	META_DATA_DECL(Animation)

	float length;

	Texture* tex;
	Quad quad;

	CLASSDECLDIF(SceneAsset, Animation)
	Animation();
	virtual ~Animation();

	virtual void Init();
	void Draw(float dt);

	void Play();
	void Stop();
};
