
#include "StringUtils.h"
#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"
#include <stdarg.h>

map<wchar_t, int> StringUtils::upper2lower;
map<wchar_t, int> StringUtils::lower2upper;

void StringUtils::Init()
{
	JSONReader reader;

	reader.Parse("settings/low2hi.dat");

	string str;

	while (reader.EnterBlock("table"))
	{
		reader.Read("lo", str);
		unsigned int lo = std::stoul(str.c_str(), nullptr, 16);

		reader.Read("hi", str);
		unsigned int hi = std::stoul(str.c_str(), nullptr, 16);

		upper2lower[lo] = hi;
		lower2upper[hi] = lo;

		reader.LeaveBlock();
	}
}

int StringUtils::GetLen(const char* str)
{
	return (int)strlen(str);
}

bool StringUtils::IsEmpty(const char* str)
{
	return !str || !str[0];
}

const char* StringUtils::StrStr(const char* str, const char* sub_str)
{
	return strstr(str, sub_str);
}

bool StringUtils::IsEqual(const char* str1, const char* str2)
{ 
#ifdef PLATFORM_PC
	return (_stricmp(str1, str2) == 0);
#else
	return (strcasecmp(str1, str2) == 0);
#endif
}

void StringUtils::Copy(char* str1, int len, const char* str2)
{
	int size = GetLen(str2)+1;
	int mv = size;
	if (size>len) mv = len-1;

	memcpy(str1, str2, mv);

	if (size > len)
	{
		str1[len - 1] = 0;
	}
}

void StringUtils::Cat(char* str1, int len, const char* str2)
{
	#ifdef PLATFORM_PC
	strcat_s(str1, len, str2);
	#else
	strcat(str1, str2);
	#endif
	
}

void StringUtils::Printf(char* str, int len, const char* format, ...)
{
	static char buffer[4096];
	va_list args;
	va_start(args, format);

	vsnprintf(buffer, sizeof(buffer) - 4, format, args);

	va_end(args);

	Copy(str, len, buffer);
}

void StringUtils::RemoveSlashes(char* fullPath)
{
	static char buffer[1024];
	Copy(buffer, 1023, fullPath);

	int len = (int)strlen(fullPath);
	int index=0;

	int skip = 0;

	for (int i=0;i<len;i++)
	{
		if (buffer[i] == '/' || buffer[i] == '\\')
		{
			skip++;
		}
		else
		{
			skip = 0;
		}

		if (skip<2)
		{
			fullPath[index] = buffer[i];
			index++;
		}
	}

	fullPath[index] = 0;
}

void StringUtils::GetFileName(const char* fullPath, char* fileName)
{
	int index = GetLen(fullPath)-1;

	while(index>=0 && fullPath[index]!='\\' && fullPath[index]!='/')
	{
		index--;
	}

	int len = GetLen(fullPath);

	for(int i=index+1; i<len;i++)
	{
		fileName[i-index-1] = fullPath[i];
	}

	fileName[len - index - 1] = 0;
}

bool StringUtils::GetPath(const char* fullPath, char* path)
{
	int index = GetLen(fullPath)-1;

	while(index>=0 && fullPath[index]!='\\' && fullPath[index]!='/')
	{
		index--;
	}

	if (index>0)
	{	
		for(int i=0;i<=index;i++)
		{
			path[i] = fullPath[i];
		}

		path[index+1]=0;

		return true;
	}

	return false;
}

void StringUtils::GetCropPath(const char* relativePath, const char* fullPath, char* path, int len)
{
	if (fullPath[1] != ':')
	{
		Copy(path, len, fullPath);
		return;
	}

	int len1 = GetLen(fullPath);
	int len2 = GetLen(relativePath);

	if (len1 <= len2)
	{
		path[0] = 0;
		return;
	}

	for(int i= len2; i<len1; i++)
	{
		path[i- len2] = fullPath[i];
	}

	path[len1 - len2]=0;
}

void StringUtils::GetRootPath(const char* path1, const char* path2, char* root)
{
	char rt[512];
	int len = GetLen(path1);

	if (len < GetLen(path2))
	{
		len = GetLen(path2);
	}

	rt[0] = 0;

	for (int i=0;i<len;i++)
	{
		if (path1[i] == path2[i])
		{
			rt[i] = path1[i];
			rt[i+1] = 0;
		}
	}

	GetPath(rt,root);
}

