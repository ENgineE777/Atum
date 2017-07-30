
#pragma once

#include "ProperyWidget.h"

class FileNameWidget : public ProperyWidget
{
public:
	EUIButton* openBtn;
	EUIButton* deleteBtn;
	std::string* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		FileNameWidget* owner;

		virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	};

	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
	void OpenFileDialog();
	void ClearData();
};