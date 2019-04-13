#pragma once
#include "ScriptContext.h"
#include <vector>

class Scripts
{
public:
	asIScriptEngine * engine = nullptr;
	ScriptContext* class_instances_ctx = nullptr;

	struct ClassInst
	{
		const char* scene_name = nullptr;
		asIScriptObject* inst = nullptr;
	};

	std::vector<ClassInst> class_instances;

public:
	void Init();
	void Start();
	ScriptContext* CreateContext();
	void RegisterClassInstance(const char* scene_name, asIScriptObject* inst);
	void CallClassInstancesMethod(const char* scene_name, const char* class_name, const char* method);
	void UnregisterClassInstance(asIScriptObject* inst);

	void Stop();
};