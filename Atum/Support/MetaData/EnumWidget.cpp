
#include "EnumWidget.h"
#include "MetaData.h"

EnumWidget::EnumWidget(MetaDataEnum* set_enumRef)
{
	enumRef = set_enumRef;
}

void EnumWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	cbox = new EUIComboBox(panel, 90, 5, 95, 20, 100);

	for (int i = 0; i < enumRef->names.size(); i++)
	{
		cbox->AddItem(enumRef->names[i].c_str());
	}

	cbox->SetListener(-1, this, 0);
}

void EnumWidget::SetData(void* set_data)
{
	data = (int*)set_data;

	for (int i = 0; i < enumRef->values.size(); i++)
	{
		if (enumRef->values[i] == *data)
		{
			cbox->SetCurString(i);
			break;
		}
	}
}

void EnumWidget::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	*data = enumRef->values[index];
	changed = true;
}