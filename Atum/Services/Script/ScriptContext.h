#pragma once
#include <angelscript.h>
#include "Libs/aswrappedcall.h"

/**
\ingroup gr_code_services_script
*/

class ScriptContext
{
public:
	friend class Scripts;
	asIScriptContext* ctx;
	ScriptContext(asIScriptContext* ctx);
public:
	void Release();
};