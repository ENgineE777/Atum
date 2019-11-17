
#pragma once

#include "Services/Scene/SceneObjectComp.h"

/**
\page scene_object_components_track2d Track 2D

This component is adding 2D track to each instance of SpriteInst.

To add track to sprite instance firstly desired instance needed to be selected. After that
component should be switched into editor mode (select Track2D component and press button ED).

And now pressing hot key 'O' will create new point track in center of screen. Pressing hot key 'P'
will create another track point near already selected track point. Holding Left Ctrl and left
mouse click will select track point. Pressing hot key 'i' will delete selected point.

Each track point has own parameters which adjustable when track point is selected.

![Caption text](../images/Track2DComp.jpg "Image")

Track parameters
--------------------------

Name              | Description
------------------| -------------
data_type         | Type of track
speed             | Speed of moving along track
track             | Array of track points

Parameters
----------

Name              | Description
------------------| -------------
flip_mode         | Type of flip when instance is moving from right to left
tracks            | Array of tracks. One track for eache sprite instance

This class ::Track2DComp is a representation on C++ side.

This class Script::Track2D is a representation in a script.

*/

/**
\ingroup gr_code_scene_assets_components
*/

/**
\brief Representation of 2D track component

This class is adding 2D track to each instance of SpriteInst. More to detail how to edit
tracks can be found at \ref scene_object_components_track2d "link".

This class Script::Track2D is a representation in a script.

*/

class Track2DComp : public SceneObjectComp
{
public:

#ifndef DOXYGEN_SKIP

	META_DATA_DECL(Track2DComp)
	
	struct Point
	{
		META_DATA_DECL_BASE(Point)
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
#ifdef EDITOR
		int* sel_point = nullptr;
		Vector2 trans_size;
#endif
		TrackType tp = OneWay;
		float dir = 1.0f;
		int cur_point = 0;
		float cur_dist = 0.0f;
		float point_dist = 0.0f;
		float speed = 40.0f;
		META_DATA_DECL_BASE(Track)
		bool active = true;
		vector<Point> points;

		void Activate(bool active);
		void Reset(bool from_start);
		void SetGizmo();
	};

	int sel_point = -1;
	int sel_track = -1;
	int prop_index = -1;

	FlipMode flip_mode;
	vector<Track> tracks;

	void BindClassToScript() override;
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;

	void SyncInstances();

	void Play() override;
	void UpdateInstances(float dt);
	void UpdateTrack(int index, float dt);

#ifdef EDITOR
	Transform2D trans;
	void Init() override;
	void ApplyProperties() override;
	void EditorDraw(float dt);
	void ResizeInst(int count);
	void SyncPosTrackWithInstPos(int index);
	void InstAdded(int create_from) override;
	void InstDeleted(int index) override;
	void CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif

#endif
};