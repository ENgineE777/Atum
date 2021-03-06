#pragma once

#include "DataTrack.h"

class FloatDataTrackWidgets;

class FloatDataTrack : public TemplDataTrack<float>
{
public:

	FloatDataTrack(TrackPlayer* own, const char* nm, float* value, ChangeKeyFunc changeKey_Func);
	virtual ~FloatDataTrack();

	virtual void Load(JSONReader& stream, int num);
	virtual void Save(JSONWriter& stream);
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR
	FloatDataTrackWidgets* wigets;

	virtual void InitControls(EUIWidget* parent);
	virtual void StartEditKey(int key);
	virtual void StopEditKey();
#endif

};

#define AddFloatDataTrack(name, value, onNextCallback, defValue)\
FloatDataTrack* name##FloatDataTrack = new FloatDataTrack(this, #name, value, onNextCallback);\
name##FloatDataTrack->def_value = defValue;\
tracks.push_back(name##FloatDataTrack);