
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeComboBox : public WinWidget
{
public:

	NativeComboBox(EUIWidget* owner) : WinWidget(owner) {};

	virtual void ClearList() = 0;
	virtual void AddItem(const char* str) = 0;
	virtual void SetCurString(int index) = 0;
	virtual void SetCurString(const char* str) = 0;
	virtual int  GetCurString() = 0;
};
