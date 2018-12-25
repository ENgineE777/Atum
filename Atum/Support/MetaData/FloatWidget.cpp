
#include "FloatWidget.h"

void FloatWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputFloat);
	ebox->SetListener(-1, this, 0);
}

void FloatWidget::SetData(void* set_data)
{
	data = (float*)set_data;
	ebox->SetText(*data);
}

void FloatWidget::OnEditBoxStopEditing(EUIEditBox* sender)
{
	*data = ebox->GetAsFloat();
	changed = true;
}