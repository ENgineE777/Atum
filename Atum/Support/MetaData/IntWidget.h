
#pragma once

#include "ProperyWidget.h"

class IntWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIEditBox* ebox;
	int* data;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* owner, void* set_data) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
};