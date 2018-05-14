
#pragma once

#include "TrackPlayer.h"
#include "Services/Render/Render.h"

class SpritePlayer : public TrackPlayer
{
public:
	META_DATA_DECL(SpritePlayer)

	Transform2D trans;
	Texture* tex;

	CLASSDECLDIF(TrackPlayer, SpritePlayer)
	SpritePlayer();
	virtual ~SpritePlayer();

	virtual void Init();
	void Draw(float dt);
};
