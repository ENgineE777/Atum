#include "SceneScriptAsset.h"
#include "Services/Core/Core.h"
#include "SceneScriptInst.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"
#include "Editor/Editor.h"

#endif

#include "Services/Script/Libs/scriptarray.h"

CLASSREG(SceneAsset, SceneScriptAsset, "Script")

META_DATA_DESC(SceneScriptAsset::NodeSceneObject)
	STRING_PROP(SceneScriptAsset::NodeSceneObject, name, "", "Property", "Name")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::NodeScriptConst)
	STRING_PROP(SceneScriptAsset::NodeScriptConst, name, "", "Property", "Name")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::NodeScriptProperty)
	STRING_PROP(SceneScriptAsset::NodeScriptProperty, name, "", "Property", "Name")
	STRING_PROP(SceneScriptAsset::NodeScriptProperty, prefix, "", "Property", "Prefix")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::LinkToMethod)
	STRING_PROP(SceneScriptAsset::LinkToMethod, param, "", "Property", "Param")
	STRING_PROP(SceneScriptAsset::LinkToMethod, param2, "", "Property", "Param2")
	STRING_PROP(SceneScriptAsset::LinkToMethod, param3, "", "Property", "Param3")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::NodeScriptMethod)
	STRING_PROP(SceneScriptAsset::NodeScriptMethod, name, "", "Property", "Name")
	ENUM_PROP(SceneScriptAsset::NodeScriptMethod, param_type, 0, "Property", "ParamType", "Type of parameter which will be passed in method during call")
		ENUM_ELEM("None", 0)
		ENUM_ELEM("Int", 1)
		ENUM_ELEM("String", 2)
		ENUM_ELEM("Int2", 3)
		ENUM_ELEM("Int3", 4)
	ENUM_END
	ENUM_PROP(SceneScriptAsset::NodeScriptMethod, call_type, 0, "Property", "CallType", "Type of method call")
		ENUM_ELEM("OnCallback", 0)
		ENUM_ELEM("OnInit", 1)
		ENUM_ELEM("EveryFrame", 2)
	ENUM_END
	INT_PROP(SceneScriptAsset::NodeScriptMethod, priority, 0, "Property", "Priority", "Priority of execution")
META_DATA_DESC_END()

#ifdef EDITOR
void CenterScriptCamera(void* owner)
{
	SceneScriptAsset* script = (SceneScriptAsset*)owner;

	if (script->nodes.size() > 0)
	{
		Vector2 leftCorner = FLT_MAX;
		Vector2 rightCorner = -FLT_MAX;

		for (auto& node : script->nodes)
		{
			leftCorner.Min(node->pos - script->nodeSize * 0.5f);
			rightCorner.Max(node->pos + script->nodeSize * 1.5f);
		}

		Vector2 half_size = (rightCorner - leftCorner) * 0.5f;
		Sprite::ed_cam_pos = leftCorner + half_size;
		Sprite::ed_cam_zoom = fmin(0.5f * core.render.GetDevice()->GetWidth() / half_size.x, 0.5f * core.render.GetDevice()->GetHeight() / half_size.y);
	}
	else
	{
		Sprite::ed_cam_pos = 0.0f;
	}

}
#endif

META_DATA_DESC(SceneScriptAsset)
	BASE_SCENE_OBJ_PROP(SceneScriptAsset)
	STRING_PROP(SceneScriptAsset, class_name, "", "Prop", "main_class")
	STRING_PROP(SceneScriptAsset, class_namespace, "", "Prop", "class_namespace")
#ifdef EDITOR
	CALLBACK_PROP(SceneScriptAsset, CenterScriptCamera, "Prop", "CenterCamera")
#endif
META_DATA_DESC_END()

void SceneScriptAsset::Node::Load(JSONReader& loader)
{
	loader.Read("name", name);
	loader.Read("pos", pos);
}

void SceneScriptAsset::Node::Save(JSONWriter& saver)
{
	saver.Write("type", (int&)type);
	saver.Write("name", name.c_str());
	saver.Write("pos", pos);
}

void SceneScriptAsset::NodeScriptProperty::Load(JSONReader& loader)
{
	NodeSceneObject::Load(loader);

	loader.Read("prefix", prefix);
}

void SceneScriptAsset::NodeScriptProperty::Save(JSONWriter& saver)
{
	NodeSceneObject::Save(saver);

	saver.Write("prefix", prefix.c_str());
}

