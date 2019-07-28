#include "SceneScriptInst.h"
#include "Services/Core/Core.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"

#endif

#include "Services/Script/Libs/scriptarray.h"

#ifdef EDITOR

void FillCallbackList(EUIComboBox* cbox, void* object)
{
	SceneScriptInst::Node* node = (SceneScriptInst::Node*)object;

	if (node->objects[0].ref.object)
	{
		for (auto& callback : node->objects[0].ref.object->script_callbacks)
		{
			cbox->AddItem(callback.GetName());
		}
	}
}

#endif

META_DATA_DESC(SceneScriptInst::WrapperSceneObjectRef)
SCENEOBJECT_PROP(SceneScriptInst::WrapperSceneObjectRef, ref, "Property", "object")
META_DATA_DESC_END()

void SceneScriptInst::WrapperSceneObjectRef::Load(JSONReader& reader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(reader);
}

void SceneScriptInst::WrapperSceneObjectRef::Save(JSONWriter& writer)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(writer);
}

CLASSREG(SceneObject, SceneScriptInst, "Script")

META_DATA_DESC(SceneScriptInst::Node)
#ifdef EDITOR
STRING_ENUM_PROP(SceneScriptInst::Node, callback_type, FillCallbackList, "Property", "callback_type")
#else
STRING_ENUM_PROP(SceneScriptInst::Node, callback_type, "Property", "callback_type")
#endif
ARRAY_PROP(SceneScriptInst::Node, objects, WrapperSceneObjectRef, "Property", "objects_list")
META_DATA_DESC_END()

#ifdef EDITOR
void StartScriptInstEdit(void* owner)
{
	SceneScriptInst* script = (SceneScriptInst*)owner;

	string filename;
	script->Asset()->GetScriptFileName(filename, false);

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
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);

	uint32_t object_uid = 0;
	uint32_t object_child_uid = 0;

	if (loader.Read("object_uid", object_uid))
	{
		loader.Read("object_child_uid", object_child_uid);

		WrapperSceneObjectRef& ref = objects[0];

		ref.ref.uid = object_uid;
		ref.ref.child_uid = object_child_uid;
		ref.ref.is_asset = false;
	}
}

void SceneScriptInst::Node::Save(JSONWriter& saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);
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

	int index = 0;

	for (auto& node : nodes)
	{
		loader.EnterBlock("Node");

		loader.Read("type", type);

		node.Load(loader);

		loader.LeaveBlock();

		index++;
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

bool SceneScriptInst::Play()
{
	Asset()->Play();

	class_inst = (asIScriptObject*)core.scripts.CreateScriptObject(Asset()->class_type);

	if (!class_inst)
	{
		return false;
	}

	core.scripts.RegisterClassInstance(GetScene(), class_inst);

	GetScene()->AddPostPlay(0, this, (Object::DelegateSimple)&SceneScriptInst::InjectIntoScript);

	if (Asset()->on_start_init.size() > 0)
	{
		GetScene()->AddPostPlay(10, this, (Object::DelegateSimple)&SceneScriptInst::CallOnInitScript);
	}

	return true;
}

void SceneScriptInst::InjectIntoScript()
{
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

					SceneObjectRef& ref = node_inst.objects[0].ref;

					if (!ref.object)
					{
						ref.object = GetScene()->FindByUID(ref.uid, ref.child_uid, false);
					}

					if (ref.object)
					{
						auto type = core.scripts.GetTypeInfoById(class_inst->GetPropertyTypeId(i));
						
						if (!ref.object->InjectIntoScript(type->GetName(), class_inst->GetAddressOfProperty(i), node_prop->prefix.c_str()))
						{
							core.Log("ScriptErr", "Object %s of type %s can't be injected into %s of type %s", ref.object->GetName(), ref.object->script_class_name, class_inst->GetPropertyName(i), type->GetName());
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

					for (auto& ref : node_inst.objects)
					{
						if (!ref.ref.object)
						{
							ref.ref.object = GetScene()->FindByUID(ref.ref.uid, ref.ref.child_uid, false);
						}

						if (ref.ref.object && ref.ref.object->script_callbacks.size() > 0)
						{
							ScriptCallback* callback = nullptr;

							if (node_inst.callback_type.size() > 0)
							{
								callback = ref.ref.object->FindScriptCallback(node_inst.callback_type.c_str());
							}
							else
							{
								callback = &ref.ref.object->script_callbacks[0];
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
		}
		index++;
	}
}

void SceneScriptInst::CallOnInitScript()
{
	CallMethods(Asset()->on_start_init, false);
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

						/*for (auto& ref : node_inst.objects)
						{
							if (ref.ref.object && ref.ref.object->script_callbacks.size() > 0)
							{
								ScriptCallback* callback = nullptr;

								if (node_inst.callback_type.size() > 0)
								{
									callback = ref.ref.object->FindScriptCallback(node_inst.callback_type.c_str());
								}
								else
								{
									callback = &ref.ref.object->script_callbacks[0];
								}

								if (callback)
								{
									callback->Unbind(class_inst);
								}
							}
						}*/
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
		core.scripts.script_caller = GetScene();

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
			Node& node = nodes[index];

			for (auto& ref : node.objects)
			{
				if (!GetScene()->FindByUID(ref.ref.uid, ref.ref.child_uid, ref.ref.is_asset))
				{
					ref.ref.uid = 0;
					ref.ref.child_uid = 0;
				}
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
				Node& node = nodes[index];

				uint32_t object_uid = 0;
				uint32_t object_child_uid = 0;

				object->GetUIDs(object_uid, object_child_uid);

				if (GetScene()->FindByUID(object_uid, object_child_uid, object->IsAsset()))
				{
					WrapperSceneObjectRef& ref = node.objects[0];

					ref.ref.object = object;
					ref.ref.uid = object_uid;
					ref.ref.child_uid = object_child_uid;
					ref.ref.is_asset = object->IsAsset();

					if (object->script_callbacks.size() > 0)
					{
						node.callback_type = object->script_callbacks[0].GetName();

						node.GetMetaData()->Prepare(&node);
						node.GetMetaData()->PrepareWidgets(ed_obj_cat);
					}
					else
					{
						node.callback_type = "";
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