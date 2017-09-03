#pragma once

#include <string>
#include <map>

class StringUtils
{
public:

	static int  GetLen(const char* str);
	static bool IsEmpty(const char* str);
	static bool IsEqual(const char* str1, const char* str2);
	static void Copy(char * str1, int len, const char* str2);
	static void Cat(char* str1, int len, const char* str2);
	static void Printf(char* str, int len, const char* format, ...);
	static void RemoveSlashes(char* fullPath);
	static void ExtractFileName(const char* fullPath, char* fileName);
	static bool ExtractPath(const char* fullPath, char* path);
	static void GetCropPath(const char* relativePath, const char* fullPath, char* path, int len);
	static void ExtractRootPath(const char* path1, const char* path2, char* root);
	static void RemoveFirstChar(char* str);
	static void FixSlashes(char * str);
	static void ExtractExctention(const char* str, char* ext, int ext_lenght);
	static void RemoveExctention(char* str);
	static int  ExtractNameNumber(const char* str, char* wo_n_str, int len);
	static void EscapeChars(const char* in, char* out, int len);
	static bool BuildUtf16fromUtf8(char c, int& bytes, int& w);
	static void BuildUtf8fromUtf16(int c, std::string& dest);
	static void Utf8toUtf16(std::wstring& dest, const char* src);
	static void Utf16toUtf8(std::string& dest, const wchar_t* src);
};