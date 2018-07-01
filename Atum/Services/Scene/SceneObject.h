
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

	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;

protected:
	Scene* owner;
	std::string  name;
	std::string  className;
#ifdef EDITOR
	bool         edited = false;
#endif

public:

#ifdef EDITOR
	static EUITreeView *  asset_treeview;
	static EUICategories* cat;
	static EUIEditBox*    objName;
	static EUIMenu*       popup_menu;
	static EUIPanel*      vieport;
#endif

	SceneObject();
	virtual ~SceneObject();

	const char* GetName();
	virtual void SetName(const char* name);
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
	ScriptContext* Scipt();
	PhysScene* PScene();
	virtual void Release();
	virtual bool Is3DObject();

#ifdef EDITOR
	void* item = nullptr;
	virtual bool UseAseetsTree();
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
