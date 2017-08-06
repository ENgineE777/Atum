
#pragma once

#include "EUIWidget.h"
#include "Native/NativeEditBox.h"

class EUIEditBox : public EUIWidget
{
	friend class WinEditBox;
	NativeEditBox* Native();

public:

	enum InputType
	{
		InputText,
		InputInteger,
		InputUInteger,
		InputFloat,
		InputUFloat
	};

protected:
	InputType inputType;
public:

	EUIEditBox(EUIWidget* parent, const char* name, float x, float y, float w, float h, InputType type);
	virtual ~EUIEditBox();

	virtual void SetText(const char* txt);
	virtual const char* GetText();
};
