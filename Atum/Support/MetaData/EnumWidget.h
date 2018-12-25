
#pragma once

#include "ProperyWidget.h"

struct MetaDataEnum;

class EnumWidget : public ProperyWidget, public EUIWidget::Listener
{
public:

	EUIComboBox* cbox;
	int* data;

	MetaDataEnum* enumRef;

	EnumWidget(MetaDataEnum* enumRef);
	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* set_data) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
};