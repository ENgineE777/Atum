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

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;

#ifdef EDITOR
	bool Is3DObject() override;
	void SetEditMode(bool ed) override;
	void Work(float dt);
#endif
	void Reset();
};
