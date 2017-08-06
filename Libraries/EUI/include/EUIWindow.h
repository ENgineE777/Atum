
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
	EUIWindow(const char* text, bool popup, bool adjust, float x, float y, float w, float h);
	virtual ~EUIWindow();

	virtual void SetSize(float w, float h);

	virtual bool IsActive();

	virtual void SetText(const char* txt);

	virtual void SetAtScreenCenter();
	virtual void Maximaze();
	virtual bool IsMinimazed();

	void SetNeedStrictSize(bool set_strict);
	void Close();
};
