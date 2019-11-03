
#include "EnumStringWidget.h"

EnumStringWidget::EnumStringWidget(Callback set_callback)
{
	callback = set_callback;
}

void EnumStringWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	cbox = new EUIComboBox(panel, 90, 5, 95, 20,200);
	cbox->SetListener(-1, this, 0);
}

void EnumStringWidget::SetData(void* set_owner, void* set_data)
{
	owner = set_owner;
	data = (std::string*)set_data;
	cbox->SetText(data->c_str());
}

void EnumStringWidget::OnComboBoxOpened(EUIComboBox* sender)
{
	cbox->ClearList();

	if (callback)
	{
		callback(cbox, owner);
		cbox->SetText(data->c_str());
	}
}

void EnumStringWidget::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	*data = cbox->GetText();
	changed = true;
}