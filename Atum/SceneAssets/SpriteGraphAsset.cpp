
#include "SpriteGraphAsset.h"
#include "SpriteGraphInst.h"
#include "Services/Render/Render.h"
#include "SpriteAsset.h"
#include "SceneObjects/RenderLevels.h"

META_DATA_DESC(SpriteGraphAsset::Link)
STRING_PROP(SpriteGraphAsset::Link, name, "", "Link", "Name")
BOOL_PROP(SpriteGraphAsset::Link, def_link, "", "Link", "DefaultLink")
META_DATA_DESC_END()

META_DATA_DESC(SpriteGraphAsset::Node)
STRING_PROP(SpriteGraphAsset::Node, name, "", "Node", "Name")
STRING_PROP(SpriteGraphAsset::Node, asset_name, "", "Node", "Asset")
BOOL_PROP(SpriteGraphAsset::Node, looped, true, "Node", "Looped")
BOOL_PROP(SpriteGraphAsset::Node, reversed, false, "Node", "Reversed")
META_DATA_DESC_END()

void SpriteGraphAsset::Instance::GotoNode(int index)
{
	cur_node = &asset->nodes[index];
	state.cur_time = -1.0f;
	state.finished = false;
	state.looped = cur_node->looped;
	state.reversed = cur_node->reversed;
}

bool SpriteGraphAsset::Instance::GotoNode(const char* node_name)
{
	int index = 0;

	for (auto& node : asset->nodes)
	{
		if (StringUtils::IsEqual(node.name.c_str(), node_name))
		{
			GotoNode(index);
			return true;
		}

		index++;
	}

	return false;
}

void SpriteGraphAsset::Instance::Reset()
{
	if (!asset)
	{
		return;
	}

	GotoNode(asset->def_node);
}

bool SpriteGraphAsset::Instance::ActivateLink(const char* link_name)
{
	if (!asset)
	{
		return false;
	}

	for (auto& link : cur_node->links)
	{
		if (StringUtils::IsEqual(link.name.c_str(), link_name))
		{
			GotoNode(link.index);
			return true;
		}
	}

	return false;
}

void SpriteGraphAsset::Instance::Update(float dt)
{
	if (!asset)
	{
		return;
	}

	Sprite::UpdateFrame(&cur_node->asset->sprite, &state, dt);

	if (!cur_node->looped && state.finished)
	{
		if (cur_node->def_link != -1)
		{
			GotoNode(cur_node->def_link);
			Sprite::UpdateFrame(&cur_node->asset->sprite, &state, 0.0f);
		}
	}
}

CLASSREG(SceneAsset, SpriteGraphAsset, "SpriteGraph")

META_DATA_DESC(SpriteGraphAsset)
BASE_SCENE_OBJ_NAME_PROP(SpriteGraphAsset)
FLOAT_PROP(SpriteGraphAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteGraphAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteGraphAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteGraphAsset, trans.size.y, 100.0f, "Prop", "height")
META_DATA_DESC_END()

Sprite::FrameState SpriteGraphAsset::state;

SpriteGraphAsset::SpriteGraphAsset() : SceneAsset()
{
	inst_class_name = "SpriteGraphInst";
}

SpriteGraphAsset::~SpriteGraphAsset()
{
}

void SpriteGraphAsset::Init()
{
#ifdef EDITOR
	RenderTasks(true)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteGraphAsset::Draw);
#endif

	owner->AddToGroup(this, "SpriteGraphAsset");
}

void SpriteGraphAsset::ApplyProperties()
{
	valid = true;

	for (auto& node : nodes)
	{
		node.asset = (SpriteAsset*)owner->FindByName(node.asset_name.c_str(), true);

		if (!node.asset)
		{
			valid = false;
		}
	}
}

