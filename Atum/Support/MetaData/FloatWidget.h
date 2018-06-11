
#pragma once

#include "ProperyWidget.h"

class FloatWidget : public ProperyWidget
{
public:
	EUIEditBox* ebox;
	float* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		FloatWidget* owner;

		void OnEditBoxStopEditing(EUIEditBox* sender) override;
	};

	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
	void SetEditedData();
};