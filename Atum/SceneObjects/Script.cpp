#include "Script.h"
#include "SceneObjects/RenderLevels.h"

#include "SceneAssets/UI/UIViewInstanceAsset.h"
#include "SceneAssets/UI/UIButtonAsset.h"
#include "SceneAssets/UI/UILabelAsset.h"

CLASSREG(SceneObject, Script, "Script")

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

void Script::Init()
{
	UIViewInstance* uiinst = (UIViewInstance*)owner->Find("Menu", false);

	NodeSceneObject node;
	node.pos = { 100, 100 };
	node.name = "Button1";
	node.object = uiinst->childs[1]->childs[0];
	link_objects.push_back(node);

	node.pos = { 100, 250 };
	node.name = "Button2";
	node.object = uiinst->childs[2]->childs[0];
	link_objects.push_back(node);

	node.pos = { 100, 400 };
	node.name = "Label";
	node.object = uiinst->childs[3];
	link_objects.push_back(node);

	NodeScriptObject node2;
	node2.type = 1;
	node2.pos = { 400, 175 };
	node2.linked_nodes.push_back({ 0, "Title1" });
	node2.linked_nodes.push_back({ 1, "Title2" });
	node2.name = "OnButton";
	node2.def = "void OnButton()";
	script_objects.push_back(node2);

	node2.type = 0;
	node2.pos = { 400, 400 };
	node2.linked_nodes.clear();
	node2.linked_nodes.push_back({ 2, "" });
	node2.name = "label";
	script_objects.push_back(node2);

	Tasks(false)->AddTask(100, this, (Object::Delegate)&Script::Work);
	Tasks(true)->AddTask(100, this, (Object::Delegate)&Script::EditorWork);
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

	for (auto& node : script_objects)
	{
		if (node.type == 0)
		{
			for (int i = 0; i < (int)class_inst->GetPropertyCount(); i++)
			{
				if (StringUtils::IsEqual(class_inst->GetPropertyName(i), node.name.c_str()))
				{
					*(asPWORD*)(class_inst->GetAddressOfProperty(i)) = (asPWORD)link_objects[node.linked_nodes[0].node].object;

					break;
				}
			}
		}

		if (node.type == 1)
		{
			node.method = class_type->GetMethodByDecl("void OnButton(string&in)");
		}
	}
}

void Script::Work(float dt)
{
	if (!Playing())
	{
		return;
	}

	for (auto& node : script_objects)
	{
		if (node.type == 1)
		{
			for (auto link : node.linked_nodes)
			{
				UIButtonAssetInst* btn = (UIButtonAssetInst*)link_objects[link.node].object;

				if (btn->fired)
				{
					Scipt()->ctx->Prepare(node.method);
					Scipt()->ctx->SetArgObject(0, &link.param);
					Scipt()->ctx->SetObject(class_inst);
					Scipt()->ctx->Execute();

					btn->fired = 0;
				}
			}
		}
	}
}

void Script::EditorWork(float dt)
{
	Vector2 nodeSize(100.0f, 50.0f);

	for (auto& node : link_objects)
	{
		render.DebugSprite(nullptr, node.pos, nodeSize, COLOR_BLUE);
		render.DebugPrintText(node.pos + Vector2(10.0f), COLOR_WHITE, node.name.c_str());
	}

	for (auto& node : script_objects)
	{
		for (auto link : node.linked_nodes)
		{
			render.DebugLine2D(link_objects[link.node].pos + nodeSize * 0.5f, COLOR_WHITE, node.pos + nodeSize * 0.5f, COLOR_WHITE);
		}

		render.DebugSprite(nullptr, node.pos, nodeSize, COLOR_BLUE);
		render.DebugPrintText(node.pos + Vector2(10.0f), COLOR_WHITE, node.name.c_str());
	}
}