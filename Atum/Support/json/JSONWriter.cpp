
#include "JSONWriter.h"
#include <string>
#include <stdarg.h>
#include "Support/StringUtils.h"

JSONWriter::JSONWriter()
{
	file = NULL;
	offset = 0;

	depth = 0;
	block_started[0] = true;
}

bool JSONWriter::Start(const char* name)
{
	file = fopen(name, "w");

	if (file)
	{
		uint8_t bom[3];
		bom[0] = 0xEF;
		bom[1] = 0xBB;
		bom[2] = 0xBF;

		fwrite(bom, 3, 1, file);

		WriteFormatedStr(false, "{\n");
		offset = 1;

		return true;
	}

	return false;
}

void JSONWriter::StartArray(const char* name)
{
	WriteFormatedStr(true, "\"%s\" : [", name);
	offset++;

	depth++;
	block_started[depth] = true;
}

void JSONWriter::FinishArray()
{
	WriteStr("\n");

	offset--;
	WriteFormatedStr(false, "]");

	depth--;
}

void JSONWriter::StartBlock(const char* name)
{
	AddComma();

	if (name)
	{
		WriteFormatedStr(false, "\"%s\" :\n", name);
	}

	WriteFormatedStr(false, "{");

	offset++;

	depth++;
	block_started[depth] = true;
}

void JSONWriter::FinishBlock()
{
	WriteStr("\n"); 

	offset--;
	WriteFormatedStr(false, "}");

	depth--;
}

void JSONWriter::Write(const char* name, const char* val)
{
	char str[1024];
	StringUtils::EscapeChars(val, str, 1024);
	StringUtils::FixSlashes(str);

	WriteFormatedStr(true, "\"%s\" : \"%s\"", name, str);
}

void JSONWriter::Write(const char* name, bool val)
{
	if (val)
	{
		WriteFormatedStr(true, "\"%s\" : true", name, val);
	}
	else
	{
		WriteFormatedStr(true, "\"%s\" : false", name, val);
	}
}

void JSONWriter::Write(const char* name, float val)
{
	WriteFormatedStr(true, "\"%s\" : %4.6f", name, val);
}

void JSONWriter::Write(const char* name, int64_t val)
{
	WriteFormatedStr(true, "\"%s\" : %i", name, val);
}

void JSONWriter::Write(const char* name, int val)
{
	WriteFormatedStr(true, "\"%s\" : %li", name, val);
}

void JSONWriter::Write(const char* name, Vector& val)
{
	StartBlock(name);
	Write("x", val.x);
	Write("y", val.y);
	Write("z", val.z);
	FinishBlock();
}

void JSONWriter::Write(const char* name, Matrix& val)
{
	StartBlock(name);

	for (int i = 0; i < 16; i++)
	{
		char tmp[16];
		sprintf(tmp, "m%i", i);
		Write(tmp, val.matrix[i]);
	}

	FinishBlock();
}

void JSONWriter::Write(const char* name, Color& val)
{
	StartBlock(name);
	Write("r", val.r);
	Write("g", val.g);
	Write("b", val.b);
	Write("a", val.a);
	FinishBlock();
}

void JSONWriter::AddComma()
{
	if (block_started[depth])
	{
		block_started[depth] = false;
	}
	else
	{
		WriteStr(",");
	}

	WriteStr("\n");
}

void JSONWriter::WriteFormatedStr(bool addComa, const char* format, ...)
{
	if (file)
	{
		if (addComa)
		{
			AddComma();
		}

		char dest[1024];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(dest, format, argptr);
		va_end(argptr);
		printf(dest);

		for (int i=0; i<offset; i++)
		{
			fwrite("\t", 1, 1, file);
		}

		fwrite(dest, strlen(dest), 1, file);
	}
}

void JSONWriter::WriteStr(const char* str)
{
	if (file)
	{
		fwrite(str, strlen(str), 1, file);
	}
}

void JSONWriter::Release()
{
	offset = 0;
	WriteStr("\n}");

	if (file)
	{
		fclose(file);
	}

	delete this;
}
