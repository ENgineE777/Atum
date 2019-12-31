#pragma once
#include "ScriptContext.h"
#include "ScriptCore.h"
#include <vector>
#include <string>

/**
\ingroup gr_code_services_script
*/

/**
\brief Scripts

This service manages loading and execution of scripts.

*/

class Scripts
{
	friend class SceneObject;
	friend class SceneScriptInst;
	friend class ScriptCore;
	friend class ScriptContext;

#ifndef DOXYGEN_SKIP
	asIScriptEngine* engine = nullptr;
	ScriptContext* class_instances_ctx = nullptr;

	struct ClassInst
	{
		class Scene* scene = nullptr;
		asIScriptObject* inst = nullptr;
	};

	std::vector<ClassInst> class_instances;

	ScriptCore scrip_core;
#endif

	class BytecodeStream : public asIBinaryStream
	{
	public:
		BytecodeStream(FILE *file)
		{
			f = file;
		}

		~BytecodeStream()
		{
			if (f)
			{
				fclose(f);
			}
		}

		int Write(const void* ptr, asUINT size) override
		{
			if (size != 0)
			{
				fwrite(ptr, size, 1, f);
			}

			return size;
		}

		int Read(void* read_ptr, asUINT size) override
		{
			if (size != 0)
			{
				fread(read_ptr, size, 1, f);
			}

			return size;
		}

	protected:
		FILE * f = nullptr;
	};

	void* script_caller = nullptr;

	float time_to_fade_exception = -1.0f;
	vector<string> exception_stack;
	int string_type_id = -1;

public:

#ifdef EDITOR
	FILE* help_file = nullptr;
	bool help_out_dir = false;
	void Init();
#endif

	void Start();

	/**
	\brief Register C++ class as gloabal script type

	/param[in] name Name a script type
	/param[in] byteSize Size in bytes of C++ class
	/param[in] help_group Belonging to a group used in generation of documentation via doxygen
	/param[in] brief Description of a class used in generation of documentation via doxygen

	*/
	void RegisterObjectType(const char* name, int byteSize, const char* help_group, const char* brief);

	/**
	\brief Register method of C++ class for a gloabal script type

	/param[in] obj Name a script type
	/param[in] declaration Declaraion of a method
	/param[in] funcPointer Ponter to a method wrapped by asSFuncPtr
	/param[in] brief Description of a method used in generation of documentation via doxygen

	*/
	void RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer, const char* brief);

	/**
	\brief Register propery of C++ class for a gloabal script type

	/param[in] obj Name a script type
	/param[in] declaration Declaraion of a property
	/param[in] byteOffset Offeset in bytes from pointer of a class to a property
	/param[in] brief Description of a method used in generation of documentation via doxygen

	*/
	void RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, const char* brief);

	/**
	\brief Create instance of a script object by TypeInfo

	/param[in] type TypeInfo of isntance

	/return pointer to instance of a script object

	*/
	void* CreateScriptObject(const asITypeInfo* type);

	/**
	\brief Get a script module

	/param[in] module Name of a module
	/param[in] flag Flags

	/return Pointer to a module
	*/
	asIScriptModule* GetModule(const char* module, asEGMFlags flag);

	/**
	\brief Load byte code into a module

	/param[in] module pointer to aa module
	/param[in] file_name Path to a file which contains byte code 
	*/
	void LoadModuleByteCode(asIScriptModule* module, const char* file_name);

	/**
	\brief Save byte code of a module

	/param[in] module pointer to aa module
	/param[in] file_name Path to a file which contains byte code
	*/
	void SaveModuleByteCode(asIScriptModule* module, const char* file_name);


	/**
	\brief Get TypeInfo by Id

	/param[in] typeId Id of a type

	/return TypeInfo
	*/
	asITypeInfo* GetTypeInfoById(int typeId) const;

	/**
	\brief Create context for execution of a csript

	/return Script context 
	*/
	ScriptContext* CreateContext();

	/**
	\brief Get id of string type

	/return Id of type
	*/
	int GetStringTypeId();

#ifndef DOXYGEN_SKIP
	void Update(float dt);
	void RegisterClassInstance(Scene* scene, asIScriptObject* inst);
	void CallClassInstancesMethod(const char* scene_name, const char* class_name, const char* method);
	void UnregisterClassInstance(asIScriptObject* inst);

	void Stop();
#endif
};