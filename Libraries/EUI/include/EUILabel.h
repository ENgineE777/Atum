
#pragma once

#include "EUIWidget.h"
#include "Native/NativeLabel.h"

class EUILabel : public EUIWidget
{
	friend class WinLabel;

	NativeLabel* Native();

	bool bck_use;
	int  bck_color[3];

public:

	EUILabel(EUIWidget* parent, const char* txt, float x, float y, float w, float h);
	virtual ~EUILabel();

	virtual void SetText(const char* txt);
	virtual void SetBackgroundColor(bool use, int* color);
};
