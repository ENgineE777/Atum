
#pragma once

#include "TrackPlayer.h"

class BoxPlayer : public TrackPlayer
{
public:
	META_DATA_DECL(BoxPlayer)

	Matrix trans;
	Color  color;
	Vector3 size;

	BoxPlayer();
	virtual ~BoxPlayer();

	void Init() override;
	void Draw(float dt);
};
