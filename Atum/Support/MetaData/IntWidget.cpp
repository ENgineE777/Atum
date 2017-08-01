
#include "IntWidget.h"

void IntWidget::Listiner::OnEditBoxEnterPressed(EUIWidget* sender)
{
	owner->SetEditedData();
}

void IntWidget::Listiner::OnEditBoxChange(EUIWidget* sender)
{
	owner->SetEditedData();
}

void IntWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputInteger);
	ebox->SetListener(&listiner, 0);
	listiner.owner = this;
}

void IntWidget::SetData(void* set_data)
{
	data = (int*)set_data;
	char text[128];
	sprintf(text, "%i", *data);
	ebox->SetText(text);
}

void IntWidget::SetEditedData()
{
	*data = atoi(ebox->GetText());
	changed = true;
}