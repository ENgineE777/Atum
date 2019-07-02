#include "SceneScriptAsset.h"
#include "Services/Core/Core.h"
#include "SceneScriptInst.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"

#endif

#include "Services/Script/Libs/scriptarray.h"

CLASSREG(SceneAsset, SceneScriptAsset, "Script")

META_DATA_DESC(SceneScriptAsset::NodeSceneObject)
STRING_PROP(SceneScriptAsset::NodeSceneObject, name, "", "Property", "Name")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::NodeScriptProperty)
STRING_PROP(SceneScriptAsset::NodeScriptProperty, name, "", "Property", "Name")
STRING_PROP(SceneScriptAsset::NodeScriptProperty, prefix, "", "Property", "Prefix")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::LinkToMethod)
STRING_PROP(SceneScriptAsset::LinkToMethod, param, "", "Property", "Params")
META_DATA_DESC_END()

META_DATA_DESC(SceneScriptAsset::NodeScriptMethod)
STRING_PROP(SceneScriptAsset::NodeScriptMethod, name, "", "Property", "Name")
ENUM_PROP(SceneScriptAsset::NodeScriptMethod, param_type, 0, "Property", "ParamType", "Type of parameter which will be passed in method during call")
	ENUM_ELEM("None", 0)
	ENUM_ELEM("Int", 1)
	ENUM_ELEM("String", 2)
ENUM_END
ENUM_PROP(SceneScriptAsset::NodeScriptMethod, call_type, 0, "Property", "CallType", "Type of method call")
	ENUM_ELEM("OnCallback", 0)
	ENUM_ELEM("OnInit", 1)
	ENUM_ELEM("EveryFrame", 2)
ENUM_END
META_DATA_DESC_END()

#ifdef EDITOR
void StartScriptEdit(void* owner)
{
	SceneScriptAsset* script = (SceneScriptAsset*)owner;

	string filename;
	script->GetScriptFileName(filename);

	ShellExecuteA(nullptr, "open", filename.c_str(), NULL, NULL, SW_SHOW);
}
#endif

META_DATA_DESC(SceneScriptAsset)
BASE_SCENE_OBJ_PROP(SceneScriptAsset)
STRING_PROP(SceneScriptAsset, main_class, "", "Prop", "main_class")
#ifdef EDITOR
CALLBACK_PROP(SpriteAsset, StartScriptEdit, "Prop", "EditScript")
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

void SceneScriptAsset::NodeSceneObject::Load(JSONReader& loader)
{
	Node::Load(loader);
}

void SceneScriptAsset::NodeSceneObject::Save(JSONWriter& saver)
{
	Node::Save(saver);
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

	int link_count = 0;
	loader.Read("Count", link_count);
	links.resize(link_count);

	for (auto& link : links)
	{
		loader.EnterBlock("Link");

		loader.Read("node", link.node);
		loader.Read("param", link.param);

		loader.LeaveBlock();
	}
}

void SceneScriptAsset::NodeScriptMethod::Save(JSONWriter& saver)
{
	Node::Save(saver);

	saver.Write("param_type", param_type);
	saver.Write("call_type", call_type);

	int link_count = (int)links.size();
	saver.Write("Count", link_count);

	saver.StartArray("Link");

	for (auto& link : links)
	{
		saver.StartBlock(nullptr);

		saver.Write("node", link.node);
		saver.Write("param", link.param.c_str());

		saver.FinishBlock();
	}

	saver.FinishArray();
}

void SceneScriptAsset::GetScriptFileName(string& filename)
{
	char str[1024];
	StringUtils::Printf(str, 1024, "%s%s.sns", GetScene()->GetPath(), GetName());

	filename = str;
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

#ifdef EDITOR
	GetScriptFileName(prev_filename);
#endif
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
}

void SceneScriptAsset::SetName(const char* set_name)
{
	SceneObject::SetName(set_name);

#ifdef EDITOR
	if (set_name[0])
	{
		RenameScriptFile();
	}
#endif
}

