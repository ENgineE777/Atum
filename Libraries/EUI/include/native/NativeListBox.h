
#pragma once

#include "NativeInclude.h"

class NativeListBox : public WidgetImpl
{
public:

	NativeListBox(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void  ClearList() = 0;
	virtual void  AddItem(const char* str, void* data) = 0;
	virtual void  ChangeItemNameByIndex(const char* str, int index) = 0;
	virtual void  ChangeItemNameByData(const char* str, void* data) = 0;
	virtual int   GetSelectedItemIndex() = 0;
	virtual void* GetSelectedItemData() = 0;
	virtual void  SelectItemByIndex(int index) = 0;
	virtual void  SelectItemByData(void* data) = 0;
	virtual void  DeleteItemByIndex(int index) = 0;
	virtual void  DeleteItemByData(void* data) = 0;
};
