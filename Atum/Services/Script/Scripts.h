#pragma once
#include "ScriptContext.h"
#include <vector>

class Scripts
{
public:
	asIScriptEngine *engine = nullptr;
	ScriptContext* class_instances_ctx = nullptr;
	std::vector<asIScriptObject*> class_instances;
public:
	void Init();
	void Start();
	ScriptContext* CreateContext();
	void RegisterClassInstance(asIScriptObject* inst);
	void CallClassInstancesMethod(const char* method);
	void UnregisterClassInstance(asIScriptObject* inst);

	void Stop();
};