bool SceneScriptAsset::Play()
{
	if (played)
	{
		return true;
	}

	played = true;

	string filename;
	GetScriptFileName(filename);

	FileInMemory file;

	if (!file.Load(filename.c_str()))
	{
		return false;
	}

	mod = core.scripts.GetModule(filename.c_str(), asGM_ALWAYS_CREATE);

	string src = (const char*)file.GetData();

	const char* incl = strstr(src.c_str(), "#import \"");

	string external_decl;

	while (incl)
	{
		int64_t index_from = incl - src.c_str();
		int64_t index_to = index_from + 9;

		while (src[index_to] != '"' || src[index_to] == 0)
		{
			index_to++;
		}

		if (src[index_to] == 0)
		{
			break;
		}

		char name[256];
		int64_t len = index_to - index_from - 9;
		memcpy(name, &src.c_str()[index_from + 9], index_to - index_from - 9);
		name[len] = 0;

		SceneScriptAsset* object = (SceneScriptAsset*)GetScene()->FindInGroup("AssetScripts", name);

		if (object)
		{
			object->Play();

			if (object->mod)
			{
				for (uint32_t i = 0; i < object->mod->GetObjectTypeCount(); i++)
				{
					asITypeInfo* tp = object->mod->GetObjectTypeByIndex(i);
					external_decl += string("external shared class ") + tp->GetName() + ";\r\n";
				}
			}
		}

		src.erase(index_from, index_to - index_from + 1);

		incl = strstr(src.c_str(), "#import \"");
	}

	StringUtils::Replace(src, "class ", "shared class ");

	if (external_decl.size() > 0)
	{
		src = external_decl + src;
	}

	mod->AddScriptSection(GetName(), src.c_str(), src.size());

	mod->Build();

	if (main_class.c_str()[0])
	{
		for (uint32_t i = 0; i < mod->GetObjectTypeCount(); i++)
		{
			asITypeInfo* tp = mod->GetObjectTypeByIndex(i);

			if (StringUtils::IsEqual(main_class.c_str(), tp->GetName()))
			{
				class_type = tp;
				break;
			}
		}

		if (!class_type)
		{
			core.Log("ScriptErr", "Class %s not found", main_class.c_str());
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

	if (mod)
	{
		mod->Discard();
		mod = nullptr;
	}
}

bool SceneScriptAsset::UsingCamera2DPos()
{
	return true;
}

#ifdef EDITOR
void SceneScriptAsset::RenameScriptFile()
{
	bool need_rename = false;

	FILE* prev_fl = fopen(prev_filename.c_str(), "a");
	if (prev_fl)
	{
		need_rename = true;
		fclose(prev_fl);
	}

	string filename;
	GetScriptFileName(filename);

	if (need_rename)
	{
		rename(prev_filename.c_str(), filename.c_str());
	}
	else
	{
		FILE* fl = fopen(filename.c_str(), "a");
		fclose(fl);
	}

	prev_filename = filename;
}

void SceneScriptAsset::SetUID(uint32_t set_uid)
{
	SceneAsset::SetUID(set_uid);

	RenameScriptFile();
}

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
				Vector2 p1 = nodes[link.node]->pos + Vector2(nodeSize.x, 37.0f) - Sprite::ed_cam_pos;
				Vector2 p2 = node->pos + Vector2(0.0f, 37.0f) - Sprite::ed_cam_pos;

				editor_drawer.DrawCurve(p1, p2, (index == sel_node && link_index == sel_link) ? COLOR_GREEN : COLOR_WHITE);

				link.arrow_pos = nodes[link.node]->pos + Vector2(nodeSize.x - 15.0f, 30.0f) - Sprite::ed_cam_pos;
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

		editor_drawer.DrawSprite(editor_drawer.node_tex, node->pos - Sprite::ed_cam_pos, nodeSize, 0.0f, 0.0f, color);
		editor_drawer.PrintText(node->pos + Vector2(5.0f + (node->type == ScriptMethod ? 15.0f : 0.0f), 30.0f) - Sprite::ed_cam_pos, COLOR_WHITE, node->name.c_str());

		if (node->type == NodeType::SceneCallback)
		{
			Color color = (sel_link != -1 && ((NodeScriptMethod*)nodes[sel_node])->links[sel_link].node == index) ? COLOR_GREEN : COLOR_WHITE;

			if (link_drag && sel_node == index)
			{
				color = COLOR_GREEN;
			}

			editor_drawer.DrawSprite(editor_drawer.arrow_tex, node->pos + Vector2(nodeSize.x - 15.0f, 30.0f) - Sprite::ed_cam_pos, linkSize, 0.0f, 0.0f, color);
		}
		else
		if (node->type == ScriptMethod)
		{
			Color color = (link_drag && target_link == index) ? COLOR_GREEN : COLOR_WHITE;
			editor_drawer.DrawSprite(editor_drawer.arrow_tex, node->pos + Vector2(5.0f, 30.0f) - Sprite::ed_cam_pos, linkSize, 0.0f, 0.0f, color);
		}

		const char* names[] = {"Callback", "Property", "Method"};
		editor_drawer.PrintText(node->pos + Vector2(5.0f, 3.0f) - Sprite::ed_cam_pos, COLOR_WHITE, names[node->type]);

		if (inst && (node->type == NodeType::SceneCallback || node->type == NodeType::ScriptProperty))
		{
			SceneScriptInst::Node& scene_node = inst->nodes[index];

			SceneObjectRef& ref = scene_node.objects[0].ref;

			if (!ref.object)
			{
				ref.object = inst->GetScene()->FindByUID(ref.uid, ref.child_uid, ref.is_asset);
			}

			editor_drawer.PrintText(node->pos + Vector2(5.0f, 50.0f) - Sprite::ed_cam_pos, COLOR_WHITE, ref.object ? ref.object->GetName() : "NULL");
		}

		index++;
	}

	if (sel_node != -1 && link_drag)
	{
		editor_drawer.DrawCurve(nodes[sel_node]->pos + Vector2(nodeSize.x, 37.0f) - Sprite::ed_cam_pos, ms_pos, COLOR_WHITE);
	}
}