void SceneScriptAsset::NodeScriptMethod::Load(JSONReader& loader)
{
	Node::Load(loader);

	loader.Read("param_type", (int&)param_type);
	loader.Read("call_type", (int&)call_type);
	loader.Read("priority", priority);

	int link_count = 0;
	loader.Read("Count", link_count);
	links.resize(link_count);

	for (auto& link : links)
	{
		loader.EnterBlock("Link");

		loader.Read("node", link.node);
		loader.Read("param", link.param);
		loader.Read("param2", link.param2);
		loader.Read("param3", link.param3);

		loader.LeaveBlock();
	}
}

void SceneScriptAsset::NodeScriptMethod::Save(JSONWriter& saver)
{
	Node::Save(saver);

	saver.Write("param_type", param_type);
	saver.Write("call_type", call_type);
	saver.Write("priority", priority);

	int link_count = (int)links.size();
	saver.Write("Count", link_count);

	saver.StartArray("Link");

	for (auto& link : links)
	{
		saver.StartBlock(nullptr);

		saver.Write("node", link.node);
		saver.Write("param", link.param.c_str());
		saver.Write("param2", link.param2.c_str());
		saver.Write("param3", link.param3.c_str());

		saver.FinishBlock();
	}

	saver.FinishArray();
}

void SceneScriptAsset::Init()
{
	inst_class_name = "SceneScriptInst";

#ifdef EDITOR
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SceneScriptAsset::EditorWork);
#endif

	GetScene()->AddToGroup(this, "AssetScripts");
}

void SceneScriptAsset::Load(JSONReader& loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);

	int count = 0;
	loader.Read("Count", count);
	nodes.resize(count);

	int type = 0;

	for (auto& node : nodes)
	{
		loader.EnterBlock("Node");

		loader.Read("type", type);

		if (type == NodeType::SceneCallback)
		{
			node = new NodeSceneObject();
		}

		if (type == NodeType::ScriptConst)
		{
			node = new NodeScriptConst();
		}

		if (type == NodeType::ScriptProperty)
		{
			node = new NodeScriptProperty();
		}

		if (type == NodeType::ScriptMethod)
		{
			node = new NodeScriptMethod();
		}

		node->type = (NodeType)type;
		node->Load(loader);

		loader.LeaveBlock();
	}

	loader.Read("dep_count", count);
	dependency.resize(count);

	for (auto& dep : dependency)
	{
		loader.EnterBlock("Dep");

		loader.Read("name", dep);
		loader.LeaveBlock();
	}
}

void SceneScriptAsset::Save(JSONWriter& saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);

	int count = (int)nodes.size();
	saver.Write("Count", count);

	saver.StartArray("Node");

	for (auto node : nodes)
	{
		saver.StartBlock(nullptr);

		node->Save(saver);

		saver.FinishBlock();
	}

	saver.FinishArray();

	count = (int)dependency.size();
	saver.Write("dep_count", count);

	saver.StartArray("Dep");

	for (auto& dep : dependency)
	{
		saver.StartBlock(nullptr);

		saver.Write("name", dep);

		saver.FinishBlock();
	}

	saver.FinishArray();
}

bool SceneScriptAsset::Play()
{
	if (played)
	{
		return true;
	}

	played = true;

	/*if (!CompileScript())
	{
		return false;
	}*/

	asIScriptModule* mod = core.scripts.mod;

	if (!mod)
	{
		return false;
	}

	if (class_name.c_str()[0])
	{
		for (uint32_t i = 0; i < mod->GetObjectTypeCount(); i++)
		{
			asITypeInfo* tp = mod->GetObjectTypeByIndex(i);

			if (StringUtils::IsEqual(class_name.c_str(), tp->GetName()) && StringUtils::IsEqual(class_namespace.c_str(), tp->GetNamespace()))
			{
				class_type = tp;
				break;
			}
		}

		if (!class_type)
		{
			core.Log("ScriptErr", "Class %s::%s not found", class_namespace.c_str(), class_name.c_str());
			core.scene_manager.failure_on_scene_play_message += string(GetName()) + ": Class " + class_namespace + "::" + class_name + "not found";
			return false;
		}
	}
	else
	{
		class_type = mod->GetObjectTypeByIndex(0);
	}

	if (class_type)
	{
		for (auto node : nodes)
		{
			if (node->type == NodeType::ScriptMethod)
			{
				NodeScriptMethod* node_method = (NodeScriptMethod*)node;

				if (node_method->call_type == CallType::OnInit || node_method->call_type == CallType::EveryFrame)
				{
					char prototype[256];

					if (node_method->call_type == CallType::OnInit)
					{
						StringUtils::Printf(prototype, 256, "void %s()", node_method->name.c_str());
						on_start_init.push_back(node_method);
					}
					else
					{
						StringUtils::Printf(prototype, 256, "void %s(float)", node_method->name.c_str());
						frame_updates.push_back(node_method);
					}

					node_method->method = class_type->GetMethodByDecl(prototype);

					if (!node_method->method)
					{
						core.Log("ScriptErr", "Method %s not found", node_method->name.c_str());
					}
				}
			}
		}
	}

	return true;
}

