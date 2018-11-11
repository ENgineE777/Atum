
#pragma once

#include "NativeInclude.h"

class NativeCheckBox : public WidgetImpl
{
public:

	NativeCheckBox(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void SetText(const char* txt) = 0;
	virtual void SetChecked(bool set) = 0;
};