void StringUtils::RemoveFirstChar(char* str)
{
	for(int i=0;i<(int)strlen(str)-1;i++)
	{
		str[i]=str[i+1];
	}

	str[strlen(str)-1]=0;
}

void StringUtils::FixSlashes(char * str)
{
	int len = GetLen(str);

	for(int i=0; i<len; i++)
	{
		if (str[i] == '\\')
		{
			str[i]='/';
		}

		if (i!=0 && str[i] == '/')
		{
			if (str[i-1] == '/')
			{
				for(int j=i;j<len;j++)
				{
					str[j] = str[j+1];
				}

				len--;
				i--;
			}
		}
	}
}

void StringUtils::GetExtension(const char* str, char* ext, int ext_lenght)
{
	ext[0] = 0;

	int  count = 0;
	int index = GetLen(str)-1;

	while (index>0&&str[index]!='.')
	{
		count++;
		
		if (ext_lenght == count) return;

		index--;
	}

	int i=index+1;

	for (i=index+1;i<(int)strlen(str);i++)
	{
		ext[i-index-1]=str[i];
	}

	ext[i-index-1]=0;
}

void StringUtils::RemoveExtension(char* str)
{
	int index = GetLen(str) - 1;

	while (index>=0&&str[index] != '.')
	{
		index--;
	}

	if (index>0)
	{
		str[index] = 0;
	}
}

int StringUtils::GetNameNumber(const char* str, char* wo_n_str, int len)
{
	Copy(wo_n_str,len,str);
	int number = 0;

	int index = GetLen(str)-1;

	if (index == 0)
	{
		return 0;
	}

	int  pow = 1;

	while (index>=0 && str[index] >= '0' && str[index] <= '9')
	{
		number += pow * (str[index] - '0');
		pow *= 10;
		index--;
	}

	wo_n_str[index+1] = 0;

	return number;
}

void StringUtils::EscapeChars(const char* in, char* out, int len)
{
	int index = 0;
	int in_len = GetLen(in);
	
	for	(int i=0;i<in_len;i++)
	{
		char c = in[i];

		if (c == ' ')
		{
			out[index] = '%';
			out[index+1] = '2';
			out[index+2] = '0';
			index += 3;
		}
		else
		if (c == ':')
		{
			out[index] = '%';
			out[index+1] = '3';
			out[index+2] = 'A';
			index += 3;
		}
		else
		if (c == '#')
		{
			out[index] = '%';
			out[index+1] = '2';
			out[index+2] = '3';
			index += 3;
		}
		else
		{
			out[index] = c;
			index ++;
		}
	}

	out[index] = 0;
}

void StringUtils::LowerCase(string& str)
{
	string text = str;
	str.clear();

	int w = 0;
	int bytes = 0;

	int len = (int)text.size();

	string tmp;
	map<wchar_t, int>::iterator it;


	for (int i = 0; i<len; i++)
	{
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		it = upper2lower.find(w);

		if (it != upper2lower.end())
		{
			w = it->second;
		}

		BuildUtf8fromUtf16(w, tmp);
		str += tmp;
	}
}

void StringUtils::UpperCase(string& str)
{
	string text = str;
	str.clear();

	int w = 0;
	int bytes = 0;

	int len = (int)text.size();

	string tmp;
	map<wchar_t, int>::iterator it;

	for (int i = 0; i<len; i++)
	{
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		it = lower2upper.find(w);

		if (it != lower2upper.end())
		{
			w = it->second;
		}

		BuildUtf8fromUtf16(w, tmp);
		str += tmp;
	}
}

bool StringUtils::CompareABC(const char* str1, const char* str2)
{
	int bytes = 0;

	int w1 = 0;
	int len1 = (int)strlen(str1);
	int index1 = 0;

	int w2 = 0;
	int len2 = (int)strlen(str2);
	int index2 = 0;

	map<wchar_t, int>::iterator it;

	bool finished = false;
	int stage = 0;

	while (!finished)
	{
		if (stage == 0)
		{
			if (BuildUtf16fromUtf8(str1[index1], bytes, w1))
			{
				//it = upper2lower.find(w1);

				//if (it != upper2lower.end())
				{
					//w1 = it->second;
				}

				stage++;
			}
			else
			{
				if (index1 + 1 >= len1)
				{
					return true;
				}
			}

			index1++;
		}
		else
		if (stage == 1)
		{
			if (BuildUtf16fromUtf8(str2[index2], bytes, w2))
			{
				//it = upper2lower.find(w2);

				//if (it != upper2lower.end())
				{
					//w2 = it->second;
				}

				stage++;
			}
			else
			{
				if (index2 + 1 >= len2)
				{
					break;
				}
			}

			index2++;
		}
		else
		{
			if (w1 != w2)
			{
				if (w1 > 128 && w2 < 128)
				{
					return true;
				}

				if (w2 > 128 && w1 < 128)
				{
					return false;
				}

				return (w1 < w2);
			}

			if (index1 >= len1 && index2 < len2)
			{
				return true;
			}

			if (index2 >= len2 && index1 < len1)
			{
				break;
			}

			stage = 0;
		}
	}

	return false;
}