void SceneScriptAsset::Release()
{
	for (auto node : nodes)
	{
		delete node;
	}
}

bool SceneScriptAsset::UsingOwnCamera()
{
	return true;
}

#ifdef EDITOR
SceneObject* SceneScriptAsset::CreateInstance(Scene* scene)
{
	SceneScriptInst* inst = (SceneScriptInst*)SceneAsset::CreateInstance(scene);

	inst->nodes.resize(nodes.size());

	return inst;
}

void SceneScriptAsset::EditorWork(float dt)
{
	EditorWork(dt, nullptr);
}

void SceneScriptAsset::EditorWork(float dt, SceneScriptInst* inst)
{
	int index = 0;

	index = 0;
	for (auto node : nodes)
	{
		if (node->type == NodeType::ScriptMethod)
		{
			int link_index = 0;
			for (auto& link : ((NodeScriptMethod*)node)->links)
			{
				Vector2 p1 = nodes[link.node]->pos + Vector2(nodeSize.x, 37.0f);
				Vector2 p2 = node->pos + Vector2(0.0f, 37.0f);

				editor_drawer.DrawCurve(p1, p2, (index == sel_node && link_index == sel_link) ? COLOR_GREEN : COLOR_WHITE);

				link.arrow_pos = nodes[link.node]->pos + Vector2(nodeSize.x - 15.0f, 30.0f);
				link_index++;
			}
		}

		index++;
	}

	index = 0;

	for (auto& node : nodes)
	{
		Color color = COLOR_CYAN;
		if ((sel_node == index || target_link == index) && sel_link == -1)
		{
			color = COLOR_GREEN;
		}

		editor_drawer.DrawSprite(editor_drawer.node_tex, node->pos, nodeSize, 0.0f, 0.0f, color);
		editor_drawer.PrintText(node->pos + Vector2(5.0f + (node->type == ScriptMethod ? 15.0f : 0.0f), 30.0f), COLOR_WHITE, node->name.c_str());

		if (node->type == NodeType::SceneCallback)
		{
			Color color = (sel_link != -1 && ((NodeScriptMethod*)nodes[sel_node])->links[sel_link].node == index) ? COLOR_GREEN : COLOR_WHITE;

			if (link_drag && sel_node == index)
			{
				color = COLOR_GREEN;
			}

			editor_drawer.DrawSprite(editor_drawer.arrow_tex, node->pos + Vector2(nodeSize.x - 15.0f, 30.0f), linkSize, 0.0f, 0.0f, color);
		}
		else
		if (node->type == ScriptMethod)
		{
			Color color = (link_drag && target_link == index) ? COLOR_GREEN : COLOR_WHITE;
			editor_drawer.DrawSprite(editor_drawer.arrow_tex, node->pos + Vector2(5.0f, 30.0f), linkSize, 0.0f, 0.0f, color);
		}

		const char* names[] = {"Callback", "Property", "Method", "Const"};
		editor_drawer.PrintText(node->pos + Vector2(5.0f, 3.0f), COLOR_WHITE, names[node->type]);

		if (inst)
		{
			if (node->type == NodeType::SceneCallback || node->type == NodeType::ScriptProperty)
			{
				SceneScriptInst::Node& scene_node = inst->nodes[index];

				SceneObjectRef& ref = scene_node.objects[0].ref;

				ref.object = inst->GetScene()->FindByUID(ref.uid, ref.child_uid, ref.is_asset);

				editor_drawer.PrintText(node->pos + Vector2(5.0f, 50.0f), COLOR_WHITE, ref.object ? ref.object->GetName() : "NULL");
			}
			else
			if (node->type == NodeType::ScriptConst)
			{
				editor_drawer.PrintText(node->pos + Vector2(5.0f, 50.0f), COLOR_WHITE, inst->nodes[index].callback_type.c_str());
			}
		}

		index++;
	}

	if (sel_node != -1 && link_drag)
	{
		editor_drawer.DrawCurve(nodes[sel_node]->pos + Vector2(nodeSize.x, 37.0f), ms_pos, COLOR_WHITE);
	}
}

