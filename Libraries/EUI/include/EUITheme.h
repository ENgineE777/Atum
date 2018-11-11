
#pragma once

#include "json/JSONParser.h"

class EUITheme
{
public:

	int  categoryHeight = 20;
	char themePath[512];

	EUITheme();

	void Init(const char* set_theme_path, const char* name);
	uint8_t* LoadFile(const char* name, uint32_t& size);
	virtual void* GetRenderDevice() { return nullptr; };
	virtual void ReadTheme(JSONParser& parser) = 0;
	virtual void Ulnload() = 0;
};