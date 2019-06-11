#import <Foundation/NSBundle.h>
#include "Support/StringUtils.h"

FILE* FileOpenIOS(const char* path, const char* mode)
{
	char file_path[2048];

	NSString* ns_name = [[NSString alloc] initWithFormat:@"%s", path];
	NSString* ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType : @""];
    
    if (!ns_path)
    {
        return nullptr;
    }
    
	StringUtils::Copy(file_path, 2048, [ns_path UTF8String]);

	return fopen(file_path, mode);
}