void StringUtils::Utf16toUtf8(string& dest, const wchar_t* src)
{
	dest.clear();

	int len = (int)wcslen(src);

	for (int i = 0; i < len; i++) 
	{
		int c = src[i];

		if (c < (1 << 7))
		{
			dest += c;
		}
		else
		if (c < (1 << 11))
		{
			dest += ((c >> 6) | 0xcC0);
			dest += ((c & 0x3F) | 0x80);
		}
		else
		if (c < (1 << 16))
		{
			dest += ((c >> 12) | 0xE0);
			dest += (((c >> 6) & 0x3F) | 0x80);
			dest += ((c & 0x3F) | 0x80);
		}
		else
		if (c < (1 << 21))
		{
			dest += ((c >> 18) | 0xE0);
			dest += (((c >> 12) & 0x3F) | 0x80);
			dest += (((c >> 6) & 0x3F) | 0x80);
			dest += ((c & 0x3F) | 0x80);
		}
	}
}

bool StringUtils::BuildUtf16fromUtf8(char c, int& bytes, int& w)
{
	if (c <= 0x7f)
	{
		if (bytes)
		{
			bytes = 0;
		}

		w = (wchar_t)c;

		return true;
	}
	else
	if (c <= 0xbf)
	{
	if (bytes)
		{
			w = ((w << 6) | (c & 0x3f));
			bytes--;

			if (bytes == 0)
			{
				return true;
			}
		}
	}
	else
	if (c <= 0xdf)
	{
		bytes = 1;
		w = c & 0x1f;
	}
	else
	if (c <= 0xef)
	{
		bytes = 2;
		w = c & 0x0f;
	}
	else
	if (c <= 0xf7)
	{
		bytes = 3;
		w = c & 0x07;
	}
	else
	{
		bytes = 0;
	}

	return false;
}

void StringUtils::BuildUtf8fromUtf16(int c, string& dest)
{
	dest.clear();

	if (c < (1 << 7))
	{
		dest += c;
	}
	else
	if (c < (1 << 11))
	{
		dest += ((c >> 6) | 0xcC0);
		dest += ((c & 0x3F) | 0x80);
	}
	else
	if (c < (1 << 16))
	{
		dest += ((c >> 12) | 0xE0);
		dest += (((c >> 6) & 0x3F) | 0x80);
		dest += ((c & 0x3F) | 0x80);
	}
	else
	if (c < (1 << 21))
	{
		dest += ((c >> 18) | 0xE0);
		dest += (((c >> 12) & 0x3F) | 0x80);
		dest += (((c >> 6) & 0x3F) | 0x80);
		dest += ((c & 0x3F) | 0x80);
	}
}

void StringUtils::Utf8toUtf16(wstring& dest, const char* src)
{
	dest.clear();
	wchar_t w = 0;
	int bytes = 0;
	wchar_t err = L'?';

	int count = GetLen(src);

	for (size_t i = 0; i < count; i++)
	{
		unsigned char c = (unsigned char)src[i];

		if (c <= 0x7f)
		{
			if (bytes)
			{
				dest.push_back(err);
				bytes = 0;
			}

			dest.push_back((wchar_t)c);
		}
		else
		if (c <= 0xbf)
		{
			if (bytes)
			{
				w = ((w << 6) | (c & 0x3f));
				bytes--;
				if (bytes == 0)
				{
					dest.push_back(w);
				}
			}
			else
			{
				dest.push_back(err);
			}
		}
		else
		if (c <= 0xdf)
		{
			bytes = 1;
			w = c & 0x1f;
		}
		else
		if (c <= 0xef)
		{
			bytes = 2;
			w = c & 0x0f;
		}
		else
		if (c <= 0xf7)
		{
			bytes = 3;
			w = c & 0x07;
		}
		else
		{
			dest.push_back(err);
			bytes = 0;
		}
	}

	if (bytes) dest.push_back(err);
}


