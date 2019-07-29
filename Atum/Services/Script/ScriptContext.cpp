
#include "ScriptContext.h"
#include "Services/Core/Core.h"


void ScriptContext::ScriptExceptionCallback(asIScriptContext* context, void* obj)
{
	asIScriptFunction *func;
	const char* scriptSection;
	int line, column;

	char str[1024];

	core.scripts.time_to_fade_exception = 5.0f;
	core.scripts.exception_stack.clear();

	for (asUINT n = 0; n < context->GetCallstackSize(); n++)
	{
		func = context->GetFunction(n);
		line = context->GetLineNumber(n, &column, &scriptSection);
	
		if (n == 0)
		{
			StringUtils::Printf(str, 1024, "A script exception occurred: %s in file: %s (%i,%i)", context->GetExceptionString(), scriptSection, line, column);
			core.Log("Script", str);
			core.scripts.exception_stack.push_back(str);

			core.Log("Script", "Call stack:");
			core.scripts.exception_stack.push_back("Call stack:");
		}

		StringUtils::Printf(str, 1024, "Function %s in file: %s (%i,%i)\n", func->GetDeclaration(), scriptSection, line, column);
		core.Log("Script", str);
		core.scripts.exception_stack.push_back(str);
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