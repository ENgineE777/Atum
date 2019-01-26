#include "SceneScriptInst.h"
#include "Services/Core/Core.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"

#endif

#include "Services/Script/Libs/scriptarray.h"

CLASSREG(SceneObject, SceneScriptInst, "Script")

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
}

void SceneScriptInst::Node::Save(JSONWriter& saver)
{
	saver.Write("object_uid", object_uid);
	saver.Write("object_child_uid", object_child_uid);
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

bool SceneScriptInst::PostPlay()
{
	class_inst = (asIScriptObject*)core.scripts.engine->CreateScriptObject(Asset()->class_type);

	core.scripts.RegisterClassInstance(class_inst);

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
						node_inst.object = owner->FindByUID(node_inst.object_uid, node_inst.object_child_uid, false);
					}
					
					if (node_inst.object)
					{
						auto type = core.scripts.engine->GetTypeInfoById(class_inst->GetPropertyTypeId(i));
						
						if (!node_inst.object->InjectIntoScript(type->GetName(), class_inst->GetAddressOfProperty(i), node_prop->prefix.c_str()))
						{
							core.Log("ScriptErr", "Object %s of type %s can't be injected into %s of type %s", node_inst.object->GetName(), node_inst.object->script_class_name, class_inst->GetPropertyName(i), type->GetName());
							return false;
						}

						injected = true;
					}

					break;
				}
			}

			if (!injected)
			{
				core.Log("ScriptErr", "Object for property %s was not set", node_prop->name.c_str());
				return false;
			}
		}
		else
		if (node->type == SceneScriptAsset::ScriptMethod)
		{
			SceneScriptAsset::NodeScriptMethod* node_method = (SceneScriptAsset::NodeScriptMethod*)node;

			if (node_method->param_type != 3)
			{
				for (auto link : node_method->links)
				{
					Node& node_inst = nodes[link->node];

					if (!node_inst.object)
					{
						node_inst.object = owner->FindByUID(node_inst.object_uid, node_inst.object_child_uid, false);
					}

					if (node_inst.object && node_inst.object->script_callbacks.size() > 0)
					{
						if (node_method->param_type == 1)
						{
							node_inst.object->script_callbacks[0].SetIntParam(atoi(link->param.c_str()));
						}
						else
						if (node_method->param_type == 2)
						{
							node_inst.object->script_callbacks[0].SetStringParam(link->param);
						}

						if (!node_inst.object->script_callbacks[0].Prepare(Asset()->class_type, class_inst, node_method->name.c_str()))
						{
							return false;
						}
					}
					else
					{
						core.Log("ScriptErr", "Callabck {%s} was not set", Asset()->nodes[link->node]->name.c_str());
						return false;
					}
				}
			}
		}

		index++;
	}

	return true;
}

void SceneScriptInst::Work(float dt)
{
	if (!Playing())
	{
		return;
	}

	for (auto& node : Asset()->nodes)
	{
		if (node->type == SceneScriptAsset::ScriptMethod)
		{
			SceneScriptAsset::NodeScriptMethod* node_method = (SceneScriptAsset::NodeScriptMethod*)node;

			if (node_method->param_type == 3)
			{
				Script()->ctx->Prepare(node_method->method);
				Script()->ctx->SetArgFloat(0, core.GetDeltaTime());
				Script()->ctx->SetObject(class_inst);
				if (Script()->ctx->Execute() < 0)
				{
					core.Log("ScriptErr", "Error occured in call of '%s'", node_method->name.c_str());
				}
			}
		}
	}
}

void SceneScriptInst::Stop()
{
	core.scripts.UnregisterClassInstance(class_inst);
	RELEASE(class_inst);
}

bool SceneScriptInst::UsingCamera2DPos()
{
	return true;
}

Vector2& SceneScriptInst::Camera2DPos()
{
	if (asset)
	{
		asset->Camera2DPos();
	}

	return camera_pos;
}

#ifdef EDITOR

void SceneScriptInst::EditorWork(float dt)
{
	Asset()->EditorWork(dt, this);
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
			if (node->type == SceneScriptAsset::ScnCallback || node->type == SceneScriptAsset::ScriptProperty)
			{
				Node& nd = nodes[index];

				nd.object_uid = object->GetParentUID();
				nd.object_child_uid = object->GetUID();

				if (nd.object_uid == 0)
				{
					nd.object_uid = nd.object_child_uid;
					nd.object_child_uid = 0;
				}
				if (nd.object_uid == nd.object_child_uid)
				{
					nd.object_child_uid = 0;
				}

				nd.object = object;
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
#endif