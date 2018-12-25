
#include "IntWidget.h"

void IntWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputInteger);
	ebox->SetListener(-1, this, 0);
}

void IntWidget::SetData(void* set_data)
{
	data = (int*)set_data;
	ebox->SetText(*data);
}

void IntWidget::OnEditBoxStopEditing(EUIEditBox* sender)
{
	*data = ebox->GetAsInt();
	changed = true;
}