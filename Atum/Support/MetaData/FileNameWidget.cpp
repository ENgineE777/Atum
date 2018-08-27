
#include "FileNameWidget.h"
#include "Support\StringUtils.h"

void FileNameWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	openBtn = new EUIButton(panel, "", 90, 5, 75, 20);
	openBtn->SetListener(-1, this, 0);

	deleteBtn = new EUIButton(panel, "X", 165, 5, 20, 20);
	deleteBtn->SetListener(-1, this, 0);
}

void FileNameWidget::SetData(void* set_data)
{
	data = (std::string*)set_data;
	openBtn->SetText(data->c_str());
}

void FileNameWidget::OpenFileDialog()
{
	const char* file_name = EUI::OpenOpenDialog(openBtn->GetRoot()->GetNative(), "Any file", NULL);

	char cur_dir[2048];
	GetCurrentDirectory(512, cur_dir);

	if (file_name)
	{
		char cropped_path[1024];
		StringUtils::GetCropPath(cur_dir, file_name, cropped_path, 1024);
		StringUtils::RemoveFirstChar(cropped_path);

		*data = cropped_path;
		openBtn->SetText(cropped_path);
		changed = true;
	}
}

void FileNameWidget::ClearData()
{
	*data = "";
	openBtn->SetText("");
	changed = true;
}

void FileNameWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == openBtn)
	{
		OpenFileDialog();
	}
	else
	if (sender == deleteBtn)
	{
		ClearData();
	}
}