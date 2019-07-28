
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>
#include "Services/Core/Core.h"
#include "Scene.h"
#include "SceneObjectComp.h"
#include "ExecuteLevels.h"

#ifdef EDITOR
#include "Editor/Gizmo.h"
#endif

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneObject

This is scene object which is a base class of every entity in a scene.

*/

class SceneObject : public Object
{
	friend class Scene;
	friend class UIViewAsset; 
	friend class UIWidgetAsset;

#ifdef EDITOR
	friend class Project;
#endif

private:
	Scene * scene = nullptr;

public:
	enum State
	{
		Invisible,
		Inactive,
		Active
	};

	class ScriptCallback
	{
		struct ClassInst
		{
			asIScriptObject* class_inst = nullptr;
			asIScriptFunction* method = nullptr;
		};

		vector<ClassInst> instances;

		const char* name = nullptr;
		const char* ret = nullptr;
		const char* decl = nullptr;
		int param_type = 0;
		int int_param = 0;
		string* str_param = nullptr;
		Scene* scene = nullptr;

	public:
		ScriptCallback(Scene* scene, const char* name, const char* ret, const char* decl);
		inline const char* GetName() { return name; };
		void SetIntParam(int param);
		void SetStringParam(string& param);
		bool Prepare(asITypeInfo* class_type, asIScriptObject* class_inst, const char* method_name);
		void Unbind(asIScriptObject* class_inst);
		bool Call(ScriptContext* context, ...);
	};

	vector<ScriptCallback> script_callbacks;
	SceneObject::ScriptCallback* FindScriptCallback(const char* name);

protected:

	std::string name;
	std::string group_name;
	std::string layer_name;
	uint32_t uid = 0;
	State state = Active;
#ifdef EDITOR
	bool edited = false;
	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
#endif

public:

#ifndef DOXYGEN_SKIP

	const char* class_name = nullptr;
	const char* script_class_name = nullptr;

	Vector2 cam2d_pos = 0.0f;
#ifdef EDITOR
	static EUITreeView*   ed_asset_treeview;
	static EUICategories* ed_obj_cat;
	static EUIMenu*       ed_popup_menu;
	static EUIPanel*      ed_vieport;
#endif

	vector<SceneObjectComp*> components;

	SceneObject() = default;
	virtual ~SceneObject() = default;

	virtual void SetName(const char* name);
	virtual void SetUID(uint32_t uid);
	virtual SceneObjectComp* AddComponent(const char* name);
	virtual void DelComponent(SceneObjectComp* comp);

	virtual MetaData* GetMetaData() = 0;
#endif

	/**
	\brief Initialization of a scene object. This is a place where all task should be registered
	*/
	virtual void Init() = 0;

	/**
	\brief Get name of a scene object

	\return Name of a scene object
	*/
	const char* GetName();

	/**
	\brief Get UID of a scene object. This UID is unique among all scene objects in a project

	\return UID of a scene object
	*/
	virtual uint32_t GetUID();

	/**
	\brief Get UID of a parent of a scene object

	\return UID of a parent of a scene object
	*/
	virtual uint32_t GetParentUID();

	/**
	\brief Set state of a scene object

	\param[in] state New state of a scene object
	*/
	virtual void SetState(State state);

	/**
	\brief Get current state of a scene object

	\return Current state of a scene object
	*/
	virtual State GetState();

	/**
	\brief Get transformation in 3D space of scene object

	\return Transformation in 3D space of scene object
	*/
	virtual Matrix& Trans();

	/**
	\brief Should 2D space used for a scene object

	\return Should 2D space used for a scene object
	*/
	virtual bool UsingCamera2DPos();

	/**
	\brief This method called after each change of properties of scene object via editor
	*/
	virtual void ApplyProperties();

	/**
	\brief Load properties from JSON

	\param[in] reader JSON helper class for reading JSON
	*/
	virtual void Load(JSONReader& reader);

	/**
	\brief Save properties into JSON

	\param[in] writer JSON helper class for writing JSON
	*/
	virtual void Save(JSONWriter& writer);

	/**
	\brief Get task pool

	\param[in] editor Should be used scene task pool or created new one which will be used in the editor
	*/
	virtual TaskExecutor::SingleTaskPool* Tasks(bool editor);

	/**
	\brief Get render task pool

	\param[in] editor Should be used scene render task pool or created new one which will be used in the editor
	*/
	virtual TaskExecutor::SingleTaskPool* RenderTasks(bool editor);

	/**
	\brief This method called when scene is starting to play

	\return Result of switching a scene object in palyng state.
	*/
	virtual bool Play();

#ifdef EDITOR
	/**
	\brief This method called when scene resiources are exporting
	*/
	virtual void Export();
#endif

