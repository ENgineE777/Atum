
#pragma once

#include "EUIWidget.h"
#include "Native/NativeComboBox.h"

class EUIComboBox : public EUIWidget
{
	friend class WinComboBox;
	friend class WinDX11ComboBox;
	int list_height = 0;

	NativeComboBox* Native();

public:

	EUIComboBox(EUIWidget* parent, int x, int y, int w, int h, int list_height);
	virtual ~EUIComboBox();

	virtual void ClearList();
	virtual void AddItem(const char* str);
	virtual void SetCurString(int index);
	virtual void SetCurString(const char* str);
	virtual const char* GetCurString();
	virtual int  GetCurStringIndex();
};
