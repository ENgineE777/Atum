
#ifdef EDITOR

#include "FloatDataTrackWidgets.h"

void FloatDataTrackWidgets::InitControls(EUIWidget* parent)
{
	InitBlendControls(parent, DataTrack::AllowedBlendAll);

	prop_label = new EUILabel(parent, "Value", 10, 40, 90, 22);
	prop_label->Show(false);

	prop_ed = new EUIEditBox(parent, "0.0", 95, 40, 100, 22, EUIEditBox::InputFloat);
	prop_ed->SetListener(-1, this, 0);
	prop_ed->Show(false);
}

void FloatDataTrackWidgets::StartEditKey(float* vl, const char* name)
{
	value = vl;
	prop_label->SetText(name);

	char str[32];
	sprintf(str, "%4.2f", *value);
	prop_ed->SetText(str);

	prop_ed->Show(true);
	prop_label->Show(true);

	ShowBlendControls(true);
}

void FloatDataTrackWidgets::StopEditKey()
{
	prop_ed->Show(false);
	prop_label->Show(false);

	ShowBlendControls(false);
}

void FloatDataTrackWidgets::OnEditBoxStopEditing(EUIWidget* sender)
{
	*value = (float)atof(prop_ed->GetText());
}

#endif