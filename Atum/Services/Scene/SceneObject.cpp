
#include "SceneObject.h"
#include "SceneAsset.h"
#include "SceneManager.h"
#include "Services/Core/Core.h"

#ifdef EDITOR
#include "Editor/Project.h"

EUITreeView *  SceneObject::ed_asset_treeview = nullptr;
EUICategories* SceneObject::ed_obj_cat = nullptr;
EUIMenu*       SceneObject::ed_popup_menu = nullptr;
EUIPanel*      SceneObject::ed_vieport;
#endif

SceneObject::ScriptCallback::ScriptCallback(const char* set_name, const char* set_ret, const char* set_decl)
{
	name = set_name;
	ret = set_ret;
	decl = set_decl;
}

void SceneObject::ScriptCallback::SetIntParam(int param)
{
	param_type = 1;
	int_param = param;
}

void SceneObject::ScriptCallback::SetStringParam(string& param)
{
	param_type = 2;
	str_param = &param;
}

bool SceneObject::ScriptCallback::Prepare(asITypeInfo* class_type, asIScriptObject* set_class_inst, const char* method_name)
{
	class_inst = set_class_inst;

	char prototype[256];

	StringUtils::Copy(prototype, 256, ret);
	StringUtils::Cat(prototype, 256, " ");
	StringUtils::Cat(prototype, 256, method_name);
	StringUtils::Cat(prototype, 256, "(");

	int index = 0;

	bool fail = false;
	int len = (int)strlen(decl);

	while (decl[index] != 0)
	{
		if (decl[index] != '%' || index + 1 >= len)
		{
			fail = true;
			break;
		}

		if (index > 0)
		{
			StringUtils::Cat(prototype, 256, ", ");
		}

		switch (decl[index + 1])
		{
			case 'i':
			{
				StringUtils::Cat(prototype, 256, "int");
				break;
			}
			case 'f':
			{
				StringUtils::Cat(prototype, 256, "float");
				break;
			}
			case 's':
			{
				StringUtils::Cat(prototype, 256, "string&in");
				break;
			}
			default:
			fail = true;
		}

		if (fail)
		{
			break;
		}

		index += 2;
	}

	if (param_type != 0)
	{
		if (len > 0)
		{
			StringUtils::Cat(prototype, 256, ", ");
		}

		if (param_type == 1)
		{
			StringUtils::Cat(prototype, 256, "int");
		}
		else
		if (param_type == 2)
		{
			StringUtils::Cat(prototype, 256, "string&in");
		}
	}

	StringUtils::Cat(prototype, 256, ")");

	method = class_type->GetMethodByDecl(prototype);

	if (!method)
	{
		core.Log("ScriptErr", "Callabck %s was not found", prototype);

		return false;
	}

	return true;
}

bool SceneObject::ScriptCallback::Call(ScriptContext* context, ...)
{
	if (!method || !class_inst)
	{
		return false;
	}

	context->ctx->Prepare(method);
	context->ctx->SetObject(class_inst);

	bool fail = false;
	int len = (int)strlen(decl);

	int count = (int)(len * 0.5f);
	va_list args;
	va_start(args, context);

	string param;
	int cur_arg = 0;

	for (int index = 0; cur_arg < count; cur_arg++, index +=2)
	{
		if (decl[index] != '%' || index + 1 >= len)
		{
			fail = true;
			break;
		}

		switch (decl[index + 1])
		{
			case 'i':
			{
				context->ctx->SetArgDWord(cur_arg, va_arg(args, int));
				break;
			}
			case 'f':
			{
				context->ctx->SetArgFloat(cur_arg, va_arg(args, float));
				break;
			}
			case 's':
			{
				param = va_arg(args, const char*);
				context->ctx->SetArgObject(cur_arg, &param);
				break;
			}
		default:
			fail = true;
		}

		if (fail)
		{
			break;
		}
	}

	va_end(args);

	if (!fail)
	{
		if (param_type == 1)
		{
			context->ctx->SetArgDWord(cur_arg, int_param);
		}
		else
		if (param_type == 2)
		{
			context->ctx->SetArgObject(cur_arg, str_param);
		}

		if (context->ctx->Execute() == 0)
		{
			return true;
		}

		core.Log("ScriptErr", "Error occured in call of '%s'", name);
	}

	return false;
}

SceneObject::ScriptCallback* SceneObject::FindScriptCallback(const char* name)
{
	for (auto& script_callabck : script_callbacks)
	{
		if (StringUtils::IsEqual(script_callabck.GetName(), name))
		{
			return &script_callabck;
		}
	}

	return nullptr;
}

void SceneObject::ApplyProperties()
{

}

#ifdef EDITOR
void SceneObject::GetUIDs(uint32_t& out_uid, uint32_t& out_child_uid)
{
	out_uid = GetParentUID();
	out_child_uid = GetUID();

	if (out_uid == 0)
	{
		out_uid = out_child_uid;
		out_child_uid = 0;
	}

	if (out_uid == out_child_uid)
	{
		out_child_uid = 0;
	}
}

