
#include "Services/Core/Core.h"
#include <sys/stat.h>

#ifdef PLATFORM_PC
#include "Windows.h"
#endif

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>

static int android_read(void* cookie, char* buf, int size)
{
	return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size)
{
	return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence)
{
	return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie)
{
	AAsset_close((AAsset*)cookie);
	return 0;
}
#endif

#ifdef PLATFORM_IOS
#import <Foundation/NSBundle.h>
#include "Support/StringUtils.h"

#endif

void Files::Init()
{
}

FILE* Files::FileOpenInner(const char* path, const char* mode)
{
#ifdef PLATFORM_ANDROID
	if (mode[0] == 'w') return nullptr;

	AAsset* asset = AAssetManager_open(android_asset_manager, path, 0);
	if (!asset) return nullptr;

	return funopen(asset, android_read, android_write, android_seek, android_close);
#endif

#ifdef PLATFORM_IOS
	char file_path[2048];

	NSString* ns_name = [[NSString alloc] initWithFormat:@"Bin/%s", path];
	NSString* ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType : @""];
	StringUtils::Copy(file_path, 2048, [ns_path UTF8String]);


	return fopen(file_path, mode);
#endif

#ifdef PLATFORM_PC
	return fopen(path, mode);
#endif
}

FILE* Files::FileOpen(const char* name, const char* mode)
{
	if (!name[0])
	{
		return nullptr;
	}

	FILE* file = nullptr;

	char path[1024];

	if (GetPath(path, name))
	{
		file = FileOpenInner(path, mode);
	}

	if (!file)
	{
		file = FileOpenInner(name, mode);
	}

	if (!file)
	{
		core.Log("Files", "File not found %s", name);
	}

	return file;
}

#ifdef PLATFORM_PC
bool Files::IsFileExist(const char*  name)
{
	struct stat buffer;

	char path[1024];

	if (GetPath(path, name))
	{
		if (stat(path, &buffer) == 0)
		{
			return true;
		}
	}

	return (stat(name, &buffer) == 0);
}

void Files::MakePathRelative(string& path, const char* file_name)
{
	char cur_dir[512];
	GetCurrentDirectory(512, cur_dir);

	const char* source = nullptr;

	if (active_path && pathes.count(active_path) > 0)
	{
		source = pathes[active_path].c_str();
	}
	else
	{
		source = cur_dir;
	}

	char cropped_path[1024];
	StringUtils::GetCropPath(source, file_name, cropped_path, 1024);

	if (cur_dir == source)
	{
		StringUtils::RemoveFirstChar(cropped_path);
	}

	path = cropped_path;
}

void Files::CreateFolder(const char* path)
{
	char ApplicationDir[512];
	GetCurrentDirectory(512, ApplicationDir);

	int index = 0;
	char PathTo[512];

	if (path[1] != ':')
	{
		strcpy(PathTo, "/");
	}
	else
	{
		strcpy(PathTo, "");
	}

	strcat(PathTo, path);

	while (index<(int)strlen(path))
	{
		while (index<(int)strlen(PathTo) && (PathTo[index] != '/'&&PathTo[index] != '\\'))
		{
			index++;
		}

		if (index<(int)strlen(PathTo))
		{
			char FolderTo[512];

			strcpy(FolderTo, PathTo);

			FolderTo[index] = 0;

			char FullPath[512];

			if (path[1] != ':')
			{
				strcpy(FullPath, ApplicationDir);
			}
			else
			{
				strcpy(FullPath, "");
			}

			strcat(FullPath, FolderTo);

			CreateDirectory(FullPath, NULL);
			index++;
		}
	}
}

void Files::DeleteFolder(const char* path)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	CHAR SerarchDir[200];
	CHAR SerarchParams[200];

	BOOL fFile = TRUE;

	strcpy(SerarchDir, path);

	strcpy(SerarchParams, path);
	strcat(SerarchParams, "\\*.*");

	hFile = FindFirstFile(SerarchParams, &ffd);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		while (fFile)
		{
			if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				char FileName[512];

				strcpy(FileName, path);
				strcat(FileName, "//");
				strcat(FileName, ffd.cFileName);

				DeleteFile(FileName);
			}
			else
			if (ffd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
			{
				char FileName[512];

				strcpy(FileName, path);
				strcat(FileName, "//");
				strcat(FileName, ffd.cFileName);

				DeleteFolder(FileName);

				RemoveDirectory(FileName);
			}

			fFile = FindNextFile(hFile, &ffd);
		}

		FindClose(hFile);
	}

	RemoveDirectory(path);
}

void Files::CopyFolder(const char* path, const char* dest_path)
{
	char cur_dir[521];
	StringUtils::Printf(cur_dir, 512, "%s/*.*", path);

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(cur_dir, &data);

	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!StringUtils::IsEqual(data.cFileName, ".") && !StringUtils::IsEqual(data.cFileName, ".."))
			{
				char sub_dir[521];
				StringUtils::Printf(sub_dir, 512, "%s/%s", path, data.cFileName);

				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char sub_path[521];
					StringUtils::Printf(sub_path, 512, "%s/%s", path, data.cFileName);

					char sub_dest_path[521];
					StringUtils::Printf(sub_dest_path, 512, "%s/%s", dest_path, data.cFileName);

					CopyFolder(sub_path, sub_dest_path);
				}
				else
				{
					char out_file[521];
					StringUtils::Printf(out_file, 512, "%s/%s", dest_path, data.cFileName);

					CpyFile(sub_dir, out_file);
				}
			}
		}

		while (FindNextFile(h, &data));
	}

	FindClose(h);
}

bool Files::CpyFile(const char* src_path, const char* dest_path)
{
	char ApplicationDir[512];
	GetCurrentDirectory(512, ApplicationDir);

	char strFrom[512];

	if (src_path[1] == ':')
	{
		strcpy(strFrom, src_path);
	}
	else
	{
		strcpy(strFrom, ApplicationDir);
		strcat(strFrom, "/");
		strcat(strFrom, src_path);
	}

	char strTo[512];

	if (dest_path[1] == ':')
	{
		strcpy(strTo, dest_path);
	}
	else
	{
		strcpy(strTo, ApplicationDir);
		strcat(strTo, "/");
		strcat(strTo, dest_path);
	}

	CreateFolder(dest_path);

	char buf[8192];
	size_t size;

	FILE* source = fopen(strFrom, "rb");

	if (!source)
	{
		return false;
	}

	FILE* dest = fopen(strTo, "wb");

	if (!dest)
	{
		fclose(source);
		return false;
	}

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	fclose(source);
	fclose(dest);

	return true;
}
#endif

void Files::SetActivePath(void* object)
{
	active_path = object;
}

void Files::AddRootPath(void* object, const char* path)
{
	DelRootPath(object);

	pathes[object] = path;
}

void Files::DelRootPath(void* object)
{
	if (pathes.count(object) > 0)
	{
		pathes.erase(object);
	}
}
