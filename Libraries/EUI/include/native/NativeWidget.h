
#pragma once

class EUIWidget;

class NativeWidget
{
public:
	EUIWidget* owner;

	NativeWidget(EUIWidget* set_owner) { owner = set_owner; };

	virtual void Show(bool set) = 0;
	virtual void Enable(bool set) = 0;
	virtual void SetPos(int set_x, int set_y) = 0;

	virtual void SetSize(int w, int h) = 0;

	virtual void SetText(const char* txt) = 0;

	virtual void SetFocused() = 0;
	virtual bool IsFocused() = 0;

	virtual bool IsHowered() = 0;

	virtual void GetMousePos(int& x, int& y) = 0;

	virtual void* GetNative() = 0;

	virtual void Draw() = 0;
	virtual void Redraw() = 0;

	virtual void CaptureMouse() = 0;
	virtual void ReleaseMouse() = 0;

	virtual void Resize() = 0;

	virtual void Release() = 0;
};
