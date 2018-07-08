
#include "Scripts.h"
#include "Services/Core/Core.h"
#include "Libs/scriptarray.h"
#include "Libs/scriptdictionary.h"
#include "Libs/scripthandle.h"
#include "Libs/scriptmath.h"
#include "Libs/scriptstdstring.h"
#include "Services/Scene/SceneObject.h"

Scripts scripts;

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	core.Log("Script", "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void PrintString_Generic(asIScriptGeneric *gen)
{
	string* arg = (string*)(gen->GetArgAddress(0));
	core.Log("Script", arg->c_str());
}


void Scripts::Init()
{
}

void Scripts::Start()
{
	engine = asCreateScriptEngine();

	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	RegisterStdString(engine);
	RegisterScriptArray(engine, true);
	RegisterStdStringUtils(engine);
	RegisterScriptDictionary(engine);
	RegisterScriptHandle(engine);
	RegisterScriptMath(engine);

	engine->RegisterGlobalFunction("void Log(string&in text)", asFUNCTION(PrintString_Generic), asCALL_GENERIC);

	for (const auto& decl : ClassFactorySceneObject::Decls())
	{
		SceneObject* obj = decl->Create();
		obj->scriptClassName = decl->GetShortName();
		obj->BindClassToScript();
		obj->Release();
	}

}

ScriptContext* Scripts::CreateContext()
{
	return new ScriptContext(engine->CreateContext());
}

void Scripts::Stop()
{
	engine->ShutDownAndRelease();
	engine = nullptr;
}