
#pragma once

#include "NativeInclude.h"

class NativeCategories : public WidgetImpl
{
public:

	NativeCategories(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void CalcThumb() = 0;
	virtual void UpdateChildPos() = 0;
};
