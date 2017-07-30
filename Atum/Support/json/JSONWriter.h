
#pragma once

#include <stdio.h>
#include "Support/Matrix.h"
#include "Support/Color.h"
#include <stdint.h>

class JSONWriter
{
	FILE* file;
	int offset;
	int depth;
	bool block_started[64];

public:

	JSONWriter();

	bool Start(const char* name);

	void StartArray(const char* name);
	void FinishArray();

	void StartBlock(const char* name);
	void FinishBlock();

	void Write(const char* name, const char* val);
	void Write(const char* name, bool val);
	void Write(const char* name, float val);
	void Write(const char* name, int val);
	void Write(const char* name, int64_t val);
	void Write(const char* name, Vector& val);
	void Write(const char* name, Matrix& val);
	void Write(const char* name, Color& val);

	void Release();

protected:

	void AddComma();
	void WriteFormatedStr(bool addComa, const char* str, ...);
	void WriteStr(const char* str);
};



