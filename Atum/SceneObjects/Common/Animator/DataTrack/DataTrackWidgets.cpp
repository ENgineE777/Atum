
#ifdef EDITOR

#include "DataTrackWidgets.h"

void DataTrackWidgets::InitBlendControls(EUIWidget* parent, DataTrack::AllowedBlend blend)
{
	blend_label = new EUILabel(parent, "Blend", 10, 10, 90, 22);
	blend_label->Show(false);

	blend_cb = new EUIComboBox(parent, 95, 10, 100, 20, 100);
	blend_cb->Show(false);
	blend_cb->SetListener(-1, this, 0);

	blend_cb->AddItem("None");
	blend_cb->AddItem("Linear");

	if (blend == DataTrack::AllowedBlendAll)
	{
		blend_cb->AddItem("EasyIn");
		blend_cb->AddItem("EasyOut");
	}
}

void DataTrackWidgets::ShowBlendControls(bool show)
{
	if (show)
	{
		blend_cb->SetCurString(*blend);
	}

	blend_label->Show(show);
	blend_cb->Show(show);
}

void DataTrackWidgets::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	if (sender == blend_cb)
	{
		*blend = (DataTrack::Blend)index;
		DataTrack::timeline->Redraw();
	}
}

#endif