
#pragma once

#include "NativeInclude.h"

class NativeEditBox : public WidgetImpl
{
public:

	NativeEditBox(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual const char* GetText() = 0;
};
