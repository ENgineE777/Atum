
#include "SceneObject.h"
#include "SceneAsset.h"
#include "SceneManager.h"
#include "Services/Core/Core.h"

#ifdef EDITOR
#include "Editor/Project.h"
#endif

SceneObject::ScriptCallback::ScriptCallback(Scene* set_scene,const char* set_name, const char* set_ret, const char* set_decl)
{
	name = set_name;
	ret = set_ret;
	decl = set_decl;
	scene = set_scene;
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

void SceneObject::ScriptCallback::SetIntParam(int param, int param2)
{
	param_type = 3;
	int_param = param;
	int_param2 = param2;
}

void SceneObject::ScriptCallback::SetIntParam(int param, int param2, int param3)
{
	param_type = 4;
	int_param = param;
	int_param2 = param2;
	int_param3 = param3;
}

bool SceneObject::ScriptCallback::Prepare(asITypeInfo* class_type, asIScriptObject* set_class_inst, const char* method_name)
{
	ClassInst inst;
	inst.class_inst = set_class_inst;

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
		else
		if (param_type == 3)
		{
			StringUtils::Cat(prototype, 256, "int, int");
		}
		else
		if (param_type == 4)
		{
			StringUtils::Cat(prototype, 256, "int, int, int");
		}
	}

	StringUtils::Cat(prototype, 256, ")");

	inst.method = class_type->GetMethodByDecl(prototype);

	if (!inst.method)
	{
		core.Log("ScriptErr", "Callabck %s was not found", prototype);

		return false;
	}

	instances.push_back(inst);

	return true;
}

void SceneObject::ScriptCallback::Unbind(asIScriptObject* class_inst)
{
	for (int i = 0; i < instances.size(); i++)
	{
		if (instances[i].class_inst == class_inst)
		{
			instances.erase(instances.begin() + i);
			i--;
		}
	}
}

bool SceneObject::ScriptCallback::Call(ScriptContext* context, ...)
{
	bool res = true;

	for (auto& inst : instances)
	{
		context->ctx->Prepare(inst.method);
		context->ctx->SetObject(inst.class_inst);

		bool fail = false;
		int len = (int)strlen(decl);

		int count = (int)(len * 0.5f);
		va_list args;
		va_start(args, context);

		static string param[7];
		int str_index = 0;
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
					param[str_index] = va_arg(args, const char*);
					context->ctx->SetArgObject(cur_arg, &param[str_index]);
					str_index++;
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
			if (param_type == 3)
			{
				context->ctx->SetArgDWord(cur_arg, int_param);
				context->ctx->SetArgDWord(cur_arg + 1, int_param2);
			}
			else
			if (param_type == 4)
			{
				context->ctx->SetArgDWord(cur_arg, int_param);
				context->ctx->SetArgDWord(cur_arg + 1, int_param2);
				context->ctx->SetArgDWord(cur_arg + 2, int_param3);
			}

			core.scripts.script_caller = scene;

			if (context->ctx->Execute() != 0)
			{
				res = false;
				core.Log("ScriptErr", "Error occured in call of '%s'", name);
			}
		}
	}

	return res;
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
void SceneObject::Set2DPos(Vector2 pos)
{

}

void SceneObject::SaveInstancesRef(JSONWriter& writer)
{

}

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

void SceneObject::SetScene(Scene* set_scene)
{
	if (scene == set_scene)
	{
		return;
	}

	scene->DeleteObject(this, IsAsset(), false);

	scene->taskPool->DelAllTasks(this, set_scene->taskPool);

	for (auto comp : components)
	{
		scene->taskPool->DelAllTasks(comp);
	}

	scene->renderTaskPool->DelAllTasks(this, set_scene->renderTaskPool);

	for (auto comp : components)
	{
		scene->renderTaskPool->DelAllTasks(comp, set_scene->renderTaskPool);
	}

	scene->DelFromAllGroups(this, set_scene);

	scene = set_scene;
	scene->AddObject(this, IsAsset());
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
	return parent_uid;
}

void SceneObject::SetState(State set_state)
{
	state = set_state;
}

SceneObject::State SceneObject::GetState()
{
#ifdef EDITOR
	if (!scene->Playing() && project.LayerHiden(layer_name.c_str()))
	{
		return State::Invisible;
	}
#endif

	return state;
}

Matrix* SceneObject::Trans()
{
	return nullptr;
}

