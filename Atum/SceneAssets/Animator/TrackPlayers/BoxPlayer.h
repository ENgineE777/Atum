
#pragma once

#include "TrackPlayer.h"

class BoxPlayer : public TrackPlayer
{
public:
	META_DATA_DECL(BoxPlayer)

	Matrix trans;
	Color  color;
	Vector size;


	CLASSDECLDIF(TrackPlayer, BoxPlayer)
	BoxPlayer();
	virtual ~BoxPlayer();

	virtual void Init();
	void Draw(float dt);
};
