
#pragma once

#include "Services/Scene/SceneObjectComp.h"

class Track2DComp : public SceneObjectComp
{
public:
	
	META_DATA_DECL(Track2DComp)
	
	struct Point
	{
		META_DATA_DECL(Point)
		Vector2 pos;
	};

	enum FlipMode
	{
		None = 0,
		Normal,
		Reversed
	};

	enum TrackType
	{
		OneWay = 0,
		ForwardBack,
		Looped
	};

	struct Track
	{
		TrackType tp = OneWay;
		float dir = 1.0f;
		int cur_point = 0;
		float cur_dist = 0.0f;
		float point_dist = 0.0f;
		float speed = 40.0f;
		META_DATA_DECL(Track)
		bool active = true;
		vector<Point> points;
	};

	FlipMode flip_mode;
	vector<Track> tracks;

	void Play() override;
	void UpdateInstances(float dt);
	void UpdateTrack(int index, float dt);

#ifdef EDITOR
	int sel_point = -1;
	int sel_track = -1;
	Transform2D trans;
	void Init() override;
	void ApplyProperties() override;
	void EditorDraw(float dt);
	void ResizeInst(int count);
	void InstAdded() override;
	void InstDeleted(int index) override;
	void CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif
};