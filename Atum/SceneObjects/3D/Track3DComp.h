
#pragma once

#include "Services/Scene/SceneObjectComp.h"

/**
\page scene_object_components_track3d Track 3D

This component is adding 3D track to each instance of MeshInst.

To add track to mesh instance firstly desired instance needed to be selected. After that
component should be switched into editor mode (select Track3D component and press button ED).

And now pressing hot key 'O' will create new point track in center of screen. Pressing hot key 'P'
will create another track point near already selected track point. Holding Left Ctrl and left
mouse click will select track point. Pressing hot key 'i' will delete selected point.

Each track point has own parameters which adjustable when track point is selected.

![Caption text](../images/Track3DComp.jpg "Image")

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
tracks            | Array of tracks. One track for eache Mesh instance

This class ::Track3DComp is a representation on C++ side.

This class Script::Track3D is a representation in a script.

*/

/**
\ingroup gr_code_scene_assets_components
*/

/**
\brief Representation of 3D track component

This class is adding 3D track to each instance of MeshInst. More to detail how to edit
tracks can be found at \ref scene_object_components_track3d "link".

This class Script::Track3D is a representation in a script.

*/

class Track3DComp : public SceneObjectComp
{
public:

#ifndef DOXYGEN_SKIP

	META_DATA_DECL(Track3DComp)
	
	struct Point
	{
		META_DATA_DECL_BASE(Point)
		Matrix trans;
		float wait_time = 0.0f;
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
#endif
		TrackType tp = OneWay;
		float dir = 1.0f;
		int cur_point = 0;
		float cur_dist = 0.0f;
		float point_dist = 0.0f;
		float speed = 20.0f;
		float rotation_speed = 1.0f;
		float wait_time = -1.0f;
		float lerp_time = 0.0f;
		Vector3 prev_dir = 0.0f;

		Vector3 cur_dir = 0.0f;
		Vector3 cur_pos = 0.0f;

		META_DATA_DECL_BASE(Track)
		bool active = true;
		vector<Point> points;

		void Activate(bool active);
		void Reset(bool from_start);
		void ResetScript();
		void SetGizmo();
		void Update(float dt);
		void PredictPosition(float dt, Vector3& pos);
	};

	int sel_point = -1;
	int sel_track = -1;
	int prop_index = -1;

	vector<Track> tracks;

	void BindClassToScript() override;
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;

	void SyncInstances();

	void Play() override;
	void UpdateInstances(float dt);
	void UpdateTrack(int index, float dt);

#ifdef EDITOR
	Matrix trans;
	void Init() override;
	void ApplyProperties() override;
	void EditorDraw(float dt);
	void ResizeInst(int count);
	void SyncPosTrackWithInstPos(int index);
	void InstAdded(int create_from) override;
	void InstDeleted(int index) override;
	void CheckSelection(Vector2 ms, Vector3 start, Vector3 dir) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif

#endif
};