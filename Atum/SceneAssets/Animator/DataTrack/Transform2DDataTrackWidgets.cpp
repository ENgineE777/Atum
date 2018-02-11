

#ifdef EDITOR

#include "Transform2DDataTrackWidgets.h"
#include "Transform2DDataTrack.h"

void Transform2DDataTrackWidgets::InitControls(EUIWidget* parent)
{
	InitBlendControls(parent, DataTrack::AllowedBlendAll);

	prop_xlabel = new EUILabel(parent, "x", 10, 40, 90, 22);
	prop_xed = new EUIEditBox(parent, "0.0", 95, 40, 100, 22, EUIEditBox::InputFloat);
	prop_xed->SetListener(-1, this, 0);

	prop_ylabel = new EUILabel(parent, "y", 10, 70, 90, 22);
	prop_yed = new EUIEditBox(parent, "0.0", 95, 70, 100, 22, EUIEditBox::InputFloat);
	prop_yed->SetListener(-1, this, 0);

	prop_sz_xlabel = new EUILabel(parent, "width", 10, 100, 90, 22);
	prop_sz_xed = new EUIEditBox(parent, "0.0", 95, 100, 100, 22, EUIEditBox::InputFloat);
	prop_sz_xed->SetListener(-1, this, 0);

	prop_sz_ylabel = new EUILabel(parent, "height", 10, 130, 90, 22);
	prop_sz_yed = new EUIEditBox(parent, "0.0", 95, 130, 100, 22, EUIEditBox::InputFloat);
	prop_sz_yed->SetListener(-1, this, 0);

	prop_rot_label = new EUILabel(parent, "rotation", 10, 160, 90, 22);
	prop_rot_ed = new EUIEditBox(parent, "0.0", 95, 160, 100, 22, EUIEditBox::InputFloat);
	prop_rot_ed->SetListener(-1, this, 0);

	Show(false);
}

void Transform2DDataTrackWidgets::StartEditKey(Transform2D* val)
{
	value = val;

	char str[32];

	sprintf(str, "%4.3f", value->pos.x);
	prop_xed->SetText(str);

	sprintf(str, "%4.3f", value->pos.y);
	prop_yed->SetText(str);

	sprintf(str, "%4.3f", value->size.x);
	prop_sz_xed->SetText(str);

	sprintf(str, "%4.3f", value->size.y);
	prop_sz_yed->SetText(str);

	sprintf(str, "%4.3f", value->rotation);
	prop_rot_ed->SetText(str);

	Show(true);
}

void Transform2DDataTrackWidgets::StopEditKey()
{
	Show(false);
}

void Transform2DDataTrackWidgets::OnEditBoxStopEditing(EUIWidget* sender)
{
	float* val_ptr = nullptr;

	if (sender == prop_xed)
	{
		val_ptr = &value->pos.x;
	}
	else
	if (sender == prop_yed)
	{
		val_ptr = &value->pos.y;
	}
	else
	if (sender == prop_sz_xed)
	{
		val_ptr = &value->size.x;
	}
	else
	if (sender == prop_sz_yed)
	{
		val_ptr = &value->size.y;
	}
	else
	if (sender == prop_rot_ed)
	{
		val_ptr = &value->rotation;
	}

	float val = (float)atof(sender->GetText());

	if (fabs(val - *val_ptr)>0.001f)
	{
		*val_ptr = val;
	}
}

void Transform2DDataTrackWidgets::Show(bool show)
{
	prop_xlabel->Show(show);
	prop_xed->Show(show);

	prop_ylabel->Show(show);
	prop_yed->Show(show);

	prop_sz_xlabel->Show(show);
	prop_sz_xed->Show(show);

	prop_sz_ylabel->Show(show);
	prop_sz_yed->Show(show);

	prop_rot_label->Show(show);
	prop_rot_ed->Show(show);

	ShowBlendControls(show);
}

#endif