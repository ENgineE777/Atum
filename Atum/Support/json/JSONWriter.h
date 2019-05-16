
#pragma once

#include <stdio.h>
#include "Support/Matrix.h"
#include "Support/Vector2.h"
#include "Support/Quaternion.h"
#include "Support/Color.h"
#include <stdint.h>
#include "string"

/**
\ingroup gr_code_common_json
*/

/**
\brief JSON Writer

This class helps to write JSON file.

*/

class JSONWriter
{
#ifndef DOXYGEN_SKIP
	FILE* file = nullptr;
	int offset = 0;
	int depth = 0;
	bool block_started[64];
#endif

public:

#ifndef DOXYGEN_SKIP
	JSONWriter();
	~JSONWriter();
#endif

	bool Start(const char* name);

	void StartArray(const char* name);
	void FinishArray();

	void StartBlock(const char* name);
	void FinishBlock();

	void Write(const char* name, std::string& val);
	void Write(const char* name, const char* val);
	void Write(const char* name, bool val);
	void Write(const char* name, float val);
	void Write(const char* name, uint16_t& val);
	void Write(const char* name, int val);
	void Write(const char* name, uint32_t val);
	void Write(const char* name, int64_t val);
	void Write(const char* name, Vector2& val);
	void Write(const char* name, Vector& val);
	void Write(const char* name, Matrix& val);
	void Write(const char* name, Color& val);
	void Write(const char* name, Quaternion& val);

	void Close();

#ifndef DOXYGEN_SKIP
protected:

	void AddComma();
	void WriteFormatedStr(bool addComa, const char* str, ...);
	void WriteStr(const char* str);
#endif
};



