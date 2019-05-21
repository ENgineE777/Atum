#include "SceneScriptInst.h"
#include "Services/Core/Core.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"

#endif

#include "Services/Script/Libs/scriptarray.h"

void FillCallbackList(EUIComboBox* cbox, void* object)
{
	SceneScriptInst::Node* node = (SceneScriptInst::Node*)object;

	if (node->object)
	{
		for (auto& callback : node->object->script_callbacks)
		{
			cbox->AddItem(callback.GetName());
		}
	}
}

CLASSREG(SceneObject, SceneScriptInst, "Script")

META_DATA_DESC(SceneScriptInst::Node)
STRING_ENUM_PROP(SceneScriptInst::Node, callback_type, FillCallbackList, "Property", "callback_type")
META_DATA_DESC_END()

#ifdef EDITOR
void StartScriptInstEdit(void* owner)
{
	SceneScriptInst* script = (SceneScriptInst*)owner;

	string filename;
	script->Asset()->GetScriptFileName(filename);

	ShellExecuteA(nullptr, "open", filename.c_str(), NULL, NULL, SW_SHOW);
}

#endif

META_DATA_DESC(SceneScriptInst)
BASE_SCENE_OBJ_PROP(SceneScriptInst)
#ifdef EDITOR
CALLBACK_PROP(SpriteAsset, StartScriptInstEdit, "Prop", "EditScript")
#endif
META_DATA_DESC_END()

void SceneScriptInst::Node::Load(JSONReader& loader)
{
	loader.Read("object_uid", object_uid);
	loader.Read("object_child_uid", object_child_uid);
	loader.Read("callback_type", callback_type);
}

void SceneScriptInst::Node::Save(JSONWriter& saver)
{
	saver.Write("object_uid", object_uid);
	saver.Write("object_child_uid", object_child_uid);
	saver.Write("callback_type", callback_type);
}

void SceneScriptInst::Init()
{
	Tasks(false)->AddTask(-100, this, (Object::Delegate)&SceneScriptInst::Work);

#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SceneScriptInst::EditorWork);
#endif
}

void SceneScriptInst::Load(JSONReader& loader)
{
	SceneObjectInst::Load(loader);

	int count = 0;
	loader.Read("Count", count);
	nodes.resize(count);

	int type = 0;

	for (auto& node : nodes)
	{
		loader.EnterBlock("Node");

		loader.Read("type", type);

		node.Load(loader);

		loader.LeaveBlock();
	}
}

void SceneScriptInst::Save(JSONWriter& saver)
{
	SceneObjectInst::Save(saver);

	int count = (int)nodes.size();
	saver.Write("Count", count);

	saver.StartArray("Node");

	for (auto node : nodes)
	{
		saver.StartBlock(nullptr);

		node.Save(saver);

		saver.FinishBlock();
	}

	saver.FinishArray();
}

bool SceneScriptInst::InjectIntoScript(const char* type, void* property, const char* prefix)
{
	if (!class_inst || !StringUtils::IsEqual(type, Asset()->main_class.c_str()))
	{
		return false;
	}

	class_inst->AddRef();
	*(asPWORD*)(property) = (asPWORD)class_inst;

	return true;
}

