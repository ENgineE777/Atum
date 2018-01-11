#pragma once

#ifdef EDITOR

#include "DataTrackWidgets.h"

class FloatDataTrackWidgets : public DataTrackWidgets
{
public:

	float* value = nullptr;

	EUILabel*   prop_label = nullptr;
	EUIEditBox* prop_ed = nullptr;

	void InitControls(EUIWidget* parent);
	void StartEditKey(float* vl, const char* name);
	void StopEditKey();

	virtual void OnEditBoxChange(EUIWidget* sender);
};

#endif