
#pragma once

#include "EUITabPanel.h"
#include "Native/NativeTabSheet.h"

class EUITabSheet : public EUIWidget
{
	friend class WinTabSheet;
	friend class EUITabPanel;

	NativeTabSheet* Native();

	EUITabSheet(EUITabPanel* parent, const char* txt);
	virtual ~EUITabSheet();

public:

	virtual void SetText(const char* text);
	virtual void MakeAsCurrent();
};
