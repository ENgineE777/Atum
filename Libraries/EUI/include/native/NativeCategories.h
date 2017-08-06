
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeCategories : public WinWidget
{
public:

	NativeCategories(EUIWidget* owner) : WinWidget(owner) {};

	virtual void UpdateChildPos() = 0;
};
