
#pragma once

#include "ProperyWidget.h"

class FileNameWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIButton* openBtn;
	EUIButton* deleteBtn;
	std::string* data;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* owner, void* set_data) override;
	void OpenFileDialog();
	void ClearData();
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};