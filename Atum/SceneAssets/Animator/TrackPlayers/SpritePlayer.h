
#pragma once

#include "TrackPlayer.h"
#include "../../../SceneObjects/quad.h"

class SpritePlayer : public TrackPlayer
{
public:
	META_DATA_DECL(SpritePlayer)

	Transform2D trans;

	Texture* tex;
	Quad quad;

	CLASSDECLDIF(TrackPlayer, SpritePlayer)
	SpritePlayer();
	virtual ~SpritePlayer();

	virtual void Init();
	void Draw(float dt);
};