	/**
	\brief Get pointer to a scene owner

	\return Pointer to a scene owner
	*/
	inline Scene* GetScene()
	{
		return scene;
	}

	/**
	\brief Get pointer to a script context. Only available when scene is playing.

	\return Pointer to a script context
	*/
	ScriptContext* Script();

	/**
	\brief Get pointer to a physical scene. Only available when scene is playing.

	\return Pointer to a physical scene
	*/
	PhysScene* PScene();

	/**
	\brief Should 3D space used for a scene object

	\return Should 3D space used for a scene object
	*/
	virtual bool Is3DObject();

	/**
	\brief Get child scene object by UID

	\param[in] uid UID of a child scene object

	\return Pointer to a child scene object
	*/
	virtual SceneObject* GetChild(uint32_t uid);

	/**
	\brief Register script class for a scene object
	*/
	virtual void BindClassToScript();

	/**
	\brief Inject properties into script property

	\param[in] type Type of a script property
	\param[in] property Pointer to a script property
	\param[in] prefix Prefix which will be used to names of fileds

	\return True will be returned in case injection was successfully.
	*/
	virtual bool InjectIntoScript(const char* type, void* property, const char* prefix);

	/**
	\brief SceneObject should released only via this method
	*/
	virtual void Release();

#ifdef EDITOR
	void* item = nullptr;
	virtual void Set2DPos(Vector2 pos);
	virtual void SaveAssetData(JSONWriter& writer);
	virtual void GetUIDs(uint32_t& out_uid, uint32_t& out_child_uid);
	virtual void SetScene(Scene* scene);
	virtual void EnableTasks(bool enable);
	virtual bool HasOwnTasks();
	virtual bool IsAsset();
	virtual bool CheckSelection(Vector2 ms);
	virtual bool AddedToTreeByParent();
	virtual void AddChildsToTree(EUITreeView* treeview);
	virtual bool UseAseetsTree();
	virtual void OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms);
	virtual void ShowPropWidgets(EUICategories* objCat);
	virtual void CheckProperties();
	virtual void Copy(SceneObject* src);
	virtual int  GetInstCount();
	virtual void SetEditMode(bool ed);
	virtual bool IsEditMode();
	virtual void OnMouseMove(Vector2 delta_ms);
	virtual void OnLeftMouseDown(Vector2 ms);
	virtual void OnLeftMouseUp();
	virtual void OnRectSelect(Vector2 p1, Vector2 p2);
	virtual void OnRightMouseDown(Vector2 ms);
	virtual void OnRightMouseUp();
	virtual void OnPopupMenuItem(int id);
#endif
};

struct SceneObjectRef
{
	uint32_t uid = 0;
	uint32_t child_uid = 0;
	bool is_asset = false;
	SceneObject* object = nullptr;
};

class SceneObjectInst : public SceneObject
{
public:
	uint32_t asset_uid;
	class SceneAsset* asset = nullptr;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

#ifdef EDITOR
	void SaveAssetData(JSONWriter& writer) override;
	void SetScene(Scene* scene) override;
	void Copy(SceneObject* src) override;
#endif
};

CLASSFACTORYDEF(SceneObject)
CLASSFACTORYDEF_END()

#define BASE_SCENE_OBJ_STATE_PROP(className)\
ENUM_PROP(className, state, 2, "Common", "State", "State of a object")\
	ENUM_ELEM("Invisible", 0)\
	ENUM_ELEM("Inactive", 1)\
	ENUM_ELEM("Active", 2)\
ENUM_END

#ifdef EDITOR
extern void FillGroupsList(EUIComboBox* cbox, void* object);
extern void FillLayersList(EUIComboBox* cbox, void* object);
#endif

#ifdef EDITOR
#define BASE_SCENE_OBJ_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")\
BASE_SCENE_OBJ_STATE_PROP(className)\
STRING_ENUM_PROP(className, group_name, FillGroupsList, "Common", "Group")\
STRING_ENUM_PROP(className, layer_name, FillLayersList, "Common", "Layer")
#else
#define BASE_SCENE_OBJ_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")\
BASE_SCENE_OBJ_STATE_PROP(className)\
STRING_PROP(className, group_name, "", "Common", "Group")\
STRING_PROP(className, layer_name, "", "Common", "Layer")
#endif

#define BASE_SCENE_ASSET_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")

#define BIND_TYPE_TO_SCRIPT(className, brief)\
core.scripts.RegisterObjectType(script_class_name, sizeof(className), "gr_script_scene_objects", brief);\
core.scripts.RegisterObjectMethod(script_class_name, "void SetState(int)", WRAP_MFN(className, SetState), "Setting state of a object");\
core.scripts.RegisterObjectMethod(script_class_name, "int GetState()", WRAP_MFN(className, GetState), "Getting state of a object");\
GetMetaData()->BindToScript(script_class_name);