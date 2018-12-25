
#pragma once

#include "ProperyWidget.h"

class EnumStringWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIComboBox * cbox;
	std::string* data;

	typedef void(*Callback)(EUIComboBox* cbox);

	Callback callback;

	EnumStringWidget(Callback callback);
	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* set_data) override;
	void OnComboBoxOpened(EUIComboBox* sender) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
};