bool SceneScriptInst::PostPlay()
{
	class_inst = (asIScriptObject*)core.scripts.CreateScriptObject(Asset()->class_type);

	if (!class_inst)
	{
		return false;
	}

	core.scripts.RegisterClassInstance(GetScene()->GetName(), class_inst);

	int index = 0;

	for (auto node : Asset()->nodes)
	{
		if (node->type == SceneScriptAsset::ScriptProperty)
		{
			SceneScriptAsset::NodeScriptProperty* node_prop = (SceneScriptAsset::NodeScriptProperty*)node;

			bool injected = false;
			for (int i = 0; i < (int)class_inst->GetPropertyCount(); i++)
			{
				if (StringUtils::IsEqual(class_inst->GetPropertyName(i), node_prop->name.c_str()))
				{
					Node& node_inst = nodes[index];

					if (!node_inst.object)
					{
						node_inst.object = GetScene()->FindByUID(node_inst.object_uid, node_inst.object_child_uid, false);
					}
					
					if (node_inst.object)
					{
						auto type = core.scripts.GetTypeInfoById(class_inst->GetPropertyTypeId(i));
						
						if (!node_inst.object->InjectIntoScript(type->GetName(), class_inst->GetAddressOfProperty(i), node_prop->prefix.c_str()))
						{
							core.Log("ScriptErr", "Object %s of type %s can't be injected into %s of type %s", node_inst.object->GetName(), node_inst.object->script_class_name, class_inst->GetPropertyName(i), type->GetName());
						}

						injected = true;
					}

					break;
				}
			}

			if (!injected)
			{
				core.Log("ScriptErr", "Object for property %s was not set", node_prop->name.c_str());
			}
		}
		else
		if (node->type == SceneScriptAsset::ScriptMethod)
		{
			SceneScriptAsset::NodeScriptMethod* node_method = (SceneScriptAsset::NodeScriptMethod*)node;

			if (node_method->param_type != 3)
			{
				for (auto& link : node_method->links)
				{
					Node& node_inst = nodes[link.node];

					if (!node_inst.object)
					{
						node_inst.object = GetScene()->FindByUID(node_inst.object_uid, node_inst.object_child_uid, false);
					}

					if (node_inst.object && node_inst.object->script_callbacks.size() > 0)
					{
						ScriptCallback* callback = nullptr;

						if (node_inst.callback_type.size() > 0)
						{
							callback = node_inst.object->FindScriptCallback(node_inst.callback_type.c_str());
						}
						else
						{
							callback = &node_inst.object->script_callbacks[0];
						}

						if (callback)
						{
							if (node_method->param_type == 1)
							{
								callback->SetIntParam(atoi(link.param.c_str()));
							}
							else
							if (node_method->param_type == 2)
							{
								callback->SetStringParam(link.param);
							}

							if (!callback->Prepare(Asset()->class_type, class_inst, node_method->name.c_str()))
							{
								//return false;
							}
						}
					}
					else
					{
						core.Log("ScriptErr", "Callabck {%s} was not set", Asset()->nodes[link.node]->name.c_str());
					}
				}
			}
		}

		index++;
	}

	CallMethods(Asset()->on_start_init, false);

	return true;
}

void SceneScriptInst::Work(float dt)
{
	if (!GetScene()->Playing() || !class_inst)
	{
		return;
	}

	CallMethods(Asset()->frame_updates, true);
}

void SceneScriptInst::Release()
{
	if (Asset())
	{
		int index = 0;

		for (auto node : Asset()->nodes)
		{
			if (node->type == SceneScriptAsset::ScriptMethod)
			{
				SceneScriptAsset::NodeScriptMethod* node_method = (SceneScriptAsset::NodeScriptMethod*)node;

				if (node_method->param_type != 3)
				{
					for (auto link : node_method->links)
					{
						Node& node_inst = nodes[link.node];

						if (node_inst.object && node_inst.object->script_callbacks.size() > 0)
						{
							ScriptCallback* callback = nullptr;

							if (node_inst.callback_type.size() > 0)
							{
								callback = node_inst.object->FindScriptCallback(node_inst.callback_type.c_str());
							}
							else
							{
								callback = &node_inst.object->script_callbacks[0];
							}

							if (callback)
							{
								callback->Unbind(class_inst);
							}
						}
					}
				}
			}
		}

		index++;

		core.scripts.UnregisterClassInstance(class_inst);
		RELEASE(class_inst);
	}

	SceneObjectInst::Release();
}

bool SceneScriptInst::UsingCamera2DPos()
{
	return true;
}

void SceneScriptInst::CallMethods(vector<SceneScriptAsset::NodeScriptMethod*> methods, bool use_dt)
{
	for (auto* node : methods)
	{
		Script()->ctx->Prepare(node->method);

		if (use_dt)
		{
			Script()->ctx->SetArgFloat(0, core.GetDeltaTime());
		}

		Script()->ctx->SetObject(class_inst);

		if (Script()->ctx->Execute() < 0)
		{
			core.Log("ScriptErr", "Error occured in call of '%s'", node->name.c_str());
		}
	}
}