void SceneScriptAsset::OnMouseMove(Vector2 delta_ms)
{
	delta_ms /= Sprite::ed_cam_zoom;
	ms_pos += delta_ms;

	if (in_drag)
	{
		if (sel_node != -1)
		{
			nodes[sel_node]->pos += delta_ms;
		}
	}

	if (link_drag)
	{
		target_link = -1;
		int index = 0;
		for (auto& node : nodes)
		{
			if (node->type != ScriptMethod)
			{
				index++;
				continue;
			}

			if (node->pos.x < ms_pos.x && ms_pos.x < node->pos.x + 15.0f &&
				node->pos.y + 30.0f < ms_pos.y && ms_pos.y < node->pos.y + 45.0f)
			{
				target_link = index;
			}

			index++;
		}
	}
}

void SceneScriptAsset::OnLeftMouseDown(Vector2 ms)
{
	ms = Sprite::MoveFromCamera(ms, false);

	ShowProperties(false);

	sel_node = -1;
	sel_link = -1;
	link_drag = false;
	in_drag = false;

	int index = 0;
	for (auto& node : nodes)
	{
		if (node->pos.x < ms.x && ms.x < node->pos.x + nodeSize.x &&
			node->pos.y < ms.y && ms.y < node->pos.y + nodeSize.y)
		{
			sel_node = index;
			sel_link = -1;
		}

		index++;
	}

	index = 0;
	for (auto& node : nodes)
	{
		if (node->type == ScriptMethod)
		{
			int link_index = 0;

			for (auto& link : ((NodeScriptMethod*)node)->links)
			{
				if (link.arrow_pos.x < ms.x && ms.x < link.arrow_pos.x + linkSize.x &&
					link.arrow_pos.y < ms.y && ms.y < link.arrow_pos.y + linkSize.y)
				{
					sel_node = index;
					sel_link = link_index;
				}

				link_index++;
			}
		}

		index++;
	}

	if (sel_node != -1 && sel_link == -1)
	{
		in_drag = true;

		Node* node = nodes[sel_node];

		if (node->type == NodeType::SceneCallback)
		{
			if (node->pos.x + nodeSize.x - 15.0f < ms.x && ms.x < node->pos.x + nodeSize.x &&
				node->pos.y + 30.0f < ms.y && ms.y < node->pos.y + 45.0f)
			{
				link_drag = true;
				in_drag = false;
			}
		}

		ms_pos = ms;
	}

	ShowProperties(true);
}

void SceneScriptAsset::OnLeftMouseUp()
{
	if (link_drag && target_link != -1)
	{
		if ((nodes[sel_node]->type == NodeType::SceneCallback && nodes[target_link]->type == NodeType::ScriptMethod))
		{
			int tmp = sel_node;
			sel_node = target_link;
			target_link = tmp;
		}

		Node* node = nodes[sel_node];
		Node* traget = nodes[target_link];

		if (node->type == NodeType::ScriptMethod && traget->type == NodeType::SceneCallback)
		{
			NodeScriptMethod* method_node = (NodeScriptMethod*)node;

			bool allow_add = true;

			for (auto& link : method_node->links)
			{
				if (link.node == target_link)
				{
					allow_add = false;
					break;
				}
			}

			if (allow_add)
			{
				LinkToMethod link;

				link.node = target_link;

				method_node->links.push_back(link);
			}
		}
	}

	in_drag = false;
	link_drag = false;
	target_link = -1;
}

void SceneScriptAsset::OnRightMouseDown(Vector2 ms)
{
	ms_pos = ms;

	editor.popup_menu->StartMenu(true);

	editor.popup_menu->StartSubMenu("Create Link to");
	editor.popup_menu->AddItem(5004, "Const");
	editor.popup_menu->AddItem(5002, "Property");
	editor.popup_menu->AddItem(5001, "Callback");
	editor.popup_menu->AddItem(5003, "Method");
	editor.popup_menu->EndSubMenu();

	editor.popup_menu->AddItem(5005, "Delete", (sel_node != -1));

	editor.popup_menu->ShowAsPopup(editor.viewport, (int)ms.x, (int)ms.y);
}

