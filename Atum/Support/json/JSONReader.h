
#pragma once

#include "json.h"
#include "Support/Matrix.h"
#include "Support/Vector2.h"
#include "Support/Quaternion.h"
#include "Support/Color.h"
#include "string"

/**
\ingroup gr_code_common_json
*/

class JSONReader
{
	block_allocator allocator;
	json_value* root = nullptr;

	uint8_t* buffer = nullptr;

	int curDepth = 0;
	json_value* nodes[50];
	json_value* curNode = nullptr;

public:

	JSONReader();
	~JSONReader();

	bool Parse(const char* name);

	bool EnterBlock(const char* name);
	void LeaveBlock();

	bool IsString(const char* name);
	bool Read(const char* name, char* val, int val_len);
	bool Read(const char* name, std::string& val);
	bool Read(const char* name, bool& val);
	bool Read(const char* name, float& val);
	bool Read(const char* name, uint16_t& val);
	bool Read(const char* name, int& val);
	bool Read(const char* name, uint32_t& val);
	bool Read(const char* name, int64_t& val);
	bool Read(const char* name, Vector2& val);
	bool Read(const char* name, Vector& val);
	bool Read(const char* name, Matrix& val);
	bool Read(const char* name, Color& val);
	bool Read(const char* name, Quaternion& val);

	void Close();

protected:

	json_value* FindValue(const char* name);
};



