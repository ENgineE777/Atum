
#pragma once

#include "EUIWidget.h"
#include "Native/NativeWindow.h"

class EUIWindow : public EUIWidget
{
	friend class WinWindow;

	bool active;

	bool need_strict_size;
	bool is_minimazed;

	NativeWindow* Native();

public:

	enum Style
	{
		Normal,
		Popup,
		PopupWithCloseBtn
	};

	EUIWindow(const char* text, Style style, bool adjust, int x, int y, int w, int h);
	virtual ~EUIWindow();

	virtual bool IsActive();

	virtual void SetText(const char* txt);

	virtual void SetAtScreenCenter();
	virtual void Maximaze();
	virtual bool IsMinimazed();

	void SetNeedStrictSize(bool set_strict);
	void Close();
};
