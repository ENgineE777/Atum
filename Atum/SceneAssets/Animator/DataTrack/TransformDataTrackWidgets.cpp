

#ifdef EDITOR

#include "TransformDataTrackWidgets.h"
#include "TransformDataTrack.h"

TransformDataTrackWidgets::TransformDataTrackWidgets(TransformDataTrack* trck)
{
	track = trck;
}

void TransformDataTrackWidgets::InitControls(EUIWidget* parent)
{
	InitBlendControls(parent, DataTrack::AllowedBlendOnlyLinear);

	prop_xlabel = new EUILabel(parent, "x", 10, 40, 90, 22);
	prop_xed = new EUIEditBox(parent, "0.0", 95, 40, 100, 22, EUIEditBox::InputFloat);
	prop_xed->SetListener(-1, this, 0);

	prop_ylabel = new EUILabel(parent, "y", 10, 70, 90, 22);
	prop_yed = new EUIEditBox(parent, "0.0", 95, 70, 100, 22, EUIEditBox::InputFloat);
	prop_yed->SetListener(-1, this, 0);

	prop_zlabel = new EUILabel(parent, "z", 10, 110, 90, 22);
	prop_zed = new EUIEditBox(parent, "0.0", 95, 110, 100, 22, EUIEditBox::InputFloat);
	prop_zed->SetListener(-1, this, 0);

	btn_grab = new EUIButton(parent, "Grab", 95, 140, 100, 22);
	btn_grab->SetListener(-1, this, 0);

	Show(false);
}

void TransformDataTrackWidgets::StartEditKey(TransformKey* val)
{
	value = val;

	char str[32];

	sprintf(str, "%4.3f", value->pos.x);
	prop_xed->SetText(str);

	sprintf(str, "%4.3f", value->pos.y);
	prop_yed->SetText(str);

	sprintf(str, "%4.3f", value->pos.z);
	prop_zed->SetText(str);

	Show(true);
}

void TransformDataTrackWidgets::StopEditKey()
{
	Show(false);
}

void TransformDataTrackWidgets::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (track && track->edited_key != -1)
	{
		track->SetDefaultValue(track->edited_key);
		track->DataToGizmo();
		track->Prepare();
	}
}

void TransformDataTrackWidgets::OnEditBoxStopEditing(EUIWidget* sender)
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
	if (sender == prop_zed)
	{
		val_ptr = &value->pos.z;
	}

	float val = (float)atof(sender->GetText());

	if (fabs(val - *val_ptr)>0.001f)
	{
		*val_ptr = val;

		track->DataToGizmo();
		track->Prepare();
	}
}

void TransformDataTrackWidgets::OnComboBoxChange(EUIWidget* sender, int index)
{
	DataTrackWidgets::OnComboBoxChange(sender, index);

	if (sender == blend_cb)
	{
		track->Prepare();
	}
}

void TransformDataTrackWidgets::Show(bool show)
{
	prop_xlabel->Show(show);
	prop_xed->Show(show);

	prop_ylabel->Show(show);
	prop_yed->Show(show);

	prop_zlabel->Show(show);
	prop_zed->Show(show);

	btn_grab->Show(show);

	ShowBlendControls(show);
}

#endif