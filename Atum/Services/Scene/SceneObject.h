
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>
#include "Services/TaskExecutor/TaskExecutor.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Scene.h"

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

protected:
public:
	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
	
	Scene* owner;
	std::string name;
	std::string className;
	std::string scriptClassName;
	uint32_t uid = 0;
	State state = Active;
#ifdef EDITOR
	bool edited = false;
#endif

public:

#ifdef EDITOR
	static EUITreeView *  ed_asset_treeview;
	static EUICategories* ed_obj_cat;
	static EUIMenu*       ed_popup_menu;
	static EUIPanel*      ed_vieport;
#endif

	SceneObject();
	virtual ~SceneObject();

	const char* GetName();
	virtual void SetName(const char* name);
	virtual uint32_t GetUID();
	virtual uint32_t GetParentUID();
	virtual void SetState(int state);
	virtual State GetState();
	virtual Matrix& Trans();
	virtual bool UsingCamera2DPos();
	virtual Vector2& Camera2DPos();
	const char* GetClassName();

	virtual void EnableTasks(bool enable);
	virtual bool HasOwnTasks();
	virtual void Init() = 0;
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
	virtual void Load(JSONReader& reader);
	virtual void Save(JSONWriter& writer);
	virtual TaskExecutor::SingleTaskPool* Tasks(bool edtitor);
	virtual TaskExecutor::SingleTaskPool* RenderTasks(bool editor);
	virtual void Play();
	virtual void Stop();
	bool Playing();
	ScriptContext* Script();
	PhysScene* PScene();
	b2World* PScene2D();
	virtual void Release();
	virtual bool Is3DObject();
	virtual SceneObject* GetChild(uint32_t uid);

	virtual void BindClassToScript();


#ifdef EDITOR
	void* item = nullptr;
	virtual bool CheckSelection(Vector2 ms);
	virtual bool AddedToTreeByParent();
	virtual void AddChildsToTree(EUITreeView* treeview);
	virtual bool UseAseetsTree();
	virtual void OnDragObjectFromSceneTreeView(SceneObject* object, Vector2 ms);
	virtual void CheckProperties();
	virtual void Copy(SceneObject* src);
	virtual void SetEditMode(bool ed);
	virtual void OnMouseMove(Vector2 delta_ms);
	virtual void OnLeftMouseDown(Vector2 ms);
	virtual void OnLeftMouseUp();
	virtual void OnRightMouseDown(Vector2 ms);
	virtual void OnRightMouseUp();
	virtual void OnPopupMenuItem(int id);
#endif
};

CLASSFACTORYDEF(SceneObject)
CLASSFACTORYDEF_END()

#define BASE_SCENE_OBJ_NAME_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")

#define BASE_SCENE_OBJ_STATE_PROP(className)\
ENUM_PROP(className, state, 2, "Common", "State")\
	ENUM_ELEM("Invisible", 0)\
	ENUM_ELEM("Inactive", 1)\
	ENUM_ELEM("Active", 2)\
ENUM_END

#define BIND_TYPE_TO_SCRIPT(className)\
scripts.engine->RegisterObjectType(scriptClassName.c_str(), sizeof(className), asOBJ_REF | asOBJ_NOCOUNT);\
scripts.engine->RegisterObjectMethod(scriptClassName.c_str(), "void SetState(int)", WRAP_MFN(className, SetState), asCALL_GENERIC);\
scripts.engine->RegisterObjectMethod(scriptClassName.c_str(), "int GetState()", WRAP_MFN(className, GetState), asCALL_GENERIC);\
GetMetaData()->BindToScript(scripts.engine, scriptClassName.c_str());