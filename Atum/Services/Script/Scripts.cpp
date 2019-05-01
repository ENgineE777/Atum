
#include "Scripts.h"
#include "Services/Core/Core.h"
#include "Libs/scriptarray.h"
#include "Libs/scriptdictionary.h"
#include "Libs/scripthandle.h"
#include "Libs/scriptmath.h"
#include "Libs/scriptstdstring.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/SceneManager.h"
#include "Services/Scene/SceneObject.h"
#include "SceneObjects/2D/Sprite.h"
#include "SceneObjects/2D/Sprite.h"

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	core.Log("Script", "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
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

	scrip_core.Register(engine);

	for (const auto& decl : ClassFactorySceneObject::Decls())
	{
		SceneObject* obj = decl->Create();
		obj->script_class_name = decl->GetShortName();
		obj->BindClassToScript();
		obj->Release();
	}

	for (const auto& decl : ClassFactorySceneObjectComp::Decls())
	{
		SceneObjectComp* obj = decl->Create();
		obj->script_class_name = decl->GetShortName();
		obj->BindClassToScript();
		obj->Release();
	}

	class_instances_ctx = CreateContext();
}

void WriteStr(FILE* file, const char* str)
{
	fwrite(str, strlen(str), 1, file);
}

void Scripts::RegisterObjectType(const char* name, int byteSize, const char* help_group)
{
	engine->RegisterObjectType(name, byteSize, asOBJ_REF | asOBJ_NOCOUNT);

#ifdef EDITOR
	if (help_out_dir)
	{
		if (help_file)
		{
			WriteStr(help_file, "}\n");
			WriteStr(help_file, "}\n");
			fclose(help_file);
		}

		string file_name = "script_help//" + string(name) + ".h";
		help_file = core.files.FileOpen(file_name.c_str(), "w");

		WriteStr(help_file, "\n");

		WriteStr(help_file, "namespace Script {\n");

		WriteStr(help_file, "/**\n");

		string str = " *@ingroup " + string(help_group) + "\n";

		WriteStr(help_file, str.c_str());
		WriteStr(help_file, " */\n");

		WriteStr(help_file, "\n");

		str = "class " + string(name) + "\n";

		WriteStr(help_file, str.c_str());
		WriteStr(help_file, "{\n");
		WriteStr(help_file, "  public:\n");
	}
#endif
}

void Scripts::RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer)
{
	engine->RegisterObjectMethod(obj, declaration, funcPointer, asCALL_GENERIC);

#ifdef EDITOR
	if (help_out_dir && help_file)
	{
		string str = "   " + string(declaration) + ";\n";
		WriteStr(help_file, str.c_str());
	}
#endif
}

void Scripts::RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset)
{
	engine->RegisterObjectProperty(obj, declaration, byteOffset);

#ifdef EDITOR
	if (help_out_dir && help_file)
	{
		string str = "   " + string(declaration) + ";\n";
		WriteStr(help_file, str.c_str());
	}
#endif
}

ScriptContext* Scripts::CreateContext()
{
	return new ScriptContext(engine->CreateContext());
}

void* Scripts::CreateScriptObject(const asITypeInfo* type)
{
	return engine->CreateScriptObject(type);
}

asIScriptModule* Scripts::GetModule(const char* module, asEGMFlags flag)
{
	return engine->GetModule(module, flag);
}

asITypeInfo* Scripts::GetTypeInfoById(int typeId) const
{
	return engine->GetTypeInfoById(typeId);
}

void Scripts::RegisterClassInstance(const char* scene_name, asIScriptObject* inst)
{
	ClassInst class_inst;
	class_inst.scene_name = scene_name;
	class_inst.inst = inst;
	class_instances.push_back(class_inst);
}

void Scripts::CallClassInstancesMethod(const char* scene_name, const char* class_name, const char* method)
{
	char prototype[256];
	StringUtils::Printf(prototype, 256, "void %s()", method);

	for (auto& class_inst : class_instances)
	{
		if ((scene_name[0] && !StringUtils::IsEqual(scene_name, scene_name)) ||
			(class_name[0] && !StringUtils::IsEqual(class_name, class_inst.inst->GetObjectType()->GetName())))
		{
			continue;
		}

		asIScriptFunction* method = class_inst.inst->GetObjectType()->GetMethodByDecl(prototype);

		if (method)
		{
			class_instances_ctx->ctx->Prepare(method);
			class_instances_ctx->ctx->SetObject(class_inst.inst);

			if (class_instances_ctx->ctx->Execute() < 0)
			{
				core.Log("ScriptErr", "Error occured in call of '%s'", method);
			}
		}
	}
}

void Scripts::UnregisterClassInstance(asIScriptObject* inst)
{
	for (int index = 0; index < class_instances.size(); index++)
	{
		if (class_instances[index].inst == inst)
		{
			class_instances.erase(class_instances.begin() + index);
			break;
		}
	}
}

void Scripts::Stop()
{
#ifdef EDITOR
	if (help_out_dir && help_file)
	{
		WriteStr(help_file, "}}\n");
		fclose(help_file);
	}
#endif

	RELEASE(class_instances_ctx);

	engine->ShutDownAndRelease();
	engine = nullptr;
}