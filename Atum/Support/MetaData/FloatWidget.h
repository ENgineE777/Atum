
#pragma once

#include "ProperyWidget.h"

class FloatWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIEditBox* ebox;
	float* data;
	float prev_value;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* owner, void* set_data) override;
	void OnUpdate(EUIWidget* sender) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
};