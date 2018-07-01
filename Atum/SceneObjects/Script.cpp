#include "Script.h"
#include "SceneObjects/RenderLevels.h"

#include "SceneAssets/UI/UIViewInstanceAsset.h"
#include "SceneAssets/UI/UIButtonAsset.h"
#include "SceneAssets/UI/UILabelAsset.h"

CLASSREG(SceneObject, Script, "Script")

META_DATA_DESC(Script::NodeSceneObject)
STRING_PROP(Script::NodeSceneObject, name, "", "Property", "Name")
META_DATA_DESC_END()

META_DATA_DESC(Script::NodeScriptProperty)
STRING_PROP(Script::NodeScriptProperty, name, "", "Property", "Name")
META_DATA_DESC_END()

META_DATA_DESC(Script::LinkToMethod)
STRING_PROP(Script::LinkToMethod, param, "", "Property", "Params")
META_DATA_DESC_END()

META_DATA_DESC(Script::NodeScriptMethod)
STRING_PROP(Script::NodeScriptMethod, name, "", "Property", "Name")
ENUM_PROP(Script::NodeScriptMethod, param_type, 0, "Property", "ParamType")
	ENUM_ELEM("None", 0)
	ENUM_ELEM("Int", 1)
	ENUM_ELEM("String", 2)
ENUM_END
META_DATA_DESC_END()

#ifdef EDITOR
void StartScriptEdit(void* owner)
{
	Script* script = (Script*)owner;
	char dir[1024];
	GetCurrentDirectory(1024, dir);
	string filename = string(dir) + string("//Media//") + string(script->GetName()) + ".asc";
	ShellExecuteA(nullptr, "open", filename.c_str(), NULL, NULL, SW_SHOW);
}
#endif

META_DATA_DESC(Script)
#ifdef EDITOR
CALLBACK_PROP(SpriteAsset, StartScriptEdit, "Prop", "EditScript")
#endif
META_DATA_DESC_END()

asIScriptModule* mod = nullptr;
asITypeInfo* class_type = nullptr;
asIScriptObject* class_inst = nullptr;

void Script::Node::Load(JSONReader& loader)
{
	loader.Read("name", name);
	loader.Read("pos", pos);
}

void Script::Node::Save(JSONWriter& saver)
{
	saver.Write("type", (int&)type);
	saver.Write("name", name.c_str());
	saver.Write("pos", pos);
}

void Script::NodeSceneObject::Load(JSONReader& loader)
{
	Node::Load(loader);
}

void Script::NodeSceneObject::Save(JSONWriter& saver)
{
	Node::Save(saver);
}

void Script::NodeScriptProperty::Load(JSONReader& loader)
{
	Node::Load(loader);

	int link_count = 0;
	loader.Read("Count", link_count);
	links.resize(link_count);

	for (auto& link : links)
	{
		loader.EnterBlock("Link");

		link = new LinkToProperty();

		loader.Read("node", link->node);

		loader.LeaveBlock();
	}
}

void Script::NodeScriptProperty::Save(JSONWriter& saver)
{
	Node::Save(saver);

	int link_count = (int)links.size();
	saver.Write("Count", link_count);

	saver.StartArray("Link");

	for (auto link : links)
	{
		saver.StartBlock(nullptr);

		saver.Write("node", link->node);

		saver.FinishBlock();
	}

	saver.FinishArray();
}

void Script::NodeScriptMethod::Load(JSONReader& loader)
{
	Node::Load(loader);

	loader.Read("param_type", param_type);

	int link_count = 0;
	loader.Read("Count", link_count);
	links.resize(link_count);

	for (auto& link : links)
	{
		loader.EnterBlock("Link");

		link = new LinkToMethod();

		loader.Read("node", link->node);
		loader.Read("param", ((LinkToMethod*)link)->param);

		loader.LeaveBlock();
	}
}

