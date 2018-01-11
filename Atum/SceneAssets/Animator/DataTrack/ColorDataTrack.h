#pragma once

#include "DataTrack.h"

class ColorDataTrackWidgets;

class ColorDataTrack : public TemplDataTrack<Color>
{
public:
	Color* value = nullptr;
	ColorDataTrack(TrackPlayer* own, const char* nm, Color* value, ChangeKeyFunc changeKey_Func);
	virtual ~ColorDataTrack();

	virtual void Load(JSONReader* stream, int num);
	virtual void Save(JSONWriter* stream);
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR
	ColorDataTrackWidgets* wigets;
	
	virtual void SetDefaultKey(int index);
	virtual void InitControls(EUIWidget* parent);
	virtual void StartEditKey(int key);
	virtual void StopEditKey();
#endif
};

#define AddColorDataTrack(name, value, onNextCallback)\
ColorDataTrack* name##ColorDataTrack = new ColorDataTrack(this, #name, value, onNextCallback);\
tracks.push_back(name##ColorDataTrack);


