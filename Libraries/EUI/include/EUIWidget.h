
#pragma once

#define NOMINMAX
#include "windows.h"
#include "CommCtrl.h"
#include <vector>
#include <string>

#include "Native/NativeWidget.h"

class EUIWidget
{
	friend class WinWidget;
	friend class WinButton;
	friend class WinLabel;
	friend class WinWindow;
	friend class WinEditBox;
	friend class WinMenu;
	friend class WinLayout;
	friend class WinPanel;
	friend class WinCheckBox;
	friend class WinListBox;
	friend class WinComboBox;
	friend class WinCategories;
	friend class EUICategories;
	friend class WinTabPanel;
	friend class WinTabSheet;
	friend class WinScrollBar;
	friend class WinTreeView;

	friend class EUI;

	friend class WinDX11Widget;
	friend class WinDX11Button;
	friend class WinDX11Label;
	friend class WinDX11Window;
	friend class WinDX11EditBox;
	friend class WinDX11Menu;
	friend class WinDX11Layout;
	friend class WinDX11Panel;
	friend class WinDX11CheckBox;
	friend class WinDX11ListBox;
	friend class WinDX11ComboBox;
	friend class WinDX11Categories;
	friend class EUIDX11Categories;
	friend class WinDX11TabPanel;
	friend class WinDX11TabSheet;
	friend class WinDX11ScrollBar;
	friend class WinDX11TreeView;

public:

	enum
	{
		OnUpdate = 1,
		OnResize = 2,
		OnDraw = 2,
	};

	class Listener
	{
	public:

		virtual void OnMouseMove(EUIWidget* sender, int mx, int my) {};

		virtual void OnLeftMouseDown(EUIWidget* sender, int mx, int my) {};
		virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my) {};
		virtual void OnLeftDoubliClick(EUIWidget* sender, int mx, int my) {};

		virtual void OnRightMouseDown(EUIWidget* sender, int mx, int my) {};
		virtual void OnRightMouseUp(EUIWidget* sender, int mx, int my) {};

		virtual void OnMiddleMouseDown(EUIWidget* sender, int mx, int my) {};
		virtual void OnMiddleMouseUp(EUIWidget* sender, int mx, int my) {};

		virtual void OnKey(EUIWidget* sender, int key) {};

		virtual void OnResize(EUIWidget* sender) {};
		virtual void OnUpdate(EUIWidget* sender) {};
		virtual void OnDraw(EUIWidget* sender) {};

		virtual void OnWinClose(EUIWidget* sender) {};
		virtual void OnWinShow(EUIWidget* sender) {};

		virtual void OnMenuItem(class EUIMenu* sender, int id) {};

		virtual void OnTabChange(class EUITabPanel* sender, int index) {};

		virtual void OnComboBoxOpened(class EUIComboBox* sender) {};
		virtual void OnComboBoxSelChange(class EUIComboBox* sender, int index) {};

		virtual void OnListBoxSelChange(class EUIListBox* sender, int index) {};
		virtual void OnListBoxDblClick(class EUIListBox* sender, int index) {};

		virtual void OnEditBoxStopEditing(class EUIEditBox* sender) {};

		virtual void OnSrollerPosChange(class EUIScrollBar* sender, int pos) {};

		virtual bool OnTreeViewItemDragged(class EUITreeView* sender, class EUIWidget* target, void* item, int prev_child_index, void* parent, int child_index) { return false; };
		virtual void OnTreeViewSelChange(class EUITreeView* sender, void* item) {};
		virtual void OnTreeViewSelItemTextChanged(class EUITreeView* sender, void* item, const char* text) {};
		virtual void OnTreeReCreateItem(class EUITreeView* sender, void* item, void* ptr) {};
		virtual void OnTreeDeleteItem(class EUITreeView* sender, void* item, void* ptr) {};
		virtual void OnTreeViewRightClick(class EUITreeView* sender, int x, int y, void* item, int child_index) {};
	};

protected:

	int x,y;
	int width,height;
	bool  visible;
	bool  enabled;

	int id;
	std::string text;
	
	EUIWidget* parent;
	std::vector<EUIWidget*> childs;

	void* user_data = nullptr;

	EUIWidget* focused;

	NativeWidget* nativeWidget;

	Listener* listener;

	uint32_t listenerFlag;

	virtual void OnChildShow(int index, bool set);

	virtual void AddChild(EUIWidget* child);
	virtual void DelChild(EUIWidget* child);

public:

	EUIWidget(EUIWidget* prnt, const char* txt);

	virtual ~EUIWidget();

	virtual int  GetID();

	void SetUserData(void* user_data);
	void* GetUserData();

	virtual void SetListener(int id, Listener* listener, uint32_t flag);
	virtual void SetID(int id);

	virtual void Show(bool set);
	virtual bool IsVisible();

	virtual void Enable(bool set);
	virtual bool IsEnabled();

	virtual void SetPos(int x, int y);
	virtual int  GetX();
	virtual int  GetY();

	virtual void SetSize(int w, int h);
	virtual int  GetWidth();
	virtual int  GetHeight();

	virtual void SetText(const char* txt);
	virtual const char* GetText();

	virtual void DeleteChilds();
	virtual int  GetIndexAsChild();

	virtual void GetMousePos(int& x, int& y);

	virtual void Update();
	virtual void Redraw();

	virtual void CaptureMouse();
	virtual void ReleaseMouse();

	virtual void* GetNative();
	virtual EUIWidget* GetRoot();

	EUIWidget* GetParent();
	int        GetChildsCount();
	EUIWidget* GetChild(int index);

	virtual void Release();

	virtual void SetFocused();
	virtual bool IsFocused();

	virtual bool IsHowered();
};
