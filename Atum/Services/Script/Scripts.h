#pragma once
#include "ScriptContext.h"
#include "ScriptCore.h"
#include <vector>
#include <string>

/**
\ingroup gr_code_services_script
*/

class Scripts
{
	asIScriptEngine* engine = nullptr;
	ScriptContext* class_instances_ctx = nullptr;

	struct ClassInst
	{
		const char* scene_name = nullptr;
		asIScriptObject* inst = nullptr;
	};

	std::vector<ClassInst> class_instances;

	ScriptCore scrip_core;

public:

#ifdef EDITOR
	FILE* help_file = nullptr;
	bool help_out_dir = false;
#endif

	void Init();
	void Start();

	void RegisterObjectType(const char* name, int byteSize, const char* help_group);
	void RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer);
	void RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset);

	void* CreateScriptObject(const asITypeInfo* type);
	asIScriptModule* GetModule(const char* module, asEGMFlags flag);
	asITypeInfo* GetTypeInfoById(int typeId) const;

	ScriptContext* CreateContext();
	void RegisterClassInstance(const char* scene_name, asIScriptObject* inst);
	void CallClassInstancesMethod(const char* scene_name, const char* class_name, const char* method);
	void UnregisterClassInstance(asIScriptObject* inst);

	void Stop();
};