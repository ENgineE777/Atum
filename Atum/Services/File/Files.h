
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

/**
\brief Files

This service allows to open files. It is neccesery to work via this service with files because
pathes are diferent on each supported platform.

*/

class Files
{
public:

#ifndef DOXYGEN_SKIP

#ifdef PLATFORM_ANDROID
	AAssetManager * android_asset_manager;
#endif

	void Init();
#endif

	/**
	\brief Open file

	\param[in] name Full path to a file
	\param[in] mode Mode of openning file

	\return Handler to a file. Nullptr will returned if file doesn't exist.
	*/
	FILE* FileOpen(const char* name, const char* mode);

#ifndef DOXYGEN_SKIP

#ifdef PLATFORM_PC
	bool IsFileExist(const char* name);
	void MakePathRelative(string& path, const char* file_name);
	void CreateFolder(const char* path);
	void DeleteFolder(const char* path);
	void CopyFolder(const char* path, const char* dest_path);
	bool CpyFile(const char* src_path, const char* dest_path);
#endif

	void* active_path = nullptr;
	std::map<void*, std::string> pathes;

	inline bool GetPath(char* path, const char* name)
	{
		if (active_path && pathes.count(active_path) > 0)
		{
			StringUtils::Printf(path, 1024, "%s%s", pathes[active_path].c_str(), name);
			return true;
		}

		return false;
	}

	void SetActivePath(void* object);
	void AddRootPath(void* object, const char* path);
	void DelRootPath(void* object);

#endif

private:
	FILE* FileOpenInner(const char* path, const char* mode);
};
