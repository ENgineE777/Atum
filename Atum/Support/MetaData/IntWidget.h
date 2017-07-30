
#pragma once

#include "ProperyWidget.h"

class IntWidget : public ProperyWidget
{
public:
	EUIEditBox* ebox;
	int* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		IntWidget* owner;

		virtual void OnEditBoxEnterPressed(EUIWidget* sender);
		virtual void OnEditBoxChange(EUIWidget* sender);
	};

	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
	void SetEditedData();
};