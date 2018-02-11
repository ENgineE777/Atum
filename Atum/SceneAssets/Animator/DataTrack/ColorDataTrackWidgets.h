#pragma once

#ifdef EDITOR

#include "DataTrackWidgets.h"

class ColorDataTrackWidgets : public DataTrackWidgets
{
public:

	Color* value;

	EUILabel* color_label = nullptr;
	EUILabel* color_ed = nullptr;
	EUILabel* alpha_label = nullptr;
	EUIEditBox* alpha_ed = nullptr;

	void InitControls(EUIWidget* parent);
	void StartEditKey(Color* value);
	void StopEditKey();

	void OnLeftDoubliClick(EUIWidget* sender, int mx, int my);
	void OnEditBoxStopEditing(EUIWidget* sender);
};

#endif