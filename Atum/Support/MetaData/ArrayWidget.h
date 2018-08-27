
#pragma once

#include "ProperyWidget.h"

class ArrayWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIButton* addBtn;
	EUICategories* elements;

	struct ElemCat
	{
		EUIButton* btnDel = nullptr;
		EUICategories* cat = nullptr;
	};
	std::vector<ElemCat> elem_cats;
	void* prop;
	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void Show(bool show) override;
	void SetData(void* set_data) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};