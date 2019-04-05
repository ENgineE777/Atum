
#include "BoolWidget.h"

void BoolWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	cbox = new EUICheckBox(panel, "", 90, 5, 95, 20);
	cbox->SetListener(-1, this, 0);
}

void BoolWidget::SetData(void* owner, void* set_data)
{
	data = (bool*)set_data;
	cbox->SetChecked(*data);
	changed = true;
}

void BoolWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	*data = !(*data);
}
