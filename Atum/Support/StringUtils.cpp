
#include "StringUtils.h"
#include <stdarg.h>

bool StringUtils::IsEmpty(const char * c)
{
	return !c || !c[0];
}

bool StringUtils::IsEqual(const char * s1, const char * s2)
{ 
	return (_stricmp(s1, s2) == 0);
}

void StringUtils::Copy(char * s1, int len, const char * s2)
{
	int size = strlen(s2)+1;
	int mv = size;
	if (size>len) mv = len-1;

	memcpy(s1, s2, mv);	

	if (size>len) s1[len-1] = 0;
}

void StringUtils::Cat(char * s1, int len, const char * s2)
{
	#ifdef PLATFORM_PC
	strcat_s(s1, len,s2);
	#else
	strcat(s1, s2);
	#endif
	
}

void StringUtils::Printf(char * s1, int len, const char * format, ...)
{
	static char buffer[4096];
	va_list args;
	va_start(args, format);

	#ifdef PLATFORM_PC
	_vsnprintf(buffer, sizeof(buffer) - 4, format, args);
	#else
	vsnprintf(buffer, sizeof(buffer) - 4, format, args);
	#endif

	va_end(args);

	Copy(s1, len, buffer);
}

void StringUtils::RemoveSlashes(char* FullPath)
{
	static char buffer[1024];
	Copy(buffer, 1023, FullPath);

	int len = strlen(FullPath);
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
			FullPath[index] = buffer[i];
			index++;
		}
	}

	FullPath[index] = 0;
}

void StringUtils::ExtractFileName(const char* FullPath, char *FileName)
{
	int index = strlen(FullPath)-1;

	while(index>=0&&FullPath[index]!='\\'&&FullPath[index]!='/')
	{
		index--;
	}

	for(int i=(int)index+1;i<(int)strlen(FullPath);i++)
	{
		FileName[i-index-1]=FullPath[i];
	}

	FileName[strlen(FullPath)-index-1]=0;
}

void StringUtils::ExtractPath(const char* FullPath, char *Path,bool retempty)
{
	int index = strlen(FullPath)-1;

	while(index>=0&&FullPath[index]!='\\'&&FullPath[index]!='/')
	{
		index--;
	}

	if (index>0)
	{	
		for(int i=0;i<=index;i++)
		{
			Path[i]=FullPath[i];
		}

		Path[index+1]=0;
	}
	else
	{
		if (retempty) Path[0] = 0;
	}
}

void StringUtils::GetCropPath(const char* RelativePath,const char* FullPath, char* Path, int len)
{
	if (FullPath[1] != ':')
	{
		Copy(Path, len, FullPath);

		return;
	}

	if (strlen(FullPath)<=strlen(RelativePath))
	{
		Path[0] = 0;

		return;
	}

	for(int i=(int)strlen(RelativePath);i<(int)strlen(FullPath);i++)
	{
		Path[i-strlen(RelativePath)]=FullPath[i];
	}

	Path[strlen(FullPath)-strlen(RelativePath)]=0;
}

void StringUtils::ExtractRootPath(const char* path1, const char* path2, char* root)
{
	char rt[512];
	int len = strlen(path1);
	if (len < strlen(path2))
	{
		len = strlen(path2);
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

	ExtractPath(rt,root,false);
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
	int len = strlen(str);

	for(int i=0;i<len;i++)
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

void StringUtils::ExtractExctention(const char* str, char* ext, int ext_lenght)
{
	ext[0] = 0;

	int  count = 0;
	int index = strlen(str)-1;

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

void StringUtils::RemoveExctention(char* str)
{
	int index = strlen(str) - 1;

	while (index>=0&&str[index] != '.')
	{
		index--;
	}

	if (index>0)
	{
		str[index] = 0;
	}
}

int StringUtils::ExtractNameNumber(const char* str, char* wo_n_str, int len)
{
	Copy(wo_n_str,len,str);
	int number = 0;

	int index = strlen(str)-1;

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
	int in_len = strlen(in);
	
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

void StringUtils::Utf16toUtf8(std::string& dest, const wchar_t* src)
{
	dest.clear();

	int len = wcslen(src);

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

void StringUtils::BuildUtf8fromUtf16(int c, std::string& dest)
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

void StringUtils::Utf8toUtf16(std::wstring& dest, const char* src)
{
	dest.clear();
	wchar_t w = 0;
	int bytes = 0;
	wchar_t err = L'?';

	int count = strlen(src);

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


