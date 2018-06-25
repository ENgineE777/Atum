
#include "ScriptContext.h"


ScriptContext::ScriptContext(asIScriptContext* set_ctx)
{
	ctx = set_ctx;
}

void ScriptContext::Release()
{
	ctx->Release();
	delete this;
}