
#include "SpriteGraphAsset.h"
#include "Services/Render/Render.h"
#include "SpriteGraphWindow.h"
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

#ifdef EDITOR
void StartEditGraphSprite(void* owner)
{
	SpriteGraphWindow::StartEdit((SpriteGraphAsset*)owner);
}
#endif

CLASSREG(SceneAsset, SpriteGraphAsset, "SpriteGraph")

META_DATA_DESC(SpriteGraphAsset)
FLOAT_PROP(SpriteGraphAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteGraphAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteGraphAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteGraphAsset, trans.size.y, 100.0f, "Prop", "height")
#ifdef EDITOR
CALLBACK_PROP(SpriteGraphAsset, StartEditGraphSprite, "Prop", "EditGraph")
#endif
META_DATA_DESC_END()

Sprite::FrameState SpriteGraphAsset::state;

SpriteGraphAsset::SpriteGraphAsset() : SceneAsset()
{
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
		node.asset = (SpriteAsset*)owner->Find(node.asset_name.c_str(), true);

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
	Sprite::Load(loader, &sprite);

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
	Sprite::Save(saver, &sprite);

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
		render.DebugLine2D(nodes[selNode].pos - Sprite::ed_cam_pos + nodeSize * 0.5f, COLOR_WHITE, mouse_pos, COLOR_WHITE);
	}

	if (selNode != -1)
	{
		Node& node = nodes[selNode];
		SpriteAsset* asset = (SpriteAsset*)owner->Find(node.asset_name.c_str(), true);

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
			Sprite::Draw(&trans, COLOR_WHITE, &node.asset->sprite, &state, true);
		}
	}

	if (selLink != -1 && selNode != -1)
	{
		Node& node = nodes[selNode];

		if (node.links[selLink].def_link)
		{
			if (node.def_link != -1 && node.def_link != selLink)
			{
				node.links[node.def_link].def_link = false;
			}

			node.def_link = selLink;
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
			if (index == selNode && selLink != -1)
			{
				if (node.links[selLink].def_link)
				{
					if (link_index != selLink)
					{
						link.def_link = false;
					}
					else
					{
						node.def_link = selLink;
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
			render.DebugSprite(nullptr, link.arrow_pos, linkSize, (selNode == index && selLink == link_index) ? COLOR_GREEN : color);

			link_index++;
		}

		Color color = (selNode == index || targetNode == index) ? Color(0.5f) : Color(0.0f);

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
	node.pos = Sprite::ed_cam_pos;
	node.name = "Node";

	nodes.push_back(node);
}

void SpriteGraphAsset::Delete()
{
	if (selNode == -1)
	{
		return;
	}

	if (selLink != -1)
	{
		nodes[selNode].links.erase(nodes[selNode].links.begin() + selLink);
		ShowProperties(false);
		return;
	}

	if (nodes[selNode].type == AnimNode)
	{
		def_node = -1;
	}

	for (auto& node : nodes)
	{
		for (int i = 0; i < node.links.size(); i++)
		{
			if (node.links[i].index == selNode)
			{
				node.links.erase(node.links.begin() + i);
				i--;
			}
			else
			if (node.links[i].index > selNode)
			{
				node.links[i].index--;
			}
		}
	}

	nodes.erase(nodes.begin() + selNode);

	ShowProperties(false);
}

void SpriteGraphAsset::MakeNodeAsDefault()
{
	if (selNode != -1 && nodes[selNode].type == AnimNode)
	{
		def_node = selNode;
	}
}

void SpriteGraphAsset::ShowProperties(bool show)
{
	if (show)
	{
		if (selNode != -1)
		{
			SpriteGraphWindow::StartEdit(this);
		}

		if (selNode != -1 && selLink != -1)
		{
			Link* link = &nodes[selNode].links[selLink];
			link->GetMetaData()->Prepare(link);
			link->GetMetaData()->PrepareWidgets(SpriteGraphWindow::instance->panel_obj);
		}
		else
		if (selNode != -1)
		{
			Node* node = &nodes[selNode];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(SpriteGraphWindow::instance->panel_obj);
		}
	}
	else
	{
		if (selNode != -1 && selLink != -1)
		{
			nodes[selNode].links[selLink].GetMetaData()->HideWidgets();
		}
		else
		if (selNode != -1)
		{
			nodes[selNode].GetMetaData()->HideWidgets();
		}

		selNode = -1;
		selLink = -1;
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
	Gizmo::inst->enabled = false;
	Gizmo::inst->trans2D = nullptr;

	if (!ed)
	{
		ShowProperties(false);
		SpriteGraphWindow::StopEdit();
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
		nodes[selNode].pos += delta_ms;
	}

	if (drag == AddLink)
	{
		targetNode = GetNodeIndex(mouse_pos);

		if ((targetNode == selNode) || (targetNode != -1 && nodes[targetNode].type == GroupNode))
		{
			targetNode = -1;
		}
	}
}

void SpriteGraphAsset::OnLeftMouseDown(Vector2 ms)
{
	mouse_pos = ms;

	ShowProperties(false);

	selNode = GetNodeIndex(mouse_pos);
	selLink = -1;

	int selected = -1;

	int index = 0;
	for (auto& node : nodes)
	{
		int link_index = 0;
		for (auto& link : node.links)
		{
			if (link.arrow_pos.x < ms.x && ms.x < link.arrow_pos.x + linkSize.x &&
				link.arrow_pos.y < ms.y && ms.y < link.arrow_pos.y + linkSize.y)
			{
				selNode = index;
				selLink = link_index;
			}

			link_index++;
		}

		index++;
	}

	if (selNode != -1 && selLink == -1)
	{
		state.cur_time = -1.0f;
		drag = MoveNode;
	}

	ShowProperties(true);
}

void SpriteGraphAsset::OnLeftMouseUp()
{
	drag = None;
}

void SpriteGraphAsset::OnRightMouseDown(Vector2 ms)
{
	mouse_pos = ms;

	ShowProperties(false);

	selNode = GetNodeIndex(mouse_pos);
	selLink = -1;

	if (selNode != -1)
	{
		ShowProperties(true);

		drag = AddLink;

		if (nodes[selNode].type == GroupNode)
		{
			drag = None;
		}

		return;
	}


	drag = MoveField;
}

void SpriteGraphAsset::OnRightMouseUp()
{
	drag = None;

	if (targetNode != -1)
	{
		bool allow_add = true;

		for (auto& link : nodes[selNode].links)
		{
			if (link.index == targetNode)
			{
				allow_add = false;
				break;
			}
		}

		if (allow_add)
		{
			Link link;
			link.index = targetNode;

			nodes[selNode].links.push_back(link);
		}

		targetNode = -1;
	}
}
#endif