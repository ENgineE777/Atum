
#include "IntWidget.h"

void IntWidget::Listiner::OnEditBoxStopEditing(EUIEditBox* sender)
{
	owner->SetEditedData();
}

void IntWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputInteger);
	ebox->SetListener(-1, &listiner, 0);
	listiner.owner = this;
}

void IntWidget::SetData(void* set_data)
{
	data = (int*)set_data;
	ebox->SetText(*data);
}

void IntWidget::SetEditedData()
{
	*data = ebox->GetAsInt();
	changed = true;
}