void Script::NodeScriptMethod::Save(JSONWriter& saver)
{
	Node::Save(saver);

	saver.Write("param_type", param_type);

	int link_count = (int)links.size();
	saver.Write("Count", link_count);

	saver.StartArray("Link");

	for (auto link : links)
	{
		saver.StartBlock(nullptr);

		saver.Write("node", link->node);
		saver.Write("param", ((LinkToMethod*)link)->param.c_str());

		saver.FinishBlock();
	}

	saver.FinishArray();
}

void Script::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&Script::Work);
	Tasks(true)->AddTask(100, this, (Object::Delegate)&Script::EditorWork);
}

void Script::Load(JSONReader& loader)
{
	UIViewInstance* uiinst = (UIViewInstance*)owner->Find("Menu", false);

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

		if (type == ScnObject || type == ScnCallback)
		{
			node = new NodeSceneObject();
		}

		if (type == ScriptProperty)
		{
			node = new NodeScriptProperty();
		}

		if (type == ScriptMethod)
		{
			node = new NodeScriptMethod();
		}

		node->type = (NodeType)type;
		node->Load(loader);

		if (type == ScnCallback && StringUtils::IsEqual(node->name.c_str(), "Button1"))
		{
			((NodeSceneObject*)node)->object = uiinst->childs[1]->childs[0];
		}

		if (type == ScnCallback && StringUtils::IsEqual(node->name.c_str(), "Button2"))
		{
			((NodeSceneObject*)node)->object = uiinst->childs[2]->childs[0];
		}

		if (type == ScnObject && StringUtils::IsEqual(node->name.c_str(), "Label"))
		{
			((NodeSceneObject*)node)->object = uiinst->childs[3];
		}

		loader.LeaveBlock();
	}
}

void Script::Save(JSONWriter& saver)
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

void Script::SetName(const char* set_name)
{
	SceneObject::SetName(set_name);

	if (set_name[0])
	{
		string filename = string("Media//") + string(GetName()) + ".asc";
		FILE* fl = fopen(filename.c_str(), "a");
		fclose(fl);
	}
}

void LabelSetText_Generic(asIScriptGeneric *gen)
{
	UILabelAssetInst* inst = (UILabelAssetInst*)gen->GetObject();
	string* arg = (string*)(gen->GetArgAddress(0));
	inst->text = *arg;
}

void Script::Play()
{
	string filename = string("Media//") + string(GetName()) + ".asc";

	FILE *f = fopen(filename.c_str(), "rb");

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);

	string script;
	script.resize(len);
	size_t c = fread(&script[0], len, 1, f);
	fclose(f);

	scripts.engine->RegisterObjectType("UILabel", sizeof(UILabelAssetInst), asOBJ_REF | asOBJ_NOCOUNT);
	scripts.engine->RegisterObjectMethod("UILabel", "void SetText(string&in)", asFUNCTION(LabelSetText_Generic), asCALL_GENERIC);

	mod = scripts.engine->GetModule(0, asGM_ALWAYS_CREATE);
	mod->AddScriptSection("script", &script[0], len);

	mod->Build();

	class_type = mod->GetTypeInfoByName("Tester");
	class_inst = (asIScriptObject*)scripts.engine->CreateUninitializedScriptObject(class_type);

	for (auto node : nodes)
	{
		if (node->type == ScriptProperty)
		{
			NodeScriptProperty* node_prop = (NodeScriptProperty*)node;

			if (node_prop->links.size() > 0)
			{
				for (int i = 0; i < (int)class_inst->GetPropertyCount(); i++)
				{
					if (StringUtils::IsEqual(class_inst->GetPropertyName(i), node_prop->name.c_str()))
					{
						NodeSceneObject* scene_prop = (NodeSceneObject*)nodes[node_prop->links[0]->node];
						*(asPWORD*)(class_inst->GetAddressOfProperty(i)) = (asPWORD)scene_prop->object;
						break;
					}
				}
			}
		}
		else
		if (node->type == ScriptMethod)
		{
			NodeScriptMethod* node_method = (NodeScriptMethod*)node;

			const char* decls[] = { "", "int", "string&in" };
			char prototype[256];
			StringUtils::Printf(prototype, 256, "void %s(%s)", node_method->name.c_str(), decls[node_method->param_type]);

			node_method->method = class_type->GetMethodByDecl(prototype);
		}
	}
}

