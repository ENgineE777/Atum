
#pragma once

#include "ProperyWidget.h"

class ArrayWidget : public ProperyWidget, public EUIWidget::Listener
{
public:
	EUIButton* addBtn;
	EUIButton* nextBtn;
	EUIButton* prevBtn;
	EUILabel* elem_index;
	EUICategories* elements;

	int32_t pre_sel_item = -1;

	struct ElemCat
	{
		EUIButton* btnDel = nullptr;
		EUICategories* cat = nullptr;
	};
	std::vector<ElemCat> elem_cats;

	uint8_t* value;
	void* prop;
	void* root;
	int32_t* sel_item = nullptr;
	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void Show(bool show) override;
	void SetData(void* owner, void* set_data) override;
	void OnUpdate(EUIWidget* sender) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};