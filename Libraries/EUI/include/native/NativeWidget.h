
#pragma once

class EUIWidget;

class NativeWidget
{
public:
	EUIWidget* owner;

	NativeWidget(EUIWidget* set_owner) { owner = set_owner; };

	virtual void Show(bool set) = 0;
	virtual void Enable(bool set) = 0;
	virtual void SetPos(float set_x, float set_y) = 0;

	virtual void SetSize(float w, float h) = 0;

	virtual void SetText(const char* txt) = 0;

	virtual void SetFocused() = 0;
	virtual bool IsFocused() = 0;

	virtual bool IsHoveredByMouse() = 0;

	virtual void* GetNative() = 0;

	virtual void Draw() = 0;
	virtual void Redraw() = 0;

	virtual void Resize() = 0;

	virtual void Release() = 0;
};
