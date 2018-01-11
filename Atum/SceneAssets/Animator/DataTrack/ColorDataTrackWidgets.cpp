
#ifdef EDITOR

#include "ColorDataTrackWidgets.h"

void ColorDataTrackWidgets::InitControls(EUIWidget* parent)
{
	InitBlendControls(parent, DataTrack::AllowedBlendOnlyLinear);

	color_label = new EUILabel(parent, "Value", 10, 40, 90, 22);
	color_label->Show(false);

	color_ed = new EUILabel(parent, "", 95, 40, 100, 22);
	color_ed->SetListener(-1, this, 0);
	color_ed->Show(false);

	alpha_label = new EUILabel(parent, "Value", 10, 70, 90, 22);
	alpha_label->Show(false);

	alpha_ed = new EUIEditBox(parent, "0.0", 95, 70, 100, 22, EUIEditBox::InputFloat);
	alpha_ed->SetListener(-1, this, 0);
	alpha_ed->Show(false);
}

void ColorDataTrackWidgets::StartEditKey(Color* set_value)
{
	value = set_value;

	int clr[3];
	clr[0] = (int)(value->r * 255.0f);
	clr[1] = (int)(value->g * 255.0f);
	clr[2] = (int)(value->b * 255.0f);

	color_ed->SetBackgroundColor(true, clr);

	char str[32];
	sprintf(str, "%4.2f", value->a);
	alpha_ed->SetText(str);

	color_label->Show(true);
	color_ed->Show(true);

	alpha_label->Show(true);
	alpha_ed->Show(true);

	ShowBlendControls(true);
}

void ColorDataTrackWidgets::StopEditKey()
{
	color_label->Show(false);
	color_ed->Show(false);

	alpha_label->Show(false);
	alpha_ed->Show(false);

	ShowBlendControls(false);
}

void ColorDataTrackWidgets::OnLeftDoubliClick(EUIWidget* sender, int mx, int my)
{
	if (sender == color_ed)
	{
		if (EUI::OpenColorDialog(color_ed->GetNativeRoot(), &value->r))
		{
			int clr[3];
			clr[0] = (int)(value->r * 255.0f);
			clr[1] = (int)(value->g * 255.0f);
			clr[2] = (int)(value->b * 255.0f);

			color_ed->SetBackgroundColor(true, clr);
		}
	}
}

void ColorDataTrackWidgets::OnEditBoxChange(EUIWidget* sender)
{
	value->a = (float)atof(alpha_ed->GetText());
}

#endif