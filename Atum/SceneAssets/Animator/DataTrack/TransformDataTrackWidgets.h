#pragma once

#ifdef EDITOR

#include "DataTrackWidgets.h"

struct TransformKey;
class TransformDataTrack;

class TransformDataTrackWidgets : public DataTrackWidgets
{
public:

	TransformKey* value = nullptr;

	TransformDataTrack* track = nullptr;

	EUILabel*   prop_xlabel = nullptr;
	EUIEditBox* prop_xed = nullptr;

	EUILabel*   prop_ylabel = nullptr;
	EUIEditBox* prop_yed = nullptr;

	EUILabel*   prop_zlabel = nullptr;
	EUIEditBox* prop_zed = nullptr;

	EUIButton*  btn_grab = nullptr;

	TransformDataTrackWidgets(TransformDataTrack* track);

	void InitControls(EUIWidget* parent);
	void StartEditKey(TransformKey* value);
	void StopEditKey();

	virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnEditBoxChange(EUIWidget* sender);
	virtual void OnComboBoxChange(EUIWidget* sender, int index);

	void Show(bool show);
};

#endif