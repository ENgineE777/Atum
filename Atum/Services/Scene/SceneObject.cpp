
#include "SceneObject.h"
#include "SceneAsset.h"

#ifdef EDITOR
EUITreeView *  SceneObject::ed_asset_treeview = nullptr;
EUICategories* SceneObject::ed_obj_cat = nullptr;
EUIMenu*       SceneObject::ed_popup_menu = nullptr;
EUIPanel*      SceneObject::ed_vieport;
#endif

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

SceneObjectComp* SceneObject::AddComponent(const char* comp_name)
{
	auto decl = ClassFactorySceneObjectComp::Find(comp_name);
	SceneObjectComp* comp = decl->Create();

	comp->class_name = decl->GetName();
	comp->object = this;

	comp->Init();
	comp->GetMetaData()->Prepare(comp);
	comp->GetMetaData()->SetDefValues();

	components.push_back(comp);

	return comp;
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

Scene* SceneObject::GetScene()
{
	return owner;
}

ScriptContext* SceneObject::Script()
{
	return owner->script;
}

PhysScene* SceneObject::PScene()
{
	return owner->pscene;
}

b2World* SceneObject::PScene2D()
{
	return owner->pscene2D;
}

void SceneObject::Release()
{
	if (owner)
	{
		owner->taskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			owner->taskPool->DelAllTasks(comp);
		}
	}

	if (taskPool)
	{
		taskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			taskPool->DelAllTasks(comp);
		}
	}

	if (owner)
	{
		owner->renderTaskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			owner->renderTaskPool->DelAllTasks(comp);
		}
	}

	if (renderTaskPool)
	{
		renderTaskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			renderTaskPool->DelAllTasks(comp);
		}
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

void SceneObject::InjectIntoScript(const char* type, void* property)
{
	if (!StringUtils::IsEqual(type, script_class_name))
	{
		return;
	}

	*(asPWORD*)(property) = (asPWORD)this;
}

#ifdef EDITOR
bool SceneObject::IsAsset()
{
	return false;
}

bool SceneObject::CheckSelection(Vector2 ms)
{
	return false;
}

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

void SceneObject::ShowPropWidgets(EUICategories* objCat)
{
	if (objCat)
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->PrepareWidgets(objCat);

		for (auto comp : components)
		{
			comp->ShowPropWidgets(objCat);
		}
	}
	else
	{
		GetMetaData()->HideWidgets();

		for (auto comp : components)
		{
			comp->ShowPropWidgets(nullptr);
		}
	}
}

void SceneObject::CheckProperties()
{
	if (GetMetaData()->IsValueWasChanged())
	{
		ApplyProperties();
	}

	for (auto comp : components)
	{
		if (comp->GetMetaData()->IsValueWasChanged())
		{
			comp->ApplyProperties();
		}
	}
}

void SceneObject::Copy(SceneObject* src)
{
	Trans() = src->Trans();
	src->GetMetaData()->Copy(src);
	ApplyProperties();

	for (auto comp : components)
	{
		comp->Release();
	}

	components.clear();

	for (auto src_comp : src->components)
	{
		SceneObjectComp* comp = AddComponent(src_comp->class_name);
		comp->Copy(src_comp);
	}
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

void SceneObjectInst::Load(JSONReader& loader)
{
	SceneObject::Load(loader);
	loader.Read("asset_uid", asset_uid);

	asset = (SceneAsset*)owner->FindByUID(asset_uid, 0, true);
}

void SceneObjectInst::Save(JSONWriter& saver)
{
	SceneObject::Save(saver);
	saver.Write("asset_uid", asset_uid);
}

#ifdef EDITOR
void SceneObjectInst::Copy(SceneObject* src)
{
	asset_uid = ((SceneObjectInst*)src)->asset_uid;

	Trans() = src->Trans();
	src->GetMetaData()->Copy(src);
	ApplyProperties();

	asset = ((SceneObjectInst*)src)->asset;
	asset->instances.push_back(this);

	for (auto comp : components)
	{
		comp->Release();
	}

	components.clear();

	for (auto src_comp : src->components)
	{
		SceneObjectComp* comp = AddComponent(src_comp->class_name);
		comp->Copy(src_comp);

		SceneObjectInstComp* src_comp_inst = dynamic_cast<SceneObjectInstComp*>(src_comp);

		if (src_comp_inst)
		{
			((SceneObjectInstComp*)comp)->asset_comp = src_comp_inst->asset_comp;
		}
	}
}
#endif