void Script::Work(float dt)
{
	if (!Playing())
	{
		return;
	}

	for (auto& node : nodes)
	{
		if (node->type == ScriptMethod)
		{
			NodeScriptMethod* node_method = (NodeScriptMethod*)node;

			for (auto link : node_method->links)
			{
				NodeSceneObject* scene_prop = (NodeSceneObject*)nodes[link->node];
				UIButtonAssetInst* btn = (UIButtonAssetInst*)scene_prop->object;

				if (btn->fired)
				{
					LinkToMethod* node_link = (LinkToMethod*)link;
					Scipt()->ctx->Prepare(node_method->method);
					Scipt()->ctx->SetArgObject(0, &node_link->param);
					Scipt()->ctx->SetObject(class_inst);
					Scipt()->ctx->Execute();

					btn->fired = 0;
				}
			}
		}
	}
}

void Script::Stop()
{
	class_inst->Release();
}

void Script::Release()
{
	for (auto node : nodes)
	{
		if (node->type == ScriptProperty || node->type == ScriptMethod)
		{
			NodeScriptProperty* prop_node = (NodeScriptProperty*)node;

			for (auto& link : prop_node->links)
			{
				delete link;
			}
		}

		delete node;
	}
}

bool Script::UsingCamera2DPos()
{
	return true;
}

Vector2& Script::Camera2DPos()
{
	return camera_pos;
}

void Script::EditorWork(float dt)
{
	int index = 0;
	for (auto node : nodes)
	{
		if (node->type == ScriptProperty)
		{
			int link_index = 0;
			for (auto link : ((NodeScriptProperty*)node)->links)
			{
				Vector2 p1 = nodes[link->node]->pos + nodeSize * 0.5f - Sprite::ed_cam_pos;
				Vector2 p2 = node->pos + nodeSize * 0.5f - Sprite::ed_cam_pos;

				render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);

				link->arrow_pos = p1 + (p2 - p1) * 0.75f - linkSize * 0.5f;
				Color color = (sel_node == index && link_index == sel_link) ? COLOR_CYAN : COLOR_WHITE;
				render.DebugSprite(nullptr, link->arrow_pos, linkSize, color);

				link_index++;
			}
		}

		if (node->type == ScriptMethod)
		{
			int link_index = 0;
			for (auto link : ((NodeScriptMethod*)node)->links)
			{
				Vector2 p1 = nodes[link->node]->pos + nodeSize * 0.5f - Sprite::ed_cam_pos;
				Vector2 p2 = node->pos + nodeSize * 0.5f - Sprite::ed_cam_pos;

				render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);

				link->arrow_pos = p1 + (p2 - p1) * 0.75f - linkSize * 0.5f;
				Color color = (sel_node == index && link_index == sel_link) ? COLOR_CYAN : COLOR_WHITE;
				render.DebugSprite(nullptr, link->arrow_pos, linkSize, color);

				link_index++;
			}
		}

		Color color = COLOR_BLUE;
		if ((sel_node == index || target_link == index) && sel_link == -1)
		{
			color = COLOR_CYAN;
		}

		render.DebugSprite(nullptr, node->pos - Sprite::ed_cam_pos, nodeSize, color);
		render.DebugPrintText(node->pos + Vector2(10.0f, 30.0f) - Sprite::ed_cam_pos, COLOR_WHITE, node->name.c_str());

		const char* names[] = {"SceneObject:", "Callback:", "Property:", "Method:"};
		render.DebugPrintText(node->pos + Vector2(10.0f, 10.0f) - Sprite::ed_cam_pos, COLOR_WHITE, names[node->type]);

		index++;
	}

	if (in_drag)
	{
		if (sel_node != -1 && link_drag)
		{
			render.DebugLine2D(nodes[sel_node]->pos + nodeSize * 0.5f - Sprite::ed_cam_pos, COLOR_WHITE, ms_pos, COLOR_WHITE);
		}
	}
}

