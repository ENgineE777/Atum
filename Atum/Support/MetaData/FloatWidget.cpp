
#include "FloatWidget.h"

void FloatWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputFloat);
	ebox->SetListener(-1, this, EUIWidget::OnUpdate);
}

void FloatWidget::SetData(void* owner, void* set_data)
{
	data = (float*)set_data;
	prev_value = *data;
	ebox->SetText(*data);
}

void FloatWidget::OnUpdate(EUIWidget* sender)
{
	if (prev_value != *data)
	{
		prev_value = *data;
		ebox->SetText(prev_value);
	}
}

void FloatWidget::OnEditBoxStopEditing(EUIEditBox* sender)
{
	*data = ebox->GetAsFloat();
	changed = true;
}