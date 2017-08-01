
#pragma once

#include "EUI.h"

class ProperyWidget
{
public:
	EUIPanel* panel;
	EUILabel* label;
	bool      changed = false;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* data) = 0;
	void Show(bool show);
};