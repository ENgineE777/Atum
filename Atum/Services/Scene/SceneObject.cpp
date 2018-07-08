
#include "SceneObject.h"

#ifdef EDITOR
EUITreeView *  SceneObject::ed_asset_treeview = nullptr;
EUICategories* SceneObject::ed_obj_cat = nullptr;
EUIMenu*       SceneObject::ed_popup_menu = nullptr;
EUIPanel*      SceneObject::ed_vieport;
#endif


SceneObject::SceneObject()
{
	owner = nullptr;
}

SceneObject::~SceneObject()
{

}

void SceneObject::EnableTasks(bool enable)
{
	if (taskPool)
	{
		taskPool->SetActive(enable);;
	}

	if (renderTaskPool)
	{
		renderTaskPool->SetActive(enable);
	}
}

bool SceneObject::HasOwnTasks()
{
	return taskPool || renderTaskPool;
}

void SceneObject::ApplyProperties()
{

}

const char* SceneObject::GetName()
{
	return name.c_str();
}

void SceneObject::SetName(const char* set_name)
{
	name = set_name;
}

uint32_t SceneObject::GetUID()
{
	return uid;
}

uint32_t SceneObject::GetParentUID()
{
	return 0;
}

void SceneObject::SetState(int set_state)
{
	state = (State)set_state;
}

SceneObject::State SceneObject::GetState()
{
	return state;
}

Matrix& SceneObject::Trans()
{
	static Matrix mat;
	return mat;
}

bool SceneObject::UsingCamera2DPos()
{
	return false;
}

Vector2& SceneObject::Camera2DPos()
{
	static Vector2 vec = 0.0f;
	return vec;
}

const char* SceneObject::GetClassName()
{
	return className.c_str();
}

void SceneObject::Load(JSONReader& reader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(reader);
}

void SceneObject::Save(JSONWriter& writer)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(writer);
}

TaskExecutor::SingleTaskPool* SceneObject::Tasks(bool editor)
{
	if (editor)
	{
		if (!taskPool)
		{
			taskPool = taskExecutor.CreateSingleTaskPool();
			taskPool->SetActive(false);
		}

		return taskPool;
	}

	return owner->taskPool;
}

TaskExecutor::SingleTaskPool* SceneObject::RenderTasks(bool editor)
{
	if (editor)
	{
		if (!renderTaskPool)
		{
			renderTaskPool = renderTaskPool = render.AddTaskPool();
			renderTaskPool->SetActive(false);
		}

		return renderTaskPool;
	}

	return owner->renderTaskPool;
}

bool SceneObject::Playing()
{
	return owner->playing;
}

void SceneObject::Play()
{
}

void SceneObject::Stop()
{
}

ScriptContext* SceneObject::Scipt()
{
	return owner->script;
}

PhysScene* SceneObject::PScene()
{
	return owner->pscene;
}

void SceneObject::Release()
{
	if (owner) owner->taskPool->DelAllTasks(this);

	if (taskPool)
	{
		taskPool->DelAllTasks(this);
	}

	if (owner) owner->renderTaskPool->DelAllTasks(this);

	if (renderTaskPool)
	{
		renderTaskPool->DelAllTasks(this);
	}

	if (owner) owner->DelFromAllGroups(this);

	delete this;
}

bool SceneObject::Is3DObject()
{
	return false;
}

SceneObject* SceneObject::GetChild(uint32_t uid)
{
	return nullptr;
}

void SceneObject::BindClassToScript()
{
}

#ifdef EDITOR
bool SceneObject::AddedToTreeByParent()
{
	return false;
}

void SceneObject::AddChildsToTree(EUITreeView* treeview)
{
}

bool SceneObject::UseAseetsTree()
{
	return false;
}

void SceneObject::OnDragObjectFromSceneTreeView(SceneObject* object, Vector2 ms)
{

}

void SceneObject::CheckProperties()
{
	if (GetMetaData()->IsValueWasChanged())
	{
		ApplyProperties();
	}
}

void SceneObject::Copy(SceneObject* src)
{
	Trans() = src->Trans();
	src->GetMetaData()->Copy(src);
	ApplyProperties();
}

void SceneObject::SetEditMode(bool ed)
{
	edited = ed;
	Gizmo::inst->enabled = false;
}

void SceneObject::OnMouseMove(Vector2 delta_ms)
{

}

void SceneObject::OnLeftMouseDown(Vector2 ms)
{

}

void SceneObject::OnLeftMouseUp()
{

}

void SceneObject::OnRightMouseDown(Vector2 ms)
{

}

void SceneObject::OnRightMouseUp()
{

}

void SceneObject::OnPopupMenuItem(int id)
{

}
#endif