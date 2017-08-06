
#pragma once

#include "EUIWidget.h"
#include "Native/NativeButton.h"

class EUIButton : public EUIWidget
{
	friend class WinButton;

	bool is_pushed;
	bool is_pushable;

	bool is_howered;

	NativeButton* Native();

public:

	enum Image
	{
		Normal = 0,
		Pushed = 1,
		Howered = 2,
		Disabled = 3
	};

	EUIButton(EUIWidget* parent, const char* txt, float x, float y, float w, float h);
	virtual ~EUIButton();

	virtual void SetImage(Image img, const char* image_name);

	virtual void SetText(const char* txt);

	bool IsPushable();
	void SetPushable(bool set);

	bool IsPushed();
	void SetPushed(bool set);
};