#ifdef EDITOR
void Script::OnMouseMove(Vector2 delta_ms)
{
	if (in_drag)
	{
		if (sel_node != -1 && !link_drag)
		{
			nodes[sel_node]->pos += delta_ms;
		}

		ms_pos += delta_ms;

		target_link = -1;
		int index = 0;
		for (auto& node : nodes)
		{
			if (node->pos.x - Sprite::ed_cam_pos.x < ms_pos.x && ms_pos.x < node->pos.x + nodeSize.x - Sprite::ed_cam_pos.x &&
				node->pos.y - Sprite::ed_cam_pos.y < ms_pos.y && ms_pos.y < node->pos.y + nodeSize.y - Sprite::ed_cam_pos.y)
			{
				target_link = index;
			}
			index++;
		}
	}
}

void Script::OnLeftMouseDown(Vector2 ms)
{
	ShowProperties(false);

	sel_node = -1;
	sel_link = -1;
	in_drag = false;

	int index = 0;
	for (auto& node : nodes)
	{
		if (node->pos.x - Sprite::ed_cam_pos.x < ms.x && ms.x < node->pos.x + nodeSize.x - Sprite::ed_cam_pos.x &&
			node->pos.y - Sprite::ed_cam_pos.y < ms.y && ms.y < node->pos.y + nodeSize.y - Sprite::ed_cam_pos.y)
		{
			sel_node = index;
		}

		if (node->type == ScriptProperty)
		{
			int link_index = 0;

			for (auto& link : ((NodeScriptProperty*)node)->links)
			{
				if (link->arrow_pos.x < ms.x && ms.x < link->arrow_pos.x + linkSize.x &&
					link->arrow_pos.y < ms.y && ms.y < link->arrow_pos.y + linkSize.y)
				{
					sel_node = index;
					sel_link = link_index;
				}

				link_index++;
			}
		}

		if (node->type == ScriptMethod)
		{
			int link_index = 0;

			for (auto& link : ((NodeScriptMethod*)node)->links)
			{
				if (link->arrow_pos.x < ms.x && ms.x < link->arrow_pos.x + linkSize.x &&
					link->arrow_pos.y < ms.y && ms.y < link->arrow_pos.y + linkSize.y)
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
		link_drag = controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active);
		ms_pos = ms;
	}

	ShowProperties(true);
}

void Script::OnLeftMouseUp()
{
	if (in_drag && link_drag && target_link != -1)
	{
		if ((nodes[sel_node]->type == ScnObject && nodes[target_link]->type == ScriptProperty) ||
			(nodes[sel_node]->type == ScnCallback && nodes[target_link]->type == ScriptMethod))
		{
			int tmp = sel_node;
			sel_node = target_link;
			target_link = tmp;
		}

		Node* node = nodes[sel_node];
		Node* traget = nodes[target_link];

		if (node->type == ScriptProperty && traget->type == ScnObject)
		{
			NodeScriptProperty* prop_node = (NodeScriptProperty*)node;

			if (prop_node->links.size() == 0)
			{
				LinkToProperty* link = new LinkToProperty();

				link->node = target_link;

				prop_node->links.push_back(link);
			}
		}
		else
		if (node->type == ScriptMethod && traget->type == ScnCallback)
		{
			NodeScriptMethod* method_node = (NodeScriptMethod*)node;

			bool allow_add = true;

			for (auto& link : method_node->links)
			{
				if (link->node == target_link)
				{
					allow_add = false;
					break;
				}
			}

			if (allow_add)
			{
				LinkToMethod* link = new LinkToMethod();

				link->node = target_link;

				method_node->links.push_back(link);
			}
		}
	}

	in_drag = false;
	link_drag = false;
	target_link = -1;
}

void Script::OnRightMouseDown(Vector2 ms)
{
	ms_pos = ms;

	popup_menu->StartMenu(true);

	popup_menu->StartSubMenu("Create Link to");
	popup_menu->AddItem(5000, "Object");
	popup_menu->AddItem(5001, "Callback");
	popup_menu->AddItem(5002, "Property");
	popup_menu->AddItem(5003, "Method");
	popup_menu->EndSubMenu();

	if (sel_node != -1)
	{
		if (sel_link == -1)
		{
			//popup_menu->AddItem(5004, "Duplicate");
		}

		popup_menu->AddItem(5005, "Delete");
	}

	popup_menu->ShowAsPopup(vieport, (int)ms.x, (int)ms.y);
}

void Script::OnPopupMenuItem(int id)
{
	Node* node = nullptr;

	if (id == 5000)
	{
		NodeSceneObject* scene_node = new NodeSceneObject();
		scene_node->type = ScnObject;
		node = scene_node;
	}

	if (id == 5001)
	{
		NodeSceneObject* scene_node = new NodeSceneObject();
		scene_node->type = ScnCallback;
		node = scene_node;
	}

	if (id == 5002)
	{
		NodeScriptProperty* prop_node = new NodeScriptProperty();
		prop_node->type = ScriptProperty;
		node = prop_node;
	}

	if (id == 5003)
	{
		NodeScriptMethod* method_node = new NodeScriptMethod();
		method_node->type = ScriptMethod;
		node->name = "property";
		node = method_node;
	}

	if (node)
	{
		node->pos = ms_pos + Sprite::ed_cam_pos;
		node->name = "method";
		nodes.push_back(node);
	}

	if (id == 5004)
	{
	}

	if (id == 5005)
	{
		if (sel_link == -1)
		{
			Node* node = nodes[sel_node];

			if (node->type == ScriptProperty || node->type == ScriptMethod)
			{
				NodeScriptProperty* prop_node = (NodeScriptProperty*)nodes[sel_node];

				for (auto& link : prop_node->links)
				{
					delete link;
				}
			}
			else
			{
				for (auto& nd : nodes)
				{
					if (nd->type == ScriptProperty || nd->type == ScriptMethod)
					{
						NodeScriptProperty* prop_node = (NodeScriptProperty*)nd;

						int index = 0;
						for (auto& link : prop_node->links)
						{
							if (link->node == sel_node)
							{
								delete link;
								prop_node->links.erase(prop_node->links.begin() + index);
							}
							else
							if (link->node > sel_node)
							{
								link->node--;
							}

							index++;
						}
					}
				}
			}

			int sel_nd = sel_node;
			ShowProperties(false);

			delete node;
			nodes.erase(nodes.begin() + sel_nd);
		}
		else
		{
			NodeScriptProperty* prop_node = (NodeScriptProperty*)nodes[sel_node];

			int link = sel_link;
			ShowProperties(false);

			delete prop_node->links[link];
			prop_node->links.erase(prop_node->links.begin() + link);
		}
	}
}

void Script::ShowProperties(bool show)
{
	if (show)
	{
		if (sel_node != -1 && sel_link != -1)
		{
			LinkToProperty* link = ((NodeScriptProperty*)nodes[sel_node])->links[sel_link];
			if (link->GetMetaData())
			{
				link->GetMetaData()->Prepare(link);
				link->GetMetaData()->PrepareWidgets(cat);
			}
		}
		else
		if (sel_node != -1)
		{
			Node* node = nodes[sel_node];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(cat);
		}
	}
	else
	{
		if (sel_node != -1 && sel_link != -1)
		{
			LinkToProperty* link = ((NodeScriptProperty*)nodes[sel_node])->links[sel_link];
			if (link->GetMetaData())
			{
				link->GetMetaData()->HideWidgets();
			}
		}
		else
		if (sel_node != -1)
		{
			nodes[sel_node]->GetMetaData()->HideWidgets();
		}

		sel_node = -1;
		sel_link = -1;
	}
}
#endif