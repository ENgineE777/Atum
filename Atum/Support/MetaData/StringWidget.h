
#pragma once

#include "ProperyWidget.h"

class StringWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIEditBox* ebox;
	std::string* data;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* set_data) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
};