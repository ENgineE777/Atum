
#include "ScriptContext.h"
#include "Services/Core/Core.h"


void ScriptExceptionCallback(asIScriptContext* context, void* obj)
{
	asIScriptFunction *func;
	const char* scriptSection;
	int line, column;

	for (asUINT n = 0; n < context->GetCallstackSize(); n++)
	{
		func = context->GetFunction(n);
		line = context->GetLineNumber(n, &column, &scriptSection);
		
		if (n == 0)
		{
			core.Log("Script", "A script exception occurred: %s in file: %s (%i,%i)", context->GetExceptionString(), scriptSection, line, column);
			core.Log("Script", "Call stack:");
		}

		core.Log("Script", "Function %s in file: %s (%i,%i)\n", func->GetDeclaration(), scriptSection, line, column);
	}
}

ScriptContext::ScriptContext(asIScriptContext* set_ctx)
{
	ctx = set_ctx;
	ctx->SetExceptionCallback(asFUNCTION(ScriptExceptionCallback), this, asCALL_CDECL);
}

void ScriptContext::Release()
{
	ctx->Release();
	delete this;
}