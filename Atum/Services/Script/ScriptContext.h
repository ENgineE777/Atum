#pragma once
#include <angelscript.h>

class ScriptContext
{
public:
	friend class Scripts;
	asIScriptContext* ctx;
	ScriptContext(asIScriptContext* ctx);
public:
	void Release();
};