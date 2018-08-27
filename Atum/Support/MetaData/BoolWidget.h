
#pragma once

#include "ProperyWidget.h"

class BoolWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUICheckBox* cbox;
	bool* data;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* set_data) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};