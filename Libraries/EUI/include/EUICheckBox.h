
#pragma once

#include "EUIWidget.h"
#include "Native/NativeCheckBox.h"

class EUICheckBox : public EUIWidget
{
	friend class WinCheckBox;
	bool checked;

	NativeCheckBox* Native();

public:
	
	EUICheckBox(EUIWidget* parent, const char* txt, int x, int y, int w, int h);
	virtual ~EUICheckBox();

	virtual void SetText(const char* txt);
	void SetChecked(bool set);
	bool IsChecked();
};
