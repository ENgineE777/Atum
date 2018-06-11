
#pragma once

#include "ProperyWidget.h"

struct MetaDataEnum;

class EnumWidget : public ProperyWidget
{
public:
	EUIComboBox* cbox;
	int* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		EnumWidget* owner;

		void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
	};

	Listiner listiner;

	MetaDataEnum* enumRef;

	EnumWidget(MetaDataEnum* enumRef);
	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
	void SetEditedData(int index);
};