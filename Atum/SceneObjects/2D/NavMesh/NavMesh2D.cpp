
#include "NavMesh2D.h"
#include "Services/Render/Render.h"

META_DATA_DESC(NavMesh2D::Link)
	INT_PROP(NavMesh2D::Link, index, 0, "Prop", "index", "index of linked node")
META_DATA_DESC_END()

META_DATA_DESC(NavMesh2D::Node)
	FLOAT_PROP(NavMesh2D::Node, pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
	FLOAT_PROP(NavMesh2D::Node, pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
	ARRAY_PROP(NavMesh2D::Node, links, Link, "Prop", "links")
META_DATA_DESC_END()

CLASSREG(SceneObject, NavMesh2D, "NavMesh2D")

META_DATA_DESC(NavMesh2D)
	BASE_SCENE_OBJ_PROP(NavMesh2D)
	ARRAY_PROP_INST_CALLGIZMO(NavMesh2D, instances, Node, "Prop", "inst", NavMesh2D, sel_inst, SetGizmo)
META_DATA_DESC_END()

void NavMesh2D::ConstructNavMesh()
{
	polygons.clear();

	vertices.resize(instances.size());

	for (int index = 0; index < instances.size(); index++)
	{
		auto inst_a = instances[index];

		vertices[index] = inst_a.pos;

		for (auto& link_a : inst_a.links)
		{
			auto inst_b = instances[link_a.index];

			for (auto& link_b : inst_b.links)
			{
				auto inst_c = instances[link_b.index];

				for (auto& link_c : inst_c.links)
				{
					if (link_c.index == index)
					{
						bool already_added = false;

						for (auto& p : polygons)
						{
							int flags = 0;

							for (int i = 0; i < 3; i++)
							{
								if (p.edges[i].index == index)
								{
									flags += 1;
								}

								if (p.edges[i].index == link_a.index)
								{
									flags += 2;
								}

								if (p.edges[i].index == link_b.index)
								{
									flags += 4;
								}
							}

							if (flags == 7)
							{
								already_added = true;
								break;
							}
						}

						if (!already_added)
						{
							polygons.push_back(Polygon());
							Polygon& polygon = polygons.back();

							polygon.edges.resize(3);

							polygon.edges[0].index = index;
							polygon.edges[1].index = link_a.index;
							polygon.edges[2].index = link_b.index;

							float area = 0;

							Vector2 points[3];
							points[0] = inst_a.pos;
							points[1] = inst_b.pos;
							points[2] = inst_c.pos;

							for (int i = 0; i < 3; i++)
							{
								Vector2 p1 = points[i];
								Vector2 p2 = points[i==2 ? 0 : i + 1];

								area += (p2.x - p1.x) * (p2.y + p1.y);
							}

							polygon.clockwise = area > 0.0f;

							polygon.center = Vector2((inst_a.pos.x + inst_b.pos.x + inst_c.pos.x) / 3.0f, (inst_a.pos.y + inst_b.pos.y + inst_c.pos.y) / 3.0f);
						}
					}
				}
			}
		}
	}

	for (int index = 0; index < polygons.size() - 1; index++)
	{
		auto& polygon_a = polygons[index];

		for (int index2 = index + 1; index2 < polygons.size(); index2++)
		{
			auto& polygon_b = polygons[index2];

			for (int i = 0; i < 3; i++)
			{
				int next_index = i == 2 ? 0 : i + 1;

				int edge_index = polygon_b.FindEdge(polygon_a.edges[i].index, polygon_a.edges[next_index].index);

				if (edge_index != -1)
				{
					polygon_a.edges[i].C = &polygons[index2];
					polygon_a.edges[i].C_edge = edge_index;

					polygon_b.edges[edge_index].C = &polygons[index];
					polygon_b.edges[edge_index].C_edge = i;
				}
			}
		}
	}
}

void NavMesh2D::ApplyProperties()
{
	ConstructNavMesh();
}

void NavMesh2D::Draw(float dt)
{ 
	if (GetScene()->Playing() || GetState() == Invisible)
	{
		return;
	}

	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

#ifdef EDITOR
	if (core.controls.DebugKeyPressed("KEY_M", Controls::Activated))
	{
		ConstructNavMesh();
	}

	if (edited)
	{
		{
			if (sel_inst != -1)
			{
				bool need_delete_links = core.controls.DebugKeyPressed("KEY_U");
				bool need_delete_node = core.controls.DebugKeyPressed("KEY_I");

				if (need_delete_links || need_delete_node)
				{
					Node& inst = instances[sel_inst];

					for (int i = 0; i < inst.links.size(); i++)
					{
						Node& linked_inst = instances[inst.links[i].index];

						for (int j = 0; j < linked_inst.links.size(); j++)
						{
							if (linked_inst.links[j].index == sel_inst)
							{
								linked_inst.links.erase(linked_inst.links.begin() + j);
								break;
							}
						}
					}

					inst.links.clear();
				}

				if (need_delete_node)
				{
					for (auto& inst : instances)
					{
						for (auto& link : inst.links)
						{
							if (link.index > sel_inst)
							{
								link.index--;
							}
						}
					}

					for (auto comp : components)
					{
						comp->InstDeleted(sel_inst);
					}

					instances.erase(sel_inst + instances.begin());
					sel_inst = -1;
					SetGizmo();
				}
			}

			bool add_center = core.controls.DebugKeyPressed("KEY_P");
			bool add_copy = core.controls.DebugKeyPressed("KEY_O");

			if (add_center || (add_copy && sel_inst != -1))
			{
				Node inst;

				if (add_copy)
				{
					inst = instances[sel_inst];
					inst.pos = instances[sel_inst].pos + 20.0f;
				}
				else
				{
					float scale = 1024.0f / core.render.GetDevice()->GetHeight();
					inst.pos = { Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale };
				}

				instances.push_back(inst);

				for (auto comp : components)
				{
					comp->InstAdded(add_copy ? sel_inst : -1);
				}

				sel_inst = (int)instances.size() - 1;


				SetGizmo();
			}
		}
	}

	if (link_mode)
	{
		Node& inst = instances[source_inst];

		Vector2 p1 = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		core.render.DebugLine2D(p1, COLOR_WHITE, target_pos, COLOR_WHITE);
	}

	{
		Vector2 p1 = start_pt * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		core.render.DebugSprite(nullptr, p1 - 10.0f, 20.0f, COLOR_MAGNETA);
	}

	{
		Vector2 p1 = end_pt * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		core.render.DebugSprite(nullptr, p1 - 10.0f, 20.0f, COLOR_RED);
	}

#endif

	for (int i = 0; i<instances.size(); i++)
	{
		Node& inst = instances[i];

		Vector2 p1 = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		core.render.DebugSprite(nullptr, p1 - 5.0f, 10.0f, (i == source_inst || i == target_inst) ? COLOR_YELLOW : COLOR_GREEN);

		for (int j = 0; j < inst.links.size(); j++)
		{
			Vector2 p2 = instances[inst.links[j].index].pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

			core.render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);
		}
	}

	/*for (auto& p : nav_mesh.polygons)
	{
		Vector2 p1 = p.edges[0].point * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		Vector2 p2 = p.edges[1].point * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		Vector2 p3 = p.edges[2].point * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);
		render.DebugLine2D(p2, COLOR_WHITE, p3, COLOR_WHITE);
		render.DebugLine2D(p3, COLOR_WHITE, p1, COLOR_WHITE);

		Vector2 pc = p.center * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, pc - 10.0f, 20.0f, COLOR_YELLOW);

		for (int i = 0; i < 3; i++)
		{
			if (p.edges[i].C)
			{
				Vector2 pce = p.edges[i].C->center * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
				render.DebugLine2D(pc, COLOR_CYAN, pce, COLOR_CYAN);
			}
		}
	}*/


	MoveAlongPath(0, 150.0f * dt);

	{
		Vector2 p1 = pathes[0].cur_pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		core.render.DebugSprite(nullptr, p1 - 5.0f, 10.0f, COLOR_RED);
	}

	for (int i = 0; i < (int)pathes[0].points.size() - 1; i++)
	{
		Vector2 p1 = pathes[0].points[i].pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
		Vector2 p2 = pathes[0].points[i + 1].pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

		core.render.DebugSprite(nullptr, p2 - 5.0f, 10.0f, COLOR_MAGNETA);

		core.render.DebugLine2D(p1, COLOR_YELLOW, p2, COLOR_YELLOW);
		p1.x -= 1;
		p2.x -= 1;
		p1.y -= 1;
		p2.y -= 1;

		core.render.DebugLine2D(p1, COLOR_YELLOW, p2, COLOR_YELLOW);
	}
}

#ifdef EDITOR
void NavMesh2D::OnMouseMove(Vector2 delta_ms)
{
	if (link_mode)
	{
		target_pos += delta_ms;

		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

		target_inst = -1;

		for (int i = 0; i < instances.size(); i++)
		{
			Node& inst = instances[i];

			Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

			if (pos.x - 10.0f< target_pos.x && target_pos.x < pos.x + 10.0f &&
				pos.y - 10.0f< target_pos.y && target_pos.y < pos.y + 10.0f)
			{
				target_inst = i;

				break;
			}
		}
	}

}

void NavMesh2D::OnLeftMouseDown(Vector2 ms)
{
	if (core.controls.DebugKeyPressed("KEY_V", Controls::Active))
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
		start_pt = (ms + Sprite::ed_cam_pos - Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f) / scale;
	}

	if (core.controls.DebugKeyPressed("KEY_B", Controls::Active))
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
		end_pt = (ms + Sprite::ed_cam_pos - Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f) / scale;

		pathes[0].used = false;
		GetPath(start_pt.x, start_pt.y, end_pt.x, end_pt.y);
	}

	if (core.controls.DebugKeyPressed("KEY_C", Controls::Active))
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

		source_inst = -1;

		for (int i = 0; i < instances.size(); i++)
		{
			Node& inst = instances[i];

			Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

			if (pos.x - 10.0f< ms.x && ms.x < pos.x + 10.0f &&
				pos.y - 10.0f< ms.y && ms.y < pos.y + 10.0f)
			{
				source_inst = i;

				break;
			}
		}

		if (source_inst != -1)
		{
			sel_inst = -1;
			SetGizmo();

			link_mode = true;
			target_inst = -1;
			target_pos = ms;
		}
	}
}