void SceneScriptAsset::OnMouseMove(Vector2 delta_ms)
{
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

			if (node->pos.x - Sprite::ed_cam_pos.x < ms_pos.x && ms_pos.x < node->pos.x + 15.0f - Sprite::ed_cam_pos.x &&
				node->pos.y + 30.0f - Sprite::ed_cam_pos.y < ms_pos.y && ms_pos.y < node->pos.y + 45.0f - Sprite::ed_cam_pos.y)
			{
				target_link = index;
			}

			index++;
		}
	}
}

void SceneScriptAsset::OnLeftMouseDown(Vector2 ms)
{
	ShowProperties(false);

	sel_node = -1;
	sel_link = -1;
	link_drag = false;
	in_drag = false;

	int index = 0;
	for (auto& node : nodes)
	{
		if (node->pos.x - Sprite::ed_cam_pos.x < ms.x && ms.x < node->pos.x + nodeSize.x - Sprite::ed_cam_pos.x &&
			node->pos.y - Sprite::ed_cam_pos.y < ms.y && ms.y < node->pos.y + nodeSize.y - Sprite::ed_cam_pos.y)
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
			if (node->pos.x + nodeSize.x - 15.0f - Sprite::ed_cam_pos.x < ms.x && ms.x < node->pos.x + nodeSize.x - Sprite::ed_cam_pos.x &&
				node->pos.y + 30.0f - Sprite::ed_cam_pos.y < ms.y && ms.y < node->pos.y + 45.0f - Sprite::ed_cam_pos.y)
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

	ed_popup_menu->StartMenu(true);

	ed_popup_menu->StartSubMenu("Create Link to");
	ed_popup_menu->AddItem(5001, "Callback");
	ed_popup_menu->AddItem(5002, "Property");
	ed_popup_menu->AddItem(5003, "Method");
	ed_popup_menu->EndSubMenu();

	ed_popup_menu->AddItem(5005, "Delete", (sel_node != -1));

	ed_popup_menu->ShowAsPopup(ed_vieport, (int)ms.x, (int)ms.y);
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

	if (node)
	{
		node->pos = ms_pos + Sprite::ed_cam_pos;
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
				link.GetMetaData()->PrepareWidgets(ed_obj_cat);
			}
		}
		else
		if (sel_node != -1)
		{
			Node* node = nodes[sel_node];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(ed_obj_cat);
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