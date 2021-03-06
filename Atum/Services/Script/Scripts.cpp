
#include "Scripts.h"
#include "Services/Core/Core.h"
#include "Libs/scriptarray.h"
#include "Libs/scriptdictionary.h"
#include "Libs/scripthandle.h"
#include "Libs/scriptmath.h"
#include "Libs/scriptvector2.h"
#include "Libs/scriptvector3.h"
#include "Libs/scriptmatrix.h"
#include "Libs/scriptstdstring.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/SceneManager.h"
#include "Services/Scene/SceneObject.h"
#include "SceneObjects/2D/Sprite.h"
#include "SceneObjects/2D/Sprite.h"

#ifdef EDITOR
#include "Editor/Project.h"
#endif

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char* type = "ERR ";

	if (msg->type == asMSGTYPE_WARNING)
	{
		type = "WARN";
	}
	else
	if (msg->type == asMSGTYPE_INFORMATION)
	{
		type = "INFO";
	}

	char str[1024];
	StringUtils::Printf(str, 1024, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);

	if (type[0] == 'E')
	{
		core.scene_manager.failure_on_scene_play_message += str;
	}

	core.Log("Script", str);
}

#ifdef EDITOR
void Scripts::Init()
{
}
#endif

bool Scripts::Start()
{
	engine = asCreateScriptEngine();

	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	RegisterStdString(engine);

	auto* type_info = engine->GetTypeInfoByName("string");
	string_type_id = type_info->GetTypeId();


	RegisterScriptArray(engine, true);
	RegisterStdStringUtils(engine);
	RegisterScriptDictionary(engine);
	RegisterScriptHandle(engine);
	RegisterScriptMath(engine);
	RegisterScriptVector2(engine);
	RegisterScriptVector3(engine);
	RegisterScriptMatrix(engine);

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

	mod = core.scripts.GetModule("Scripts", asGM_CREATE_IF_NOT_EXISTS);

#ifdef EDITOR
	GatherScriptFiiles(project.project_path);
#endif
	//LoadModuleByteCode(mod, filename.c_str());
	//if (mod)
	//{
	//	core.scripts.SaveModuleByteCode(mod, filename.c_str());
	//}

	return (mod->Build() == asSUCCESS);
}

#ifdef EDITOR
void Scripts::GatherScriptFiiles(const char* path)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	CHAR SerarchDir[200];
	CHAR SerarchParams[200];

	BOOL fFile = TRUE;

	strcpy(SerarchDir, path);

	strcpy(SerarchParams, path);
	strcat(SerarchParams, "\\*.*");

	hFile = FindFirstFile(SerarchParams, &ffd);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		while (fFile)
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char extension[64];
				StringUtils::GetExtension(ffd.cFileName, extension, 64);

				if (StringUtils::IsEqual(extension, "sns"))
				{
					char fileName[512];

					strcpy(fileName, path);
					strcat(fileName, "//");
					strcat(fileName, ffd.cFileName);

					FileInMemory file;

					if (file.Load(fileName))
					{
						mod->AddScriptSection(fileName, (const char*)file.GetData(), file.GetSize());
					}
				}
			}
			else
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
			{
				char FileName[512];

				strcpy(FileName, path);
				strcat(FileName, "//");
				strcat(FileName, ffd.cFileName);

				GatherScriptFiiles(FileName);
			}

			fFile = FindNextFile(hFile, &ffd);
		}

		FindClose(hFile);
	}

	RemoveDirectory(path);
}
#endif

void WriteStr(FILE* file, const char* str)
{
	fwrite(str, strlen(str), 1, file);
}

void Scripts::RegisterObjectType(const char* name, int byteSize, const char* help_group, const char* brief)
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

		WriteStr(help_file, "/**\n");

		str = " \\brief " + string(brief) + "\n";

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

