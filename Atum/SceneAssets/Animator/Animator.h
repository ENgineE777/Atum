#pragma once

#include "TrackPlayers/TrackPlayer.h"
#include "Services/Scene/SceneAsset.h"
#include "AnimatorWindow.h"

class Animator : public SceneAsset
{
#ifdef EDITOR
	AnimatorWindow* wnd;
#endif

public:

	META_DATA_DECL(Animator)

	float edCurTime;
	float fromTime;
	float toTime;
	float timeLenght;

	std::vector<TrackPlayer*> players;

	virtual void Init();
	virtual void Load(JSONReader& loader);
	virtual void Save(JSONWriter& saver);

#ifdef EDITOR
	virtual void SetEditMode(bool ed);
	void Work(float dt);
#endif
	void Reset();
};
