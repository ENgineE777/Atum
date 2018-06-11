
#pragma once

#include "json/JSONReader.h"

class EUITheme
{
	friend class WinTheme;

	struct ButtonColors
	{
		char backFromColor[32];
		char backToColor[32];
		char borderFromColor[32];
		char borderToColor[32];
		char textColor[32];
		char font[32];
	};

	struct CheckBoxColors
	{
		char boxBorderColor[32];
		char boxBackColor[32];
		char boxCheckColor[32];
		char textColor[32];
		char font[32];
	};

	struct CategoriesColors
	{
		char borderColor[32];
		char backColor[32];
		char openedImage[32];
		char closedImage[32];
		char textColor[32];
		char font[32];
	};

	struct ScrollbarColors
	{
		char topBorderColor[32];
		char topBackColor[32];
		char topImage[32];
		char middleBorderColor[32];
		char middleBackColor[32];
		char bottomBorderColor[32];
		char bottomBackColor[32];
		char bottomImage[32];
		char thumbBorderColor[32];
		char thumbBackColor[32];
	};

public:

	char             fontSelecetd[32];
	char             fontBackSeleceted[32];
	char             buttonFocusColor[32];
	ButtonColors     buttonColors[4];
	int              checkBoxSize;
	CheckBoxColors   checkBoxColors[2];
	int              categoryHeight;
	CategoriesColors categoriesColors[2];
	int              scrollbarThin;
	int              scrollbarPaddingX;
	int              scrollbarPaddingY;
	ScrollbarColors  scrollbarColors[2];

	char themePath[512];

	enum State
	{
		UISTATE_NORMAL   = 1,
		UISTATE_HOWERED  = 2,
		UISTATE_PUSHED   = 4,
		UISTATE_DISABLED = 8,
		UISTATE_FOCUSED  = 16
	};

	EUITheme();

	virtual void ReadTheme(const char* name);
	virtual void Ulnload() = 0;

protected:
	virtual void LoadColors(JSONReader* reader) = 0;
	virtual void LoadFonts(JSONReader* reader) = 0;
	virtual void LoadCursors(JSONReader* reader) = 0;
};