void Scripts::RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer, const char* brief)
{
	engine->RegisterObjectMethod(obj, declaration, funcPointer, asCALL_GENERIC);

#ifdef EDITOR
	if (help_out_dir && help_file)
	{
		WriteStr(help_file, "   /**\n");

		string str = "    \\brief " + string(brief) + "\n";

		WriteStr(help_file, str.c_str());
		WriteStr(help_file, "   */\n");

		WriteStr(help_file, "\n");

		str = "   " + string(declaration) + ";\n";
		WriteStr(help_file, str.c_str());
	}
#endif
}

void Scripts::RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, const char* brief)
{
	engine->RegisterObjectProperty(obj, declaration, byteOffset);

#ifdef EDITOR
	if (help_out_dir && help_file)
	{
		WriteStr(help_file, "   /**\n");

		string str = "    \\brief " + string(brief) + "\n";

		WriteStr(help_file, str.c_str());
		WriteStr(help_file, "   */\n");

		str = "   " + string(declaration) + ";\n";
		WriteStr(help_file, str.c_str());
	}
#endif
}

ScriptContext* Scripts::CreateContext()
{
	return new ScriptContext(engine->CreateContext());
}

int Scripts::GetStringTypeId()
{
	return string_type_id;
}

void* Scripts::CreateScriptObject(const asITypeInfo* type)
{
	return engine->CreateScriptObject(type);
}

asIScriptModule* Scripts::GetModule(const char* module, asEGMFlags flag)
{
	return engine->GetModule(module, flag);
}

void Scripts::LoadModuleByteCode(asIScriptModule* module, const char* file_name)
{
	auto* file = core.files.FileOpen(file_name, "rb");

	if (file)
	{
		BytecodeStream stream(file);
		module->LoadByteCode(&stream);
	}
}

void Scripts::SaveModuleByteCode(asIScriptModule* module, const char* file_name)
{
	auto* file = core.files.FileOpen(file_name, "wb");

	if (file)
	{
		BytecodeStream stream(file);
		module->SaveByteCode(&stream);
	}
}

asITypeInfo* Scripts::GetTypeInfoById(int typeId) const
{
	return engine->GetTypeInfoById(typeId);
}

void Scripts::Update(float dt)
{
	if (time_to_fade_exception > 0.0f)
	{
		time_to_fade_exception -= dt;

		if (time_to_fade_exception <= 0.0f)
		{
			exception_stack.clear();
			time_to_fade_exception = -1.0f;
		}
		else
		{
			for (int i = 0; i < (int)exception_stack.size(); i++)
			{
				core.render.DebugPrintText(Vector2(10.0f, 50.0f + i * 40.0f), ScreenCorner::LeftTop, COLOR_YELLOW, exception_stack[i].c_str());
			}
		}
	}
}

void Scripts::RegisterClassInstance(Scene* scene, asIScriptObject* inst)
{
	ClassInst class_inst;
	class_inst.scene = scene;
	class_inst.inst = inst;
	class_instances.push_back(class_inst);
}

void Scripts::CallClassInstancesMethod(const char* scene_name, const char* class_name, const char* method)
{
	if (class_instances_ctx->ctx->GetState() == asEXECUTION_ACTIVE)
	{
		return;
	}

	void* current_script_caller = script_caller;

	char prototype[256];
	StringUtils::Printf(prototype, 256, "void %s()", method);

	for (auto& class_inst : class_instances)
	{
		if ((scene_name[0] && !StringUtils::IsEqual(scene_name, class_inst.scene->GetName())) ||
			(class_name[0] && !StringUtils::IsEqual(class_name, class_inst.inst->GetObjectType()->GetName())))
		{
			continue;
		}

		asIScriptFunction* method = class_inst.inst->GetObjectType()->GetMethodByDecl(prototype);

		if (method)
		{
			script_caller = class_inst.scene;

			class_instances_ctx->ctx->Prepare(method);
			class_instances_ctx->ctx->SetObject(class_inst.inst);

			if (class_instances_ctx->ctx->Execute() < 0)
			{
				core.Log("ScriptErr", "Error occured in call of '%s'", method);
			}
		}
	}

	script_caller = current_script_caller;
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
	if (mod)
	{
		mod->Discard();
	}

	exception_stack.clear();
	time_to_fade_exception = -1.0f;

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