
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeCheckBox : public WinWidget
{
public:

	NativeCheckBox(EUIWidget* owner) : WinWidget(owner) {};

	virtual void SetText(const char* txt) = 0;
	virtual void SetChecked(bool set) = 0;
};