void SpriteGraphAsset::Load(JSONReader& loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);

	loader.Read("def_node", def_node);

	int count = 0;
	loader.Read("Count", count);
	nodes.resize(count);

	for (int i = 0; i<count; i++)
	{
		if (!loader.EnterBlock("Node")) break;

		Node& node = nodes[i];
		loader.Read("type", (int&)node.type);
		loader.Read("looped", node.looped);
		loader.Read("reverse", node.reversed);
		loader.Read("pos", node.pos);
		loader.Read("name", node.name);
		loader.Read("asset", node.asset_name);
		loader.Read("def_link", node.def_link);

		int link_count = 0;
		loader.Read("Count", link_count);
		node.links.resize(link_count);

		for (int j = 0; j<link_count; j++)
		{
			if (!loader.EnterBlock("Link")) break;

			Link& link = node.links[j];

			loader.Read("index", link.index);
			loader.Read("name", link.name);

			loader.LeaveBlock();
		}

		if (node.def_link != -1)
		{
			node.links[node.def_link].def_link = true;
		}

		loader.LeaveBlock();
	}
}

void SpriteGraphAsset::Save(JSONWriter& saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);

	saver.Write("def_node", def_node);

	int count = (int)nodes.size();
	saver.Write("Count", count);

	saver.StartArray("Node");

	for (int i = 0; i<count; i++)
	{
		saver.StartBlock(nullptr);

		Node& node = nodes[i];
		saver.Write("type", (int&)node.type);
		saver.Write("looped", node.looped);
		saver.Write("reverse", node.reversed);
		saver.Write("pos", node.pos);
		saver.Write("name", node.name.c_str());
		saver.Write("asset", node.asset_name.c_str());
		saver.Write("def_link", node.def_link);

		int link_count = (int)node.links.size();
		saver.Write("Count", link_count);

		saver.StartArray("Link");

		for (int j = 0; j < link_count; j++)
		{
			saver.StartBlock(nullptr);

			Link& link = node.links[j];

			saver.Write("index", link.index);
			saver.Write("name", link.name.c_str());

			saver.FinishBlock();
		}

		saver.FinishArray();

		saver.FinishBlock();
	}

	saver.FinishArray();
}

#ifdef EDITOR
void SpriteGraphAsset::Draw(float dt)
{
	if (drag == AddLink)
	{
		render.DebugLine2D(nodes[sel_node].pos - Sprite::ed_cam_pos + nodeSize * 0.5f, COLOR_WHITE, mouse_pos, COLOR_WHITE);
	}

	if (sel_node != -1)
	{
		Node& node = nodes[sel_node];
		SpriteAsset* asset = (SpriteAsset*)owner->FindByName(node.asset_name.c_str(), true);

		if (node.asset != asset)
		{
			state.cur_time = -1.0f;
			node.asset = asset;
		}

		if (node.asset)
		{
			state.looped = node.looped;
			state.reversed = node.reversed;

			trans.pos = node.asset->trans.size * 0.5f;
			trans.size = node.asset->trans.size;
			trans.BuildMatrices();
			Sprite::UpdateFrame(&node.asset->sprite, &state, dt);
			Sprite::Draw(&trans, COLOR_WHITE, &node.asset->sprite, &state, true, true);
		}
	}

	if (sel_link != -1 && sel_node != -1)
	{
		Node& node = nodes[sel_node];

		if (node.links[sel_link].def_link)
		{
			if (node.def_link != -1 && node.def_link != sel_link)
			{
				node.links[node.def_link].def_link = false;
			}

			node.def_link = sel_link;
		}
		else
		{
			node.def_link = -1;
		}
	}

	int index = 0;
	for (auto& node : nodes)
	{
		int link_index = 0;
		for (auto& link : node.links)
		{
			if (index == sel_node && sel_link != -1)
			{
				if (node.links[sel_link].def_link)
				{
					if (link_index != sel_link)
					{
						link.def_link = false;
					}
					else
					{
						node.def_link = sel_link;
					}
				}
				else
				if (link.def_link)
				{
					node.def_link = link_index;
				}
			}

			Vector2 p1 = node.pos - Sprite::ed_cam_pos + nodeSize * 0.5f;
			Vector2 p2 = nodes[link.index].pos - Sprite::ed_cam_pos + nodeSize * 0.5f;

			render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);

			link.arrow_pos = p1 + (p2 - p1) * 0.75f - linkSize * 0.5f;
			Color color = (link_index == node.def_link) ? COLOR_CYAN : COLOR_WHITE;
			render.DebugSprite(nullptr, link.arrow_pos, linkSize, (sel_node == index && sel_link == link_index) ? COLOR_GREEN : color);

			link_index++;
		}

		Color color = (sel_node == index || target_node == index) ? Color(0.5f) : Color(0.0f);

		if (node.type == AnimNode)
		{
			color.b = 1.0f;

			if (index == def_node)
			{
				color.g = 1.0f;
			}
		}
		
		if (node.type == LogicNode)
		{
			color.g = 1.0f;
		}

		if (node.type == GroupNode)
		{
			color.r = 1.0f;
			color.b = 1.0f;
		}

		render.DebugSprite(nullptr, node.pos - Sprite::ed_cam_pos, nodeSize, color);
		render.DebugPrintText(node.pos + Vector2(10.0f) - Sprite::ed_cam_pos, COLOR_WHITE,node.name.c_str());

		index++;
	}
}
#endif

