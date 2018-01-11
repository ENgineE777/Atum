#pragma once

#include "Support/Support.h"
#include "DataTrack.h"

class TransformDataTrackWidgets;

struct TransformKey
{
	Quaternion rot;
	Vector dir;
	Vector a, b, c, d;
	Vector dirAxeX;
	Vector aX, bX, cX, dX;
	Quaternion aQ, bQ, cQ, dQ;

	Vector pos;
	Vector b_pt1;
	Vector b_pt2;
	
	float length;
	float dst[20];

	float prev_speed;
	float speed;
};

class TransformDataTrack : public TemplDataTrack<TransformKey>
{
	int     curve_count;
	Vector* curve;
	float   radius;

public:

	bool  orient_by_spline;
	Matrix* value;

	TransformDataTrack(TrackPlayer* own, const char* nm, Matrix* value, float radius, bool orinet, ChangeKeyFunc changeKey_Func);
	virtual ~TransformDataTrack();

	virtual void Load(JSONReader* stream, int num);
	virtual void Save(JSONWriter* stream);

	void BezierFunc(Vector& p1, Vector& p2,Vector& p3,Vector& p4, Vector& pos, float t);
	void CubicFunc(int index, Vector& pos, float s);
	void CubicFuncAxeX(int index, Vector& pos, float s);
	void CubicFuncQuat(int index, Quaternion& quat, float s);
	void GenerateKoef(int start_index, int end_index);
	void GenerateKoefAxeX(int start_index, int end_index);
	void GenerateKoefQuat(int start_index, int end_index);
	void Prepare();
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR
	int edited_key = -1;
	TransformDataTrackWidgets* wigets;

	virtual void SetDefaultKey(int index);
	virtual void DelKey(int index);
	virtual int  AddKey(float time);

	virtual void InitControls(EUIWidget* parent);
	virtual void StartEditKey(int key);
	virtual void DataToGizmo();
	virtual void DataFromGizmo();
	virtual void StopEditKey();
	void DrawKey(Matrix& view_proj, Color& color, TransformKey& key);
	virtual void DebugDraw();
	void GrabCamera(int index);
#endif

};

#define AddTransformDataTrack(name, value, r, orient, onNextCallback)\
TransformDataTrack* name##TransformDataTrack = new TransformDataTrack(this, #name, value, r, orient, onNextCallback);\
tracks.push_back(name##TransformDataTrack);