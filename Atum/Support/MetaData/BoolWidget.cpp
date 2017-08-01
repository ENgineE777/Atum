
#include "BoolWidget.h"

void BoolWidget::Listiner::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	*owner->data = !(*owner->data);
}

void BoolWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	cbox = new EUICheckBox(panel, "", 90, 5, 95, 20);
	cbox->SetListener(&listiner, 0);
	listiner.owner = this;
}

void BoolWidget::SetData(void* set_data)
{
	data = (bool*)set_data;
	cbox->SetChecked(*data);
	changed = true;
}