bool SpriteGraphAsset::UsingCamera2DPos()
{
	return true;
}

Vector2& SpriteGraphAsset::Camera2DPos()
{
	return camera_pos;
}

void SpriteGraphAsset::PrepareInstance(Instance* inst)
{
	if (def_node == -1 || !valid)
	{
		return;
	}

	inst->asset = this;
	inst->Reset();
}

#ifdef EDITOR
int SpriteGraphAsset::GetNodeIndex(Vector2& ms)
{
	int selected = -1;

	int index = 0;
	for (auto& node : nodes)
	{
		if (node.pos.x - Sprite::ed_cam_pos.x < ms.x && ms.x < node.pos.x + nodeSize.x - Sprite::ed_cam_pos.x &&
			node.pos.y - Sprite::ed_cam_pos.y < ms.y && ms.y < node.pos.y + nodeSize.y - Sprite::ed_cam_pos.y)
		{
			selected = index;
		}

		index++;
	}

	return selected;
}

void SpriteGraphAsset::CreateNode(NodeType type)
{
	if (def_node == -1 && type == AnimNode)
	{
		def_node = (int)nodes.size();
	}

	Node node;
	node.type = type;
	node.pos = Sprite::ed_cam_pos + mouse_pos;
	node.name = "Node";

	nodes.push_back(node);
}

void SpriteGraphAsset::Delete()
{
	if (sel_node == -1)
	{
		return;
	}

	if (sel_link != -1)
	{
		nodes[sel_node].links.erase(nodes[sel_node].links.begin() + sel_link);
		ShowProperties(false);
		return;
	}

	if (nodes[sel_node].type == AnimNode)
	{
		def_node = -1;
	}

	for (auto& node : nodes)
	{
		for (int i = 0; i < node.links.size(); i++)
		{
			if (node.links[i].index == sel_node)
			{
				node.links.erase(node.links.begin() + i);
				i--;
			}
			else
			if (node.links[i].index > sel_node)
			{
				node.links[i].index--;
			}
		}
	}

	nodes.erase(nodes.begin() + sel_node);

	ShowProperties(false);
}

void SpriteGraphAsset::MakeNodeAsDefault()
{
	if (sel_node != -1 && nodes[sel_node].type == AnimNode)
	{
		def_node = sel_node;
	}
}

void SpriteGraphAsset::ShowProperties(bool show)
{
	if (show)
	{
		if (sel_node != -1 && sel_link != -1)
		{
			Link* link = &nodes[sel_node].links[sel_link];
			link->GetMetaData()->Prepare(link);
			link->GetMetaData()->PrepareWidgets(ed_obj_cat);
		}
		else
		if (sel_node != -1)
		{
			Node* node = &nodes[sel_node];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(ed_obj_cat);
		}
	}
	else
	{
		if (sel_node != -1 && sel_link != -1)
		{
			nodes[sel_node].links[sel_link].GetMetaData()->HideWidgets();
		}
		else
		if (sel_node != -1)
		{
			nodes[sel_node].GetMetaData()->HideWidgets();
		}

		sel_node = -1;
		sel_link = -1;
	}
}

