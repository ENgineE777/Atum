
#include "ColorWidget.h"

void ColorWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ecolor = new EUILabel(panel, "", 90, 5, 95, 20);
	ecolor->SetListener(-1, this, 0);
}

void ColorWidget::SetData(void* set_data)
{
	data = (Color*)set_data;
	int clr[3];
	clr[0] = (int)(data->r * 255.0f);
	clr[1] = (int)(data->g * 255.0f);
	clr[2] = (int)(data->b * 255.0f);

	ecolor->SetBackgroundColor(true, clr);
}

void ColorWidget::OnLeftDoubliClick(EUIWidget* sender, int mx, int my)
{
	if (EUI::OpenColorDialog(ecolor->GetRoot()->GetNative(), &data->r))
	{
		int clr[3];
		clr[0] = (int)(data->r * 255.0f);
		clr[1] = (int)(data->g * 255.0f);
		clr[2] = (int)(data->b * 255.0f);

		ecolor->SetBackgroundColor(true, clr);

		changed = true;
	}
}