#pragma once
#include <angelscript.h>
#include "Libs/aswrappedcall.h"

class ScriptContext
{
public:
	friend class Scripts;
	asIScriptContext* ctx;
	ScriptContext(asIScriptContext* ctx);
public:
	void Release();
};