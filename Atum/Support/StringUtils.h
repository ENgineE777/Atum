#pragma once

#include <string>
#include <map>

class StringUtils
{
public:

	static bool IsEmpty(const char * c);
	static bool IsEqual(const char * s1, const char * s2);
	static void Copy(char * s1, int len, const char * s2);
	static void Cat(char * s1, int len, const char * s2);
	static void Printf(char * s1, int len, const char * format, ...);
	static void RemoveSlashes(char* FullPath);
	static void ExtractFileName(const char* FullPath, char *FileName);
	static void ExtractPath(const char* FullPath, char *Path,bool retempty);
	static void GetCropPath(const char* RelativePath,const char* FullPath, char *Path, int len);
	static void ExtractRootPath(const char* path1, const char* path2, char* root);
	static void RemoveFirstChar(char* str);
	static void FixSlashes(char * str);
	static void ExtractExctention(const char* str,char* ext, int ext_lenght);
	static void RemoveExctention(char* str);
	static int  ExtractNameNumber(const char* str, char* wo_n_str, int len);
	static void EscapeChars(const char* in, char* out, int len);
	static bool BuildUtf16fromUtf8(char c, int& bytes, int& w);
	static void BuildUtf8fromUtf16(int c, std::string& dest);
	static void Utf8toUtf16(std::wstring& dest, const char* src);
	static void Utf16toUtf8(std::string& dest, const wchar_t* src);
};