#pragma once

#include "Support/Support.h"
#include "DataTrack.h"

class Transform2DDataTrackWidgets;

class Transform2DDataTrack : public TemplDataTrack<Transform2D>
{
public:

	Transform2D* value;
	Transform2DDataTrack(TrackPlayer* own, const char* nm, Transform2D* value, ChangeKeyFunc changeKey_Func);
	virtual ~Transform2DDataTrack();

	virtual void Load(JSONReader* stream, int num);
	virtual void Save(JSONWriter* stream);
	virtual void CalcValue(int index, float blend);


#ifdef EDITOR
	int edited_key = -1;
	Transform2DDataTrackWidgets* wigets;

	virtual void SetDefaultKey(int index);
	virtual void InitControls(EUIWidget* parent);
	virtual void StartEditKey(int key);
	virtual void StopEditKey();
#endif
};

#define AddTransform2DDataTrack(name, value, onNextCallback)\
Transform2DDataTrack* name##Transform2DDataTrack = new Transform2DDataTrack(this, #name, value, onNextCallback);\
tracks.push_back(name##Transform2DDataTrack);