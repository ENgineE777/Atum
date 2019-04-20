
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

class SceneObject : public Object
{
	friend class Scene;
	friend class UIViewAsset;

#ifdef EDITOR
	friend class Project;
#endif

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

	public:
		ScriptCallback(const char* name, const char* ret, const char* decl);
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

	Scene* owner = nullptr;
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

	inline Scene* GetOwner()
	{
		return owner;
	}

	const char* class_name = nullptr;
	const char* script_class_name = nullptr;

#ifdef EDITOR
	Vector2 cam2d_pos = 0.0f;
	static EUITreeView*   ed_asset_treeview;
	static EUICategories* ed_obj_cat;
	static EUIMenu*       ed_popup_menu;
	static EUIPanel*      ed_vieport;
#endif

	vector<SceneObjectComp*> components;

	SceneObject() = default;
	virtual ~SceneObject() = default;

	const char* GetName();
	virtual void SetName(const char* name);
	virtual void SetUID(uint32_t uid);
	virtual uint32_t GetUID();
	virtual uint32_t GetParentUID();
	virtual void SetState(int state);
	virtual State GetState();
	virtual Matrix& Trans();
	virtual bool UsingCamera2DPos();

	virtual SceneObjectComp* AddComponent(const char* name);
	virtual void DelComponent(SceneObjectComp* comp);
	virtual void Init() = 0;
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
	virtual void Load(JSONReader& reader);
	virtual void Save(JSONWriter& writer);
	virtual TaskExecutor::SingleTaskPool* Tasks(bool edtitor);
	virtual TaskExecutor::SingleTaskPool* RenderTasks(bool editor);
	virtual bool Play();
	virtual bool PostPlay();
	virtual void Stop();
	bool Playing();
	Scene* GetScene();
	ScriptContext* Script();
	PhysScene* PScene();
	virtual void Release();
	virtual bool Is3DObject();
	virtual SceneObject* GetChild(uint32_t uid);

	virtual void BindClassToScript();
	virtual bool InjectIntoScript(const char* type, void* property, const char* prefix);

	virtual void OnContact(int index, SceneObject* contact_object, int contact_index, const char* callback_name);

#ifdef EDITOR
	void* item = nullptr;
	virtual void SaveAssetData(JSONWriter& writer);
	virtual void GetUIDs(uint32_t& out_uid, uint32_t& out_child_uid);
	virtual void SetOwner(Scene* owner);
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
	void SetOwner(Scene* owner) override;
	void Copy(SceneObject* src) override;
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

#define BIND_TYPE_TO_SCRIPT(className)\
core.scripts.engine->RegisterObjectType(script_class_name, sizeof(className), asOBJ_REF | asOBJ_NOCOUNT);\
core.scripts.engine->RegisterObjectMethod(script_class_name, "void SetState(int)", WRAP_MFN(className, SetState), asCALL_GENERIC);\
core.scripts.engine->RegisterObjectMethod(script_class_name, "int GetState()", WRAP_MFN(className, GetState), asCALL_GENERIC);\
GetMetaData()->BindToScript(core.scripts.engine, script_class_name);