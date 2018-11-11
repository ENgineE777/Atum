
#pragma once

#include "NativeInclude.h"

class EUITabSheet;

class NativeTabSheet : public WidgetImpl
{
public:

	NativeTabSheet(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void SetText(const char* text) = 0;
	virtual void MakeAsCurrent() = 0;
};
