#pragma once

#ifdef EDITOR

#include <EUI.h>
#include "DataTrack.h"

class DataTrackWidgets : public EUIWidget::Listener
{
public:

	DataTrack::Blend* blend = nullptr;

	EUILabel*    blend_label = nullptr;
	EUIComboBox* blend_cb = nullptr;

	void InitBlendControls(EUIWidget* parent, DataTrack::AllowedBlend blend);
	void ShowBlendControls(bool show);

	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
};

#endif