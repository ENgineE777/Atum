
#include "SpriteGraphAsset.h"
#include "SpriteGraphInst.h"
#include "Services/Render/Render.h"
#include "SpriteAsset.h"

#ifdef EDITOR

#include "Editor/EditorDrawer.h"
#include "Editor/Editor.h"

#endif

META_DATA_DESC(SpriteGraphAsset::Link)
	STRING_PROP(SpriteGraphAsset::Link, name, "", "Link", "Name")
	BOOL_PROP(SpriteGraphAsset::Link, def_link, "", "Link", "DefaultLink", "If link is default")
META_DATA_DESC_END()

META_DATA_DESC(SpriteGraphAsset::Event)
	FLOAT_PROP(SpriteGraphAsset::Event, time, 0.0f, "Event", "time", "Time on timeline when event should be fired")
	STRING_PROP(SpriteGraphAsset::Event, name, "", "Event", "Name")
	STRING_PROP(SpriteGraphAsset::Event, param, "", "Event", "param")
META_DATA_DESC_END()

META_DATA_DESC(SpriteGraphAsset::Node)
	STRING_PROP(SpriteGraphAsset::Node, name, "", "Node", "Name")
	BOOL_PROP(SpriteGraphAsset::Node, looped, true, "Node", "Looped", "If animation is looped")
	BOOL_PROP(SpriteGraphAsset::Node, reversed, false, "Node", "Reversed", "If node should be fliped horizontally")
	ARRAY_PROP(SpriteGraphAsset::Node, events, Event, "Node", "event")
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
	if (!asset)
	{
		return false;
	}

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

void SpriteGraphAsset::Instance::Update(const char* entity_name, int index, ScriptContext* context, SceneObject::ScriptCallback* callback, float dt)
{
	if (!asset)
	{
		return;
	}

	auto fire_events = [this, callback, index, context, entity_name](float from, float to)
	{
		if (callback)
		{
			for (auto& event : cur_node->events)
			{
				if (from <= event.time && event.time < to)
				{
					callback->Call(context, entity_name, index, event.name.c_str(), event.param.c_str());
				}
			}
		}
	};

	Sprite::UpdateFrame(&cur_node->asset->sprite, &state, dt, fire_events);

	if (!cur_node->looped && state.finished)
	{
		if (cur_node->def_link != -1)
		{
			GotoNode(cur_node->links[cur_node->def_link].index);
			Sprite::UpdateFrame(&cur_node->asset->sprite, &state, 0.0f);
		}
	}
}

