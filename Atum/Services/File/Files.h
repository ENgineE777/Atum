
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include <map>
#include <string>
#include "Support/StringUtils.h"

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

/**
\ingroup gr_code_services_files
*/

class Files
{
public:

#ifdef PLATFORM_ANDROID
	AAssetManager * android_asset_manager;
#endif

	void Init();
	FILE* FileOpen(const char* name, const char* mode);
	bool IsFileExist(const char* name);

#ifdef PLATFORM_PC
	void* active_path = nullptr;
	std::map<void*, std::string> pathes;

	inline bool GetPath(char* path, const char* name)
	{
		if (active_path && pathes.count(active_path) > 0)
		{
			StringUtils::Printf(path, 1024, "%s\\%s", pathes[active_path].c_str(), name);
			return true;
		}

		return false;
	}

	void SetActivePath(void* object);
	void MakePathRelative(string& path, const char* file_name);
	void AddRootPath(void* object, const char* path);
	void DelRootPath(void* object);
#endif
};
