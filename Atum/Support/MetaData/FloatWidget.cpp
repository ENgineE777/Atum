
#include "FloatWidget.h"

void FloatWidget::Listiner::OnEditBoxStopEditing(EUIWidget* sender)
{
	owner->SetEditedData();
}

void FloatWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputFloat);
	ebox->SetListener(-1, &listiner, 0);
	listiner.owner = this;
}

void FloatWidget::SetData(void* set_data)
{
	data = (float*)set_data;
	char text[128];
	sprintf(text, "%4.5f", *data);
	ebox->SetText(text);
}

void FloatWidget::SetEditedData()
{
	*data = (float)atof(ebox->GetText());
	changed = true;
}