#ifdef EDITOR
void CenterGraphCamera(void* owner)
{
	SpriteGraphAsset* graph = (SpriteGraphAsset*)owner;

	if (graph->nodes.size() > 0)
	{
		Vector2 leftCorner = FLT_MAX;
		Vector2 rightCorner = -FLT_MAX;

		for (auto& node : graph->nodes)
		{
			leftCorner.Min(node.pos - graph->nodeSize * 0.5f);
			rightCorner.Max(node.pos + graph->nodeSize * 1.5f);
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

CLASSREG(SceneAsset, SpriteGraphAsset, "SpriteGraph")

META_DATA_DESC(SpriteGraphAsset)
	BASE_SCENE_ASSET_PROP(SpriteGraphAsset)
#ifdef EDITOR
	CALLBACK_PROP(SpriteGraphAsset, CenterGraphCamera, "Prop", "CenterCamera")
#endif
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
	RenderTasks(true)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpriteGraphAsset::Draw);
#endif

	GetScene()->AddToGroup(this, "SpriteGraphAsset");
}

void SpriteGraphAsset::ApplyProperties()
{
	valid = true;

	for (auto& node : nodes)
	{
		node.asset = dynamic_cast<SpriteAsset*>(GetScene()->FindByUID(node.object_uid, 0, true));

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
		loader.Read("object_uid", node.object_uid);
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

		int event_count = 0;
		loader.Read("CountEvent", event_count);
		node.events.resize(event_count);

		for (int j = 0; j<event_count; j++)
		{
			if (!loader.EnterBlock("Event")) break;

			Event& event = node.events[j];

			loader.Read("time", event.time);
			loader.Read("name", event.name);
			loader.Read("param", event.param);

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
		saver.Write("object_uid", node.object_uid);
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

		int event_count = (int)node.events.size();
		saver.Write("CountEvent", event_count);

		saver.StartArray("Event");

		for (int j = 0; j < event_count; j++)
		{
			saver.StartBlock(nullptr);

			Event& event = node.events[j];

			saver.Write("time", event.time);
			saver.Write("name", event.name.c_str());
			saver.Write("param", event.param.c_str());

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
		editor_drawer.DrawLine(nodes[sel_node].pos + nodeSize * 0.5f, mouse_pos, COLOR_WHITE);
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

			Vector2 p1 = node.pos + nodeSize * 0.5f;
			Vector2 p2 = nodes[link.index].pos + nodeSize * 0.5f;

			Vector2 dir = (p2 - p1);
			float len = dir.Normalize();

			link.arrow_pos = p1 + dir * len * 0.4f;
			link.angle = atan2(dir.y, dir.x);

			if (sel_node != -1 && link.index == sel_node)
			{
				bool should_skip = false;

				for (auto& sub_link : nodes[sel_node].links)
				{
					if (sub_link.index == index)
					{
						should_skip = true;
						break;
					}
				}

				if (should_skip)
				{
					continue;
				}
			}

			Color color = COLOR_WHITE;

			if (index == sel_node)
			{
				if (sel_link == -1)
				{
					color = COLOR_YELLOW;
				}
				else
				if (link_index == sel_link)
				{
					color = COLOR_GREEN;
				}
			}

			editor_drawer.DrawLine(p1, p2, color);

			link_index++;
		}

		index++;
	}

	index = 0;
	for (auto& node : nodes)
	{
		int link_index = 0;
		for (auto& link : node.links)
		{
			Color color = COLOR_WHITE;

			if (index == sel_node)
			{
				if (sel_link == -1)
				{
					color = COLOR_CYAN;
				}
				else
				if (link_index == sel_link)
				{
					color = COLOR_CYAN;
				}
			}

			editor_drawer.DrawSprite(editor_drawer.arrow_tex, link.arrow_pos, linkSize, -0.5f * linkSize, link.angle, color);

			link_index++;
		}

		index++;
	}

	index = 0;

	for (auto& node : nodes)
	{
		Color color = COLOR_CYAN;

		if (index == def_node)
		{
			color = COLOR_MAGNETA;
		}

		if ((sel_node == index && sel_link == -1) || target_node == index)
		{
			color = COLOR_GREEN;
		}

		/*if (node.type == AnimNode)
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
		}*/

		editor_drawer.DrawSprite(editor_drawer.node_tex, node.pos, nodeSize, 0.0f, 0.0f, color);

		editor_drawer.PrintText(node.pos + Vector2(5.0f, 3.0f), COLOR_WHITE,node.name.c_str());

		editor_drawer.PrintText(node.pos + Vector2(5.0f, 25.0f), COLOR_WHITE, node.asset ? node.asset->GetName() : "null");

		if (index == def_node)
		{
			editor_drawer.PrintText(node.pos + Vector2(5.0f, 55.0f), COLOR_WHITE, "Start node");
		}

		index++;
	}

	if (sel_node != -1 && sel_link == -1)
	{
		Node& node = nodes[sel_node];

		if (node.asset)
		{
			state.looped = node.looped;
			state.reversed = node.reversed;

			trans_anim.pos = node.asset->trans.size * 0.5f;
			trans_anim.size = node.asset->trans.size;
			trans_anim.BuildMatrices();
			Sprite::UpdateFrame(&node.asset->sprite, &state, dt);
			Sprite::Draw(&trans_anim, COLOR_WHITE, &node.asset->sprite, &state, true, true);
		}
	}
}
#endif

bool SpriteGraphAsset::UsingOwnCamera()
{
	return true;
}

void SpriteGraphAsset::PrepareInstance(Instance* inst)
{
	if (def_node == -1 || !valid || inst->asset)
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
		if (node.pos.x < ms.x && ms.x < node.pos.x + nodeSize.x &&
			node.pos.y < ms.y && ms.y < node.pos.y + nodeSize.y)
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
		int prev_sel_node = sel_node;
		int prev_sel_link = sel_link;

		ShowProperties(false);
		nodes[prev_sel_node].links.erase(nodes[prev_sel_node].links.begin() + prev_sel_link);
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

	int prev_sel_node = sel_node;
	ShowProperties(false);
	nodes.erase(nodes.begin() + prev_sel_node);
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
			link->GetMetaData()->PrepareWidgets(editor.obj_cat);
		}
		else
		if (sel_node != -1)
		{
			Node* node = &nodes[sel_node];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->PrepareWidgets(editor.obj_cat);
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
	camera2d_pos = src_graph->camera2d_pos;
	camera2d_zoom = src_graph->camera2d_zoom;
	def_node = src_graph->def_node;

	nodes = src_graph->nodes;

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
	delta_ms /= Sprite::ed_cam_zoom;
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

		if (target_node != -1 && target_node != sel_node)
		{
			Node& node = nodes[target_node];

			if (node.pos.x > mouse_pos.x || mouse_pos.x > node.pos.x + nodeSize.x ||
				node.pos.y > mouse_pos.y || mouse_pos.y > node.pos.y + nodeSize.y)
			{
				target_node = -1;
			}
		}

		if ((target_node == sel_node) || (target_node != -1 && nodes[target_node].type == GroupNode))
		{
			target_node = -1;
		}
	}
}

void SpriteGraphAsset::OnLeftMouseDown(Vector2 ms)
{
	mouse_pos = Sprite::MoveFromCamera(ms, false);

	int prev_sel_node = sel_node;
	int prev_sel_link = sel_link;

	ShowProperties(false);

	sel_node = GetNodeIndex(mouse_pos);
	sel_link = -1;

	if (sel_node == -1)
	{
		int index = 0;
		for (auto& node : nodes)
		{
			if (node.links.size() > 0)
			{
				int link_index = 0;

				for (auto& link : node.links)
				{
					if (link.arrow_pos.x - linkSize.x < mouse_pos.x && mouse_pos.x < link.arrow_pos.x + linkSize.x &&
						link.arrow_pos.y - linkSize.y < mouse_pos.y && mouse_pos.y < link.arrow_pos.y + linkSize.y)
					{
						sel_node = index;
						sel_link = link_index;
					}

					link_index++;
				}

			}

			index++;
		}
	}

	if (sel_node != -1 && sel_link == -1)
	{
		state.cur_time = -1.0f;
		drag = MoveNode;
	}

	if (sel_node != -1 && core.controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
	{
		drag = AddLink;
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

			sel_link = (int)nodes[sel_node].links.size() - 1;

			ShowProperties(true);
		}

		target_node = -1;
	}

	drag = None;
}

void SpriteGraphAsset::OnRightMouseDown(Vector2 ms)
{
	mouse_pos = ms;

	editor.popup_menu->StartMenu(true);

	editor.popup_menu->StartSubMenu("Create");
	editor.popup_menu->AddItem(5000, "Anim Node");
	//popup_menu->AddItem(5001, "Logic Node");
	//popup_menu->AddItem(5002, "Group Node");
	editor.popup_menu->EndSubMenu();

	editor.popup_menu->AddItem(5004, "Make Node as Default");
	editor.popup_menu->AddItem(5005, "Delete", (sel_node != -1));

	editor.popup_menu->ShowAsPopup(editor.viewport, (int)ms.x, (int)ms.y);
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

void SpriteGraphAsset::OnDragObjectFromTreeView(bool is_assets_tree, SceneObject* object, Vector2 ms)
{
	if (!is_assets_tree)
	{
		return;
	}

	SpriteAsset* asset = dynamic_cast<SpriteAsset*>(object);
	
	if (!asset)
	{
		return;
	}

	ms = Sprite::MoveFromCamera(ms, false);

	for (auto& node : nodes)
	{
		if (node.pos.x < ms.x && ms.x < node.pos.x + nodeSize.x &&
			node.pos.y < ms.y && ms.y < node.pos.y + nodeSize.y)
		{
			node.asset = asset;
			node.object_uid = asset->GetUID();

			state.cur_time = -1.0f;

			break;
		}
	}
}

#endif