#ifdef EDITOR
void SceneScriptInst::SetScene(Scene* set_scene)
{
	SceneObjectInst::SetScene(set_scene);

	int index = 0;
	for (auto& node : Asset()->nodes)
	{
		if (node->type == SceneScriptAsset::NodeType::SceneCallback || node->type == SceneScriptAsset::NodeType::ScriptProperty)
		{
			Node& nd = nodes[index];

			if (!GetScene()->FindByUID(nd.object_uid, nd.object_child_uid, nd.object->IsAsset()))
			{
				nd = Node();
			}
		}

		index++;
	}
}

void SceneScriptInst::Copy(SceneObject* src)
{
	SceneObjectInst::Copy(src);
	nodes = ((SceneScriptInst*)src)->nodes;
}

void SceneScriptInst::EditorWork(float dt)
{
	Asset()->EditorWork(dt, this);
}

void SceneScriptInst::SetEditMode(bool ed)
{
	SceneObjectInst::SetEditMode(ed);

	if (ed)
	{
		Asset()->script_inst = this;
		Asset()->ShowProperties(true);
	}
	else
	{
		Asset()->ShowProperties(false);
		Asset()->script_inst = nullptr;
	}
}

void SceneScriptInst::OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms)
{
	if (!is_scene_tree)
	{
		return;
	}

	int index = 0;

	for (auto& node : Asset()->nodes)
	{
		if (node->pos.x - Sprite::ed_cam_pos.x < ms.x && ms.x < node->pos.x + Asset()->nodeSize.x - Sprite::ed_cam_pos.x &&
			node->pos.y - Sprite::ed_cam_pos.y < ms.y && ms.y < node->pos.y + Asset()->nodeSize.y - Sprite::ed_cam_pos.y)
		{
			if (node->type == SceneScriptAsset::NodeType::SceneCallback || node->type == SceneScriptAsset::NodeType::ScriptProperty)
			{
				Node& nd = nodes[index];

				uint32_t object_uid = 0;
				uint32_t object_child_uid = 0;

				object->GetUIDs(object_uid, object_child_uid);

				if (GetScene()->FindByUID(object_uid, object_child_uid, object->IsAsset()))
				{
					nd.object = object;
					nd.object_uid = object_uid;
					nd.object_child_uid = object_child_uid;

					if (object->script_callbacks.size() > 0)
					{
						nd.callback_type = object->script_callbacks[0].GetName();

						nd.GetMetaData()->Prepare(&nd);
						nd.GetMetaData()->PrepareWidgets(ed_obj_cat);
					}
					else
					{
						nd.callback_type = "";
					}
				}
			}

			break;
		}

		index++;
	}
}

void SceneScriptInst::OnMouseMove(Vector2 delta_ms)
{
	Asset()->OnMouseMove(delta_ms);
}

void SceneScriptInst::OnLeftMouseDown(Vector2 ms)
{
	Asset()->OnLeftMouseDown(ms);
}

void SceneScriptInst::OnLeftMouseUp()
{
	Asset()->OnLeftMouseUp();
}

void SceneScriptInst::OnRightMouseDown(Vector2 ms)
{
	Asset()->OnRightMouseDown(ms);
}

void SceneScriptInst::OnPopupMenuItem(int id)
{
	Asset()->OnPopupMenuItem(id);
}

void SceneScriptInst::ShowProperties(bool show)
{
	if (Asset()->sel_node != -1 && Asset()->sel_link == -1 && Asset()->nodes[Asset()->sel_node]->type == SceneScriptAsset::NodeType::SceneCallback)
	{
		if (show)
		{
			Node& node = nodes[Asset()->sel_node];
			node.GetMetaData()->Prepare(&node);
			node.GetMetaData()->PrepareWidgets(ed_obj_cat);
		}
		else
		{
			nodes[Asset()->sel_node].GetMetaData()->HideWidgets();
		}
	}
}
#endif