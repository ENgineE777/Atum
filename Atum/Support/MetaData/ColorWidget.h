
#pragma once

#include "ProperyWidget.h"
#include "Support/Color.h"

class ColorWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUILabel* ecolor;
	Color* data;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* set_data)  override;
	void OnLeftDoubliClick(EUIWidget* sender, int mx, int my) override;
};