void SceneObject::SetOwner(Scene* set_owner)
{
	owner->DeleteObject(this, IsAsset(), false);

	owner->taskPool->DelAllTasks(this, set_owner->taskPool);

	for (auto comp : components)
	{
		owner->taskPool->DelAllTasks(comp);
	}

	if (owner)
	{
		owner->renderTaskPool->DelAllTasks(this, set_owner->renderTaskPool);

		for (auto comp : components)
		{
			owner->renderTaskPool->DelAllTasks(comp, set_owner->renderTaskPool);
		}
	}

	owner->DelFromAllGroups(this, set_owner);

	owner = set_owner;
	owner->AddObject(this, IsAsset());
}
#endif

const char* SceneObject::GetName()
{
	return name.c_str();
}

void SceneObject::SetName(const char* set_name)
{
	name = set_name;
}

void SceneObject::SetUID(uint32_t set_uid)
{
	uid = set_uid;
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
#ifdef EDITOR
	if (!owner->Playing() && project.LayerHiden(layer_name.c_str()))
	{
		return State::Invisible;
	}
#endif

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
	return cam2d_pos;
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

void SceneObject::DelComponent(SceneObjectComp* comp)
{
	if (owner)
	{
		for (auto comp : components)
		{
			owner->taskPool->DelAllTasks(comp);
		}
	}

#ifdef EDITOR
	if (taskPool)
	{
		for (auto comp : components)
		{
			taskPool->DelAllTasks(comp);
		}
	}
#endif

	if (owner)
	{
		for (auto comp : components)
		{
			owner->renderTaskPool->DelAllTasks(comp);
		}
	}

#ifdef EDITOR
	if (renderTaskPool)
	{
		for (auto comp : components)
		{
			renderTaskPool->DelAllTasks(comp);
		}
	}
#endif

	for (int index = 0; index < components.size(); index++)
	{
		if (components[index] == comp)
		{
#ifdef EDITOR
			components[index]->GetMetaData()->HideWidgets();
#endif
			components[index]->Release();
			components.erase(components.begin() + index);
			break;
		}
	}
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
#ifdef EDITOR
		if (!taskPool)
		{
			taskPool = core.taskExecutor.CreateSingleTaskPool();
			taskPool->SetActive(false);
		}

		return taskPool;
#else
	return nullptr;
#endif
	}

	return owner->taskPool;
}

TaskExecutor::SingleTaskPool* SceneObject::RenderTasks(bool editor)
{
	if (editor)
	{
#ifdef EDITOR
		if (!renderTaskPool)
		{
			renderTaskPool = core.render.AddTaskPool();
			renderTaskPool->SetActive(false);
		}

		return renderTaskPool;
#else
		return nullptr;
#endif
	}

	return owner->renderTaskPool;
}

bool SceneObject::Playing()
{
	return owner->playing;
}

bool SceneObject::Play()
{
	return true;
}

bool SceneObject::PostPlay()
{
	return true;
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
	return core.scene_manager.pscene;
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

#ifdef EDITOR
	if (taskPool)
	{
		delete taskPool;
	}
#endif

	if (owner)
	{
		owner->renderTaskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			owner->renderTaskPool->DelAllTasks(comp);
		}
	}

#ifdef EDITOR
	if (renderTaskPool)
	{
		core.render.DelTaskPool(renderTaskPool);
		delete renderTaskPool;
	}
#endif

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

bool SceneObject::InjectIntoScript(const char* type, void* property, const char* prefix)
{
	if (!StringUtils::IsEqual(type, script_class_name))
	{
		return false;
	}

	*(asPWORD*)(property) = (asPWORD)this;

	return true;
}

void SceneObject::OnContact(int index, SceneObject* contact_object, int contact_index, const char* callback_name)
{
	SceneObject::ScriptCallback* callabck = FindScriptCallback(callback_name);

	if (callabck)
	{
		if (callabck->Call(Script(), index, contact_object->GetName(), contact_index))
		{
			Script()->ctx->GetReturnDWord();
		}
	}
}

#ifdef EDITOR
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

void SceneObject::OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms)
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
	cam2d_pos = src->cam2d_pos;
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

int SceneObject::GetInstCount()
{
	return 0;
}

void SceneObject::SetEditMode(bool ed)
{
	edited = ed;

	if (!ed)
	{
		Gizmo::inst->Disable();
	}
	else
	if (ed && Is3DObject())
	{
		Gizmo::inst->SetTrans3D(Trans());
	}
}

bool SceneObject::IsEditMode()
{
	return edited;
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

void SceneObject::OnRectSelect(Vector2 p1, Vector2 p2)
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
void SceneObjectInst::SetOwner(Scene* owner)
{
	SceneObject::SetOwner(owner);

	for (auto& inst : asset->instances)
	{
		if (inst.GetObject() == this)
		{
			inst.scene_path = owner->project_scene_path;
			inst.inst_uid = GetUID();
		}
	}
}

void SceneObjectInst::Copy(SceneObject* src)
{
	asset_uid = ((SceneObjectInst*)src)->asset_uid;

	Trans() = src->Trans();
	src->GetMetaData()->Copy(src);
	ApplyProperties();

	asset = ((SceneObjectInst*)src)->asset;

	asset->instances.push_back(SceneAsset::AssetInstance(this));

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