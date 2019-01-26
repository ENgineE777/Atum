
#pragma once

#include "stdio.h"
#include <stdlib.h>

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class Files
{
public:

#ifdef PLATFORM_ANDROID
	AAssetManager * android_asset_manager;
#endif

	void Init();
	FILE* FileOpen(const char* name, const char* mode);
	bool IsFileExist(const char*  name);
};
