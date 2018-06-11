
#pragma once

#include "ProperyWidget.h"

class BoolWidget : public ProperyWidget
{
public:
	EUICheckBox* cbox;
	bool* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		BoolWidget* owner;

		void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	};

	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
};