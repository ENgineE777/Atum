
#pragma once

#include "EUIWindow.h"
#include "EUIButton.h"
#include "EUILabel.h"
#include "EUIEditBox.h"
#include "EUIMenu.h"
#include "EUILayout.h"
#include "EUIPanel.h"
#include "EUICheckBox.h"
#include "EUIListBox.h"
#include "EUIComboBox.h"
#include "EUICategories.h"
#include "EUITabPanel.h"
#include "EUITabSheet.h"

class EUI
{

public:
	static std::vector<EUIWindow*> wnds;
	static void Init(const char* theme);
	static void ReloadTheme(const char* theme);
	static int  Run();
	static const char* OpenOpenDialog(char* extName, const char* ext);
	static const char* OpenSaveDialog(char* extName, const char* ext);
	static bool OpenColorDialog(float* color);
};
