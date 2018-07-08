#pragma once

#include "EUIWidget.h"
#include "Native/NativeTreeView.h"

class EUITreeView : public EUIWidget
{
	friend class WinTreeView;

	NativeTreeView* Native();

public:

	EUITreeView(EUIWidget* parent, int x, int y, int w, int h, bool abs_sort, bool allow_edit_names);
	virtual ~EUITreeView();

	virtual void  AddImage(const char* name);
	virtual void  DeleteItem(void* item);
	virtual void  ClearTree();
	virtual void* AddItem(const char* txt, int image, void* ptr, void* parent, int child_index, bool can_have_childs, const char* tooltip = nullptr);
	virtual void  SetABSortChilds(void* item, bool sort);
	virtual void  SetItemText(void* item, const char* text);
	virtual void* GetSelectedItem();
	virtual void  SelectItem(void* item);
	virtual void  GetItemText(void* item, std::string& text);
	virtual void* GetItemPtr(void* item);
	virtual void* GetItemParent(void* item);
	virtual int   GetItemChildCount(void* item);
	virtual void* GetItemChild(void* item, int index);
};

