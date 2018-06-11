#pragma once

#ifdef EDITOR

#include "DataTrackWidgets.h"

class Transform2DDataTrackWidgets : public DataTrackWidgets
{
public:

	Transform2D* value = nullptr;

	EUILabel*   prop_xlabel;
	EUIEditBox* prop_xed;

	EUILabel*   prop_ylabel;
	EUIEditBox* prop_yed;

	EUILabel*   prop_sz_xlabel;
	EUIEditBox* prop_sz_xed;

	EUILabel*   prop_sz_ylabel;
	EUIEditBox* prop_sz_yed;

	EUILabel*   prop_rot_label;
	EUIEditBox* prop_rot_ed;

	void InitControls(EUIWidget* parent);
	void StartEditKey(Transform2D* value);
	void StopEditKey();

	void OnEditBoxStopEditing(EUIEditBox* sender) override;

	void Show(bool show);
};

#endif