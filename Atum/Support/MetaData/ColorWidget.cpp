
#include "ColorWidget.h"

void ColorWidget::Listiner::OnLeftDoubliClick(EUIWidget* sender, int mx, int my)
{
	owner->ChoseColor();
}

void ColorWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ecolor = new EUILabel(panel, "", 90, 5, 95, 20);
	ecolor->SetListener(&listiner, 0);
	listiner.owner = this;
}

void ColorWidget::SetData(void* set_data)
{
	data = (Color*)set_data;
	int clr[3];
	clr[0] = data->r * 255;
	clr[1] = data->g * 255;
	clr[2] = data->b * 255;

	ecolor->SetBackgroundColor(true, clr);
}

void ColorWidget::ChoseColor()
{
	if (EUI::OpenColorDialog(&data->r))
	{
		int clr[3];
		clr[0] = data->r * 255;
		clr[1] = data->g * 255;
		clr[2] = data->b * 255;

		ecolor->SetBackgroundColor(true, clr);

		changed = true;
	}
}