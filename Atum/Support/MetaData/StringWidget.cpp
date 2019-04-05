
#include "StringWidget.h"

void StringWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	ebox = new EUIEditBox(panel, "0", 90, 5, 95, 20, EUIEditBox::InputText);
	ebox->SetListener(-1, this, 0);
}

void StringWidget::SetData(void* owner, void* set_data)
{
	data = (std::string*)set_data;
	ebox->SetText(data->c_str());
}

void StringWidget::OnEditBoxStopEditing(EUIEditBox* sender)
{
	*data = ebox->GetText();
	changed = true;
}