#pragma once

#include "TrackPlayer.h"

class CameraPlayer : public TrackPlayer
{
public:

	float fov = 45.0f;
	Matrix trans;

	CLASSDECLDIF(TrackPlayer, CameraPlayer)

	META_DATA_DECL(CameraPlayer)

	virtual void Init();
	virtual void SetTime(float time);
};