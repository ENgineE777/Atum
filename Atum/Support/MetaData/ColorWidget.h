
#pragma once

#include "ProperyWidget.h"
#include "Support/Color.h"

class ColorWidget : public ProperyWidget
{
public:
	EUILabel* ecolor;
	Color* data;

	class Listiner : public EUIWidget::Listener
	{
	public:
		ColorWidget* owner;

		void OnLeftDoubliClick(EUIWidget* sender, int mx, int my) override;
	};

	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* set_data);
	void ChoseColor();
};