
#pragma once

#include "EUIWidget.h"
#include "Native/NativeListBox.h"

class EUIListBox : public EUIWidget
{
	friend class WinListBox;

	NativeListBox* Native();

public:

	EUIListBox(EUIWidget* parent, int x, int y, int w, int h, bool abs_sort);
	virtual ~EUIListBox();

	virtual void  ClearList();
	virtual void  AddItem(const char* str, void* data);
	virtual void  ChangeItemNameByIndex(const char* str, int index);
	virtual void  ChangeItemNameByData(const char* str, void* data);
	virtual int   GetSelectedItemIndex();
	virtual void* GetSelectedItemData();
	virtual void  SelectItemByIndex(int index);
	virtual void  SelectItemByData(void* data);
	virtual void  DeleteItemByIndex(int index);
	virtual void  DeleteItemByData(void* data);
};
