
#pragma once

#include "EUI.h"

class ProperyWidget
{
public:
	EUIPanel* panel;
	EUILabel* label;
	void*     owner = nullptr;
	bool      changed = false;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void SetData(void* owner, void* data) = 0;
	virtual void Show(bool show);
};