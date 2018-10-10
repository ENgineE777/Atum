
#include "Files.h"
#include <sys/stat.h>

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

Files files;

void Files::Init()
{
}

FILE* Files::FileOpen(const char* name, const char* mode)
{
    if (!name[0])
    {
        return nullptr;
    }
    
#ifdef PLATFORM_ANDROID
	if (mode[0] == 'w') return nullptr;

	AAsset* asset = AAssetManager_open(android_asset_manager, name, 0);
	if (!asset) return nullptr;

	return funopen(asset, android_read, android_write, android_seek, android_close);
#endif

#ifdef PLATFORM_IOS
    char file_path[2048];
    
    NSString* ns_name = [[NSString alloc] initWithFormat:@"Bin/%s", name];
    NSString* ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType : @""];
    StringUtils::Copy(file_path, 2048, [ns_path UTF8String]);

    
	return fopen(file_path, mode);
#endif

	return fopen(name, mode);
}

bool Files::IsFileExist(const char*  name)
{
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}