void NavMesh2D::OnLeftMouseUp()
{
	if (link_mode)
	{
		if (source_inst != -1 && target_inst != -1)
		{
			Node& source = instances[source_inst];

			bool found = false;

			for (int i = 0; i < source.links.size(); i++)
			{
				if (source.links[i].index == target_inst)
				{
					found = true;
				}
			}

			if (!found)
			{
				instances[source_inst].links.push_back(Link(target_inst));
				instances[target_inst].links.push_back(Link(source_inst));
			}
		}

		link_mode = false;
		source_inst = -1;
		target_inst = -1;
	}
}

bool NavMesh2D::CheckSelection(Vector2 ms)
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Node& inst = instances[i];

		Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

			if (pos.x - 10.0f< ms.x && ms.x < pos.x + 10.0f &&
				pos.y - 10.0f< ms.y && ms.y < pos.y + 10.0f)
		{
			sel_inst = i;

			if (edited)
			{
				SetGizmo();
			}

			return true;
		}
	}

	return false;
}

void NavMesh2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		SetGizmo();
	}
}

void NavMesh2D::SetGizmo()
{
	if (sel_inst != -1)
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
		trans.size = 60.0f / scale;

		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(&instances[sel_inst].pos, &trans.size, &trans.offset), Gizmo::trans_2d_move);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}

int NavMesh2D::GetInstCount()
{
	return (int)instances.size();
}
#endif
