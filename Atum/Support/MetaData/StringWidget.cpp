
#include "StringWidget.h"

void StringWidget::Listiner::OnEditBoxEnterPressed(EUIWidget* sender)
{
	owner->SetEditedData();
}

void StringWidget::Listiner::OnEditBoxChange(EUIWidget* sender)
{
	owner->SetEditedData();
}

void StringWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputText);
	ebox->SetListener(&listiner, 0);
	listiner.owner = this;
}

void StringWidget::SetData(void* set_data)
{
	data = (std::string*)set_data;
	ebox->SetText(data->c_str());
}

void StringWidget::SetEditedData()
{
	*data = ebox->GetText();
}