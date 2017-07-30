
#include "EnumWidget.h"
#include "MetaData.h"

void EnumWidget::Listiner::OnComboBoxChange(EUIWidget* sender, int index)
{
	owner->SetEditedData(index);
}

EnumWidget::EnumWidget(MetaDataEnum* set_enumRef)
{
	enumRef = set_enumRef;
}

void EnumWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	cbox = new EUIComboBox(panel, 90, 5, 95, 100);

	for (int i = 0; i < enumRef->names.size(); i++)
	{
		cbox->AddItem(enumRef->names[i].c_str());
	}

	cbox->SetListener(&listiner, 0);
	listiner.owner = this;
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

void EnumWidget::SetEditedData(int index)
{
	*data = enumRef->values[index];
}