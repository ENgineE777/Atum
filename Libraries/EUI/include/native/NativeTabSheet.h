
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class EUITabSheet;

class NativeTabSheet : public WinWidget
{
public:

	NativeTabSheet(EUIWidget* owner) : WinWidget(owner) {};

	virtual void SetText(const char* text) = 0;
	virtual void MakeAsCurrent() = 0;
};
