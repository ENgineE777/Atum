#pragma once

#include <string>
#include <map>

/**
\ingroup gr_code_common
*/

/**
\brief Helpers functions for work with null terminated strings

Set of helpers functions tah helps work with null terminated strings

*/

using namespace std;

class StringUtils
{
#ifndef DOXYGEN_SKIP
	static map<wchar_t, int> upper2lower;
	static map<wchar_t, int> lower2upper;
#endif

public:

	/**
	\brief Init should be called before usage of methods line Deserialize data from JSON
	*/
	static void Init();

	/**
	\brief Get length of a string
	*/
	static int  GetLen(const char* str);

	/**
	\brief Check if string is empty
	*/
	static bool IsEmpty(const char* str);

	/**
	\brief Find sub string
	*/
	static const char* StrStr(const char* str, const char* sub_str);

	/**
	\brief Check if strings are equal
	*/
	static bool IsEqual(const char* str1, const char* str2);

	/**
	\brief Copy content of one string to another
	*/
	static void Copy(char * str1, int len, const char* str2);

	/**
	\brief Concatinate one string with another
	*/
	static void Cat(char* str1, int len, const char* str2);

	/**
	\brief Constract string from formated string
	*/
	static void Printf(char* str, int len, const char* format, ...);

	/**
	\brief remove all salhes in a string
	*/
	static void RemoveSlashes(char* fullPath);

	/**
	\brief Replase all '\\' slashes to '/' slashes 
	*/
	static void FixSlashes(char * str);

	/**
	\brief Remove first char in a string
	*/
	static void RemoveFirstChar(char* str);

	/**
	\brief Retrive filename from full path of a file
	*/
	static void GetFileName(const char* fullPath, char* fileName);

	/**
	\brief Retrive a path from full path name of a file
	*/
	static bool GetPath(const char* fullPath, char* path);

	/**
	\brief Get crop path
	*/
	static void GetCropPath(const char* relativePath, const char* fullPath, char* path, int len);

	/**
	\brief Get root path
	*/
	static void GetRootPath(const char* path1, const char* path2, char* root);

	/**
	\brief Get extension form file name
	*/
	static void GetExtension(const char* str, char* ext, int ext_lenght);

	/**
	\brief Remove extension in file name
	*/
	static void RemoveExtension(char* str);

	/**
	\brief Make replacement
	*/
	static void Replace(string& str, const char* from, const char* to);

	/**
	\brief Get Number from a name. For example for string 'Test678' result will be 678
	*/
	static int  GetNameNumber(const char* str, char* wo_n_str, int len);

	/**
	\brief Make escaping of ' ', ':' and '#'
	*/
	static void EscapeChars(const char* in, char* out, int len);

	/**
	\brief Get UTF-16 char form UTF-8 schunck
	*/
	static bool BuildUtf16fromUtf8(char c, int& bytes, int& w);

	/**
	\brief Build UTF-8 chunck form UTF-16 char
	*/
	static void BuildUtf8fromUtf16(int c, string& dest);

	/**
	\brief Make string lowercased
	*/
	static void LowerCase(string& str);

	/**
	\brief Make string uppercased
	*/
	static void UpperCase(string& str);

	/**
	\brief Compare alphabeticaly two strings
	*/
	static bool CompareABC(const char* str1, const char* str2);

	/**
	\brief Convert UTF-16 string to UTF-8 string
	*/
	static void Utf8toUtf16(wstring& dest, const char* src);

	/**
	\brief Convert UTF-8 string to UTF-16 string
	*/
	static void Utf16toUtf8(string& dest, const wchar_t* src);
};