bool SceneObject::UsingOwnCamera()
{
	return false;
}

SceneObjectComp* SceneObject::AddComponent(const char* comp_name)
{
	auto decl = ClassFactorySceneObjectComp::Find(comp_name);

	if (!decl)
	{
		return nullptr;
	}

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
	if (scene)
	{
		for (auto comp : components)
		{
			scene->taskPool->DelAllTasks(comp);
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

	if (scene)
	{
		for (auto comp : components)
		{
			scene->renderTaskPool->DelAllTasks(comp);
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

	return scene->taskPool;
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

	return scene->renderTaskPool;
}

bool SceneObject::Play()
{
	return true;
}

#ifdef EDITOR
void SceneObject::Export()
{

}
#endif

ScriptContext* SceneObject::Script()
{
	return scene->script;
}

PhysScene* SceneObject::PScene()
{
	return core.scene_manager.pscene;
}

void SceneObject::Release()
{
	if (scene)
	{
		scene->taskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			scene->taskPool->DelAllTasks(comp);
		}
	}

#ifdef EDITOR
	if (treeview && (item || asset_item))
	{
		treeview->DeleteItem(item ? item : asset_item);
	}

	if (taskPool)
	{
		delete taskPool;
	}
#endif

	if (scene)
	{
		scene->renderTaskPool->DelAllTasks(this);

		for (auto comp : components)
		{
			scene->renderTaskPool->DelAllTasks(comp);
		}
	}

#ifdef EDITOR
	if (renderTaskPool)
	{
		core.render.DelTaskPool(renderTaskPool);
		delete renderTaskPool;
	}
#endif

	if (scene) scene->DelFromAllGroups(this);

	for (auto comp : components)
	{
		comp->Release();
	}

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

bool SceneObject::InjectIntoScript(const char* type_name, int name, void* property, const char* prefix)
{
	if (!StringUtils::IsEqual(type_name, script_class_name))
	{
		return false;
	}

	*(asPWORD*)(property) = (asPWORD)this;

	return true;
}

#ifdef EDITOR
void SceneObject::EnableTasks(bool enable)
{
	if (taskPool)
	{
		taskPool->SetActive(enable);
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

bool SceneObject::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
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

void SceneObject::OnDragObjectFromTreeView(bool is_assets_tree, SceneObject* object, Vector2 ms)
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
	if (UsingOwnCamera())
	{
		if (Is3DObject())
		{
			camera3d_pos = src->camera3d_pos;
			camera3d_angles = src->camera3d_angles;
		}
		else
		{
			camera2d_pos = src->camera2d_pos;
			camera2d_zoom = src->camera2d_zoom;
		}
	}

	if (Trans())
	{
		(*Trans()) = (*src->Trans());
	}

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

	if (ed && Is3DObject() && Trans())
	{
		Gizmo::inst->SetTrans3D(Trans());
	}
	else
	{
		Gizmo::inst->Disable();
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

void SceneObject::CorrectRefToParent()
{

}
#endif

void SceneObjectInst::Load(JSONReader& loader)
{
	SceneObject::Load(loader);
	loader.Read("asset_uid", asset_uid);

	asset = (SceneAsset*)GetScene()->FindByUID(asset_uid, 0, true);
}

void SceneObjectInst::Save(JSONWriter& saver)
{
	SceneObject::Save(saver);
	saver.Write("asset_uid", asset_uid);
}

#ifdef EDITOR
void SceneObjectInst::SetScene(Scene* set_scene)
{
	SceneObject::SetScene(set_scene);

	for (auto& inst : asset->instances)
	{
		if (inst.GetObject() == this)
		{
			inst.scene_path = set_scene->project_scene_path;
			inst.inst_uid = GetUID();
		}
	}
}

void SceneObjectInst::Copy(SceneObject* src)
{
	asset_uid = ((SceneObjectInst*)src)->asset_uid;

	if (UsingOwnCamera())
	{
		if (Is3DObject())
		{
			camera3d_pos = src->camera3d_pos;
			camera3d_angles = src->camera3d_angles;
		}
		else
		{
			camera2d_pos = src->camera2d_pos;
			camera2d_zoom = src->camera2d_zoom;
		}
	}

	if (Trans())
	{
		(*Trans()) = (*src->Trans());
	}

	src->GetMetaData()->Copy(src);
	asset = ((SceneObjectInst*)src)->asset;

	ApplyProperties();

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