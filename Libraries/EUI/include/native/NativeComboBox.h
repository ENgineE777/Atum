
#pragma once

#include "NativeInclude.h"

class NativeComboBox : public WidgetImpl
{
public:

	NativeComboBox(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void ClearList() = 0;
	virtual void AddItem(const char* str) = 0;
	virtual void SetCurString(int index) = 0;
	virtual void SetCurString(const char* str) = 0;
	virtual const char* GetCurString() = 0;
	virtual int  GetCurStringIndex() = 0;
};