void SceneScriptAsset::OnPopupMenuItem(int id)
{
	Node* node = nullptr;

	if (id == 5001)
	{
		NodeSceneObject* scene_node = new NodeSceneObject();
		scene_node->type = NodeType::SceneCallback;
		scene_node->name = "Object";
		node = scene_node;
	}

	if (id == 5002)
	{
		NodeScriptProperty* prop_node = new NodeScriptProperty();
		prop_node->type = NodeType::ScriptProperty;
		prop_node->name = "property";
		node = prop_node;
	}

	if (id == 5003)
	{
		NodeScriptMethod* method_node = new NodeScriptMethod();
		method_node->type = NodeType::ScriptMethod;
		method_node->name = "method";
		node = method_node;
	}

	if (id == 5004)
	{
		NodeScriptConst* method_node = new NodeScriptConst();
		method_node->type = NodeType::ScriptConst;
		method_node->name = "const";
		node = method_node;
	}

	if (node)
	{
		node->pos = Sprite::MoveFromCamera(ms_pos, false);
		nodes.push_back(node);

		for (auto& inst : instances)
		{
			((SceneScriptInst*)inst.GetObject())->nodes.push_back(SceneScriptInst::Node());
		}
	}

	if (id == 5005)
	{
		if (sel_link == -1)
		{
			Node* node = nodes[sel_node];

			for (auto& nd : nodes)
			{
				if (nd->type == NodeType::ScriptMethod)
				{
					NodeScriptMethod* node_method = (NodeScriptMethod*)nd;

					for (int i = 0; i < node_method->links.size(); i++)
					{
						auto& link = node_method->links[i];

						if (link.node == sel_node)
						{
							node_method->links.erase(node_method->links.begin() + i);
							i--;
						}
						else
						if (link.node > sel_node)
						{
							link.node--;
						}
					}
				}
			}

			int sel_nd = sel_node;
			ShowProperties(false);

			delete node;
			nodes.erase(nodes.begin() + sel_nd);

			for (auto& inst : instances)
			{
				SceneScriptInst* script_inst = (SceneScriptInst*)inst.GetObject();
				script_inst->nodes.erase(script_inst->nodes.begin() + sel_nd);
			}
		}
		else
		{
			NodeScriptMethod* node_method = (NodeScriptMethod*)nodes[sel_node];

			int link = sel_link;
			ShowProperties(false);

			node_method->links.erase(node_method->links.begin() + link);
		}
	}
}

void SceneScriptAsset::Copy(SceneObject* src)
{
	SceneAsset::Copy(src);

	int type = 0;

	SceneScriptAsset* src_asset = (SceneScriptAsset*)src;

	nodes.resize(src_asset->nodes.size());

	for (int i = 0; i < nodes.size(); i++)
	{
		Node* node = nullptr;

		switch (src_asset->nodes[i]->type)
		{
			case NodeType::SceneCallback:
			{
				node = new NodeSceneObject();
				*((NodeSceneObject*)node) = *((NodeSceneObject*)src_asset->nodes[i]);
				break;
			}
			case NodeType::ScriptConst:
			{
				node = new NodeScriptConst();
				*((NodeScriptConst*)node) = *((NodeScriptConst*)src_asset->nodes[i]);
				break;
			}
			case NodeType::ScriptProperty:
			{
				node = new NodeScriptProperty();
				*((NodeScriptProperty*)node) = *((NodeScriptProperty*)src_asset->nodes[i]);
				break;
			}
			case NodeType::ScriptMethod:
			{
				node = new NodeScriptMethod();
				*((NodeScriptMethod*)node) = *((NodeScriptMethod*)src_asset->nodes[i]);
				break;
			}
		}

		nodes[i] = node;
	}
}

void SceneScriptAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);

	ShowProperties(ed);
}

void SceneScriptAsset::ShowProperties(bool show)
{
	if (show)
	{
		if (sel_node != -1 && sel_link != -1)
		{
			LinkToMethod& link = ((NodeScriptMethod*)nodes[sel_node])->links[sel_link];
			if (link.GetMetaData())
			{
				link.GetMetaData()->Prepare(&link);
				link.GetMetaData()->PrepareWidgets(editor.obj_cat);
			}
		}
		else
		if (sel_node != -1)
		{
			Node* node = nodes[sel_node];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(editor.obj_cat);
		}
	}
	else
	{
		if (sel_node != -1 && sel_link != -1)
		{
			LinkToMethod& link = ((NodeScriptMethod*)nodes[sel_node])->links[sel_link];
			if (link.GetMetaData())
			{
				link.GetMetaData()->HideWidgets();
			}
		}
		else
		if (sel_node != -1)
		{
			nodes[sel_node]->GetMetaData()->HideWidgets();
		}
	}

	if (script_inst)
	{
		script_inst->ShowProperties(show);
	}

	if (!show)
	{
		sel_node = -1;
		sel_link = -1;
	}
}
#endif