void SpriteGraphAsset::Copy(SceneObject* src)
{
	SpriteGraphAsset* src_graph = (SpriteGraphAsset*)src;
	camera_pos = src_graph->camera_pos;
	def_node = src_graph->def_node;

	int count = (int)src_graph->nodes.size();
	nodes.resize(count);

	for (int i = 0; i < count; i++)
	{
		Node& node = nodes[i];
		node.type = src_graph->nodes[i].type;
		node.looped = src_graph->nodes[i].looped;
		node.reversed = src_graph->nodes[i].reversed;
		node.pos = src_graph->nodes[i].pos;
		node.name = src_graph->nodes[i].name;
		node.asset_name = src_graph->nodes[i].asset_name;
		node.def_link = src_graph->nodes[i].def_link;

		int link_count = (int)src_graph->nodes[i].links.size();
		node.links.resize(link_count);

		for (int j = 0; j < link_count; j++)
		{
			node.links[j] = src_graph->nodes[i].links[j];
		}
	}

	SceneAsset::Copy(src);
}

void SpriteGraphAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);
	
	Gizmo::inst->Disable();

	if (!ed)
	{
		ShowProperties(false);
	}
}

void SpriteGraphAsset::OnMouseMove(Vector2 delta_ms)
{
	mouse_pos += delta_ms;

	if (drag == None)
	{
		return;
	}

	if (drag == MoveNode)
	{
		nodes[sel_node].pos += delta_ms;
	}

	if (drag == AddLink)
	{
		target_node = GetNodeIndex(mouse_pos);

		if ((target_node == sel_node) || (target_node != -1 && nodes[target_node].type == GroupNode))
		{
			target_node = -1;
		}
	}
}

void SpriteGraphAsset::OnLeftMouseDown(Vector2 ms)
{
	mouse_pos = ms;

	ShowProperties(false);

	sel_node = GetNodeIndex(mouse_pos);
	sel_link = -1;

	if (sel_node != -1 && controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
	{
		ShowProperties(true);

		drag = AddLink;

		if (nodes[sel_node].type == GroupNode)
		{
			drag = None;
		}

		return;
	}

	int index = 0;
	for (auto& node : nodes)
	{
		int link_index = 0;
		for (auto& link : node.links)
		{
			if (link.arrow_pos.x < ms.x && ms.x < link.arrow_pos.x + linkSize.x &&
				link.arrow_pos.y < ms.y && ms.y < link.arrow_pos.y + linkSize.y)
			{
				sel_node = index;
				sel_link = link_index;
			}

			link_index++;
		}

		index++;
	}

	if (sel_node != -1 && sel_link == -1)
	{
		state.cur_time = -1.0f;
		drag = MoveNode;
	}

	ShowProperties(true);
}

void SpriteGraphAsset::OnLeftMouseUp()
{
	if (target_node != -1)
	{
		bool allow_add = true;

		for (auto& link : nodes[sel_node].links)
		{
			if (link.index == target_node)
			{
				allow_add = false;
				break;
			}
		}

		if (allow_add)
		{
			Link link;
			link.index = target_node;

			nodes[sel_node].links.push_back(link);
		}

		target_node = -1;
	}

	drag = None;
}

void SpriteGraphAsset::OnRightMouseDown(Vector2 ms)
{
	mouse_pos = ms;

	ed_popup_menu->StartMenu(true);

	ed_popup_menu->StartSubMenu("Create");
	ed_popup_menu->AddItem(5000, "Anim Node");
	//popup_menu->AddItem(5001, "Logic Node");
	//popup_menu->AddItem(5002, "Group Node");
	ed_popup_menu->EndSubMenu();

	ed_popup_menu->AddItem(5004, "Make Node as Default");
	ed_popup_menu->AddItem(5005, "Delete", (sel_node != -1));

	ed_popup_menu->ShowAsPopup(ed_vieport, (int)ms.x, (int)ms.y);

}

void SpriteGraphAsset::OnPopupMenuItem(int id)
{
	if (id == 5000)
	{
		CreateNode(SpriteGraphAsset::AnimNode);
	}

	if (id == 5001)
	{
		CreateNode(SpriteGraphAsset::LogicNode);
	}

	if (id == 5002)
	{
		CreateNode(SpriteGraphAsset::GroupNode);
	}

	if (id == 5004)
	{
		MakeNodeAsDefault();
	}

	if (id == 5005)
	{
		Delete();
	}
}
#endif