
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>
#include "Services/TaskExecutor/TaskExecutor.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Scene.h"
#include "SceneObjectComp.h"
#include "ExecuteLevels.h"

#ifdef EDITOR
#include "Editor/Gizmo.h"
#endif

class SceneObject : public Object
{
	friend class Scene;
	friend class UIViewAsset;

public:
	enum State
	{
		Invisible,
		Inactive,
		Active
	};

	class ScriptCallback
	{
		asIScriptObject* class_inst = nullptr;
		asIScriptFunction* method = nullptr;
		const char* name = nullptr;
		const char* ret = nullptr;
		const char* decl = nullptr;
		int param_type = 0;
		int int_param = 0;
		string* str_param = nullptr;

	public:
		ScriptCallback(const char* name, const char* ret, const char* decl);
		inline const char* GetName() { return name; };
		void SetIntParam(int param);
		void SetStringParam(string& param);
		void Prepare(asITypeInfo* class_type, asIScriptObject* class_inst, const char* method_name);
		bool Call(ScriptContext* context, ...);
	};

	vector<ScriptCallback> script_callbacks;
	SceneObject::ScriptCallback* FindScriptCallback(const char* name);

protected:
	
	Scene* owner = nullptr;
	std::string name;
	std::string group_name;
	uint32_t uid = 0;
	State state = Active;
	Vector2 cam2d_pos = 0.0f;
#ifdef EDITOR
	bool edited = false;
	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
#endif

public:

	const char* class_name = nullptr;
	const char* script_class_name = nullptr;

#ifdef EDITOR
	static EUITreeView *  ed_asset_treeview;
	static EUICategories* ed_obj_cat;
	static EUIMenu*       ed_popup_menu;
	static EUIPanel*      ed_vieport;
#endif

	vector<SceneObjectComp*> components;

	SceneObject() = default;
	virtual ~SceneObject() = default;

	const char* GetName();
	virtual void SetName(const char* name);
	virtual uint32_t GetUID();
	virtual uint32_t GetParentUID();
	virtual void SetState(int state);
	virtual State GetState();
	virtual Matrix& Trans();
	virtual bool UsingCamera2DPos();
	virtual Vector2& Camera2DPos();

	virtual SceneObjectComp* AddComponent(const char* name);
	virtual void Init() = 0;
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
	virtual void Load(JSONReader& reader);
	virtual void Save(JSONWriter& writer);
	virtual TaskExecutor::SingleTaskPool* Tasks(bool edtitor);
	virtual TaskExecutor::SingleTaskPool* RenderTasks(bool editor);
	virtual bool Play();
	virtual void Stop();
	bool Playing();
	Scene* GetScene();
	ScriptContext* Script();
	PhysScene* PScene();
	virtual void Release();
	virtual bool Is3DObject();
	virtual SceneObject* GetChild(uint32_t uid);

	virtual void BindClassToScript();
	virtual bool InjectIntoScript(const char* type, void* property);

	virtual bool OnContact(int index, SceneObject* contact_object, int contact_index);

#ifdef EDITOR
	void* item = nullptr;
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
	virtual void OnRightMouseDown(Vector2 ms);
	virtual void OnRightMouseUp();
	virtual void OnPopupMenuItem(int id);
#endif
};

class SceneObjectInst : public SceneObject
{
public:
	uint32_t asset_uid;
	class SceneAsset* asset = nullptr;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

#ifdef EDITOR
	void Copy(SceneObject* src);
#endif
};

CLASSFACTORYDEF(SceneObject)
CLASSFACTORYDEF_END()

#define BASE_SCENE_OBJ_STATE_PROP(className)\
ENUM_PROP(className, state, 2, "Common", "State")\
	ENUM_ELEM("Invisible", 0)\
	ENUM_ELEM("Inactive", 1)\
	ENUM_ELEM("Active", 2)\
ENUM_END

#define BASE_SCENE_OBJ_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")\
BASE_SCENE_OBJ_STATE_PROP(className)\
STRING_PROP(className, group_name, "", "Common", "Group")

#define BASE_SCENE_ASSET_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")

#define BIND_TYPE_TO_SCRIPT(className)\
scripts.engine->RegisterObjectType(script_class_name, sizeof(className), asOBJ_REF | asOBJ_NOCOUNT);\
scripts.engine->RegisterObjectMethod(script_class_name, "void SetState(int)", WRAP_MFN(className, SetState), asCALL_GENERIC);\
scripts.engine->RegisterObjectMethod(script_class_name, "int GetState()", WRAP_MFN(className, GetState), asCALL_GENERIC);\
GetMetaData()->BindToScript(scripts.engine, script_class_name);