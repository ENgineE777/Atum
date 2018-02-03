
#pragma once

#include "EUIWidget.h"
#include "Native/NativeScrollBar.h"

class EUIScrollBar : public EUIWidget
{
	friend class WinScrollBar;

	int cur_pos;
	int max_pos;
	int page_size;
	bool horiz;

	NativeScrollBar* Native();

public:
	EUIScrollBar(EUIWidget* parent, bool horiz, int x, int y, int w, int h);
	virtual ~EUIScrollBar();

	void SetPosition(int pos);
	int  GetPosition();
	void SetLimit(int page_sz, int max_ps);
};
