
#include "FileNameWidget.h"

void FileNameWidget::Listiner::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == owner->openBtn)
	{
		owner->OpenFileDialog();
	}
	else
	if (sender == owner->deleteBtn)
	{
		owner->ClearData();
	}
}

void FileNameWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	openBtn = new EUIButton(panel, "", 90, 5, 75, 20);
	openBtn->SetListener(&listiner, 0);

	deleteBtn = new EUIButton(panel, "X", 165, 5, 20, 20);
	deleteBtn->SetListener(&listiner, 0);

	listiner.owner = this;
}

void FileNameWidget::SetData(void* set_data)
{
	data = (std::string*)set_data;
	openBtn->SetText(data->c_str());
}

void FileNameWidget::OpenFileDialog()
{
	const char* fileName = EUI::OpenOpenDialog("Any file", NULL);

	if (fileName)
	{
		*data = fileName;
		openBtn->SetText(fileName);
	}
}

void FileNameWidget::ClearData()
{
	*data = "";
	openBtn->SetText("");
}