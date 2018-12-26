
#include "NavMesh2D.h"
#include "Services/Render/Render.h"

META_DATA_DESC(NavMesh2D::Link)
	INT_PROP(NavMesh2D::Link, index, 0, "Prop", "index")
META_DATA_DESC_END()

META_DATA_DESC(NavMesh2D::Node)
	FLOAT_PROP(NavMesh2D::Node, pos.x, 0.0f, "Prop", "x")
	FLOAT_PROP(NavMesh2D::Node, pos.y, 0.0f, "Prop", "y")
	ARRAY_PROP(NavMesh2D::Node, links, Link, "Prop", "links")
META_DATA_DESC_END()

CLASSREG(SceneObject, NavMesh2D, "NavMesh2D")

META_DATA_DESC(NavMesh2D)
	BASE_SCENE_OBJ_PROP(NavMesh2D)
	ARRAY_PROP_INST(NavMesh2D, instances, Node, "Prop", "inst", NavMesh2D, sel_inst)
META_DATA_DESC_END()

Vector2 GetClosestPointToSegment2d(const Vector2 &p_point, const Vector2 *p_segment)
{
	Vector2 p = p_point - p_segment[0];
	Vector2 n = p_segment[1] - p_segment[0];
	float l = n.Length();
	if (l < 1e-10)
		return p_segment[0]; // both points are the same, just give any
	n /= l;

	float d = n.Dot(p);

	if (d <= 0.0)
		return p_segment[0]; // before first point
	else if (d >= l)
		return p_segment[1]; // after first point
	else
		return p_segment[0] + n * d; // inside
}

bool IsPointInTriangle(const Vector2 &s, const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
	Vector2 an = a - s;
	Vector2 bn = b - s;
	Vector2 cn = c - s;

	bool orientation = an.Cross(bn) > 0;

	if ((bn.Cross(cn) > 0) != orientation) return false;

	return (cn.Cross(an) > 0) == orientation;
}

void NavMesh2D::FindPath(int index, const Vector2 &p_start, const Vector2 &p_end)
{
	Path& path = pathes[index];
	path.finished = false;

	path.points.clear();

	Polygon *begin_poly = nullptr;
	Polygon *end_poly = nullptr;
	Vector2 begin_point;
	Vector2 end_point;
	float begin_d = FLT_MAX;
	float end_d = FLT_MAX;

	//look for point inside triangle
	for (auto& p : nav_mesh.polygons)
	{
		if (begin_d || end_d)
		{
			for (int i = 2; i < p.edges.size(); i++)
			{
				if (begin_d > 0)
				{
					if (IsPointInTriangle(p_start, p.edges[0].point, p.edges[i - 1].point, p.edges[i].point))
					{
						begin_poly = &p;
						begin_point = p_start;
						begin_d = 0;

						if (end_d == 0) break;
					}
				}

				if (end_d > 0)
				{
					if (IsPointInTriangle(p_end, p.edges[0].point, p.edges[i - 1].point, p.edges[i].point))
					{
						end_poly = &p;
						end_point = p_end;
						end_d = 0;

						if (begin_d == 0) break;
					}
				}
			}
		}

		p.prev_edge = -1;
	}

	//start or end not inside triangle.. look for closest segment :|
	if (begin_d || end_d)
	{
		for (auto& p : nav_mesh.polygons)
		{
			int es = (int)p.edges.size();

			for (int i = 0; i < es; i++)
			{
				Vector2 edge[2] = { p.edges[i].point, p.edges[(i + 1) % es].point };

				if (begin_d > 0)
				{
					Vector2 spoint = GetClosestPointToSegment2d(p_start, edge);

					float d = spoint.Length(p_start);

					if (d < begin_d)
					{
						begin_poly = &p;
						begin_point = spoint;
						begin_d = d;
					}
				}

				if (end_d > 0)
				{
					Vector2 spoint = GetClosestPointToSegment2d(p_end, edge);
					float d = spoint.Length(p_end);
					if (d < end_d)
					{
						end_poly = &p;
						end_point = spoint;
						end_d = d;
					}
				}
			}
		}
	}

	if (!begin_poly || !end_poly)
	{
		path.finished = true;

		return;
	}

	if (begin_poly == end_poly)
	{
		path.points.resize(2);
		path.points[0].pos = begin_point;
		path.points[1].pos = end_point;

		return;
	}

	bool found_route = false;

	std::list<Polygon*> open_list;

	begin_poly->entry = p_start;

	for (int i = 0; i < begin_poly->edges.size(); i++)
	{
		if (begin_poly->edges[i].C)
		{

			begin_poly->edges[i].C->prev_edge = begin_poly->edges[i].C_edge;
#ifdef USE_ENTRY_POINT
			Vector2 edge[2] = {
				_get_vertex(begin_poly->edges[i].point),
				_get_vertex(begin_poly->edges[(i + 1) % begin_poly->edges.size()].point)
			};

			Vector2 entry = Geometry::get_closest_point_to_segment_2d(begin_poly->entry, edge);
			begin_poly->edges[i].C->distance = begin_poly->entry.distance_to(entry);
			begin_poly->edges[i].C->entry = entry;
#else
			begin_poly->edges[i].C->distance = begin_poly->center.Length(begin_poly->edges[i].C->center);
#endif
			open_list.push_back(begin_poly->edges[i].C);

			if (begin_poly->edges[i].C == end_poly)
			{
				found_route = true;
			}
		}
	}

	while (!found_route)
	{
		if (open_list.size() == 0)
		{
			break;
		}

		//check open list

		Polygon* least_cost_poly = nullptr;
		float least_cost = FLT_MAX;

		for (auto& p : open_list)
		{
			float cost = p->distance;

#ifdef USE_ENTRY_POINT
			int es = p->edges.size();

			float shortest_distance = 1e30;

			for (int i = 0; i < es; i++) {
				Polygon::Edge &e = p->edges.write[i];

				if (!e.C)
					continue;

				Vector2 edge[2] = {
					_get_vertex(p->edges[i].point),
					_get_vertex(p->edges[(i + 1) % es].point)
				};

				Vector2 edge_point = Geometry::get_closest_point_to_segment_2d(p->entry, edge);
				float dist = p->entry.distance_to(edge_point);
				if (dist < shortest_distance)
					shortest_distance = dist;
			}

			cost += shortest_distance;
#else
			cost += p->center.Length(end_point);
#endif
			if (cost < least_cost)
			{
				least_cost_poly = p;
				least_cost = cost;
			}
		}

		Polygon *p = least_cost_poly;
		//open the neighbours for search
		int es = (int)p->edges.size();

		for (int i = 0; i < es; i++)
		{
			Polygon::Edge &e = p->edges[i];

			if (!e.C)
			{
				continue;
			}

#ifdef USE_ENTRY_POINT
			Vector2 edge[2] = {
				_get_vertex(p->edges[i].point),
				_get_vertex(p->edges[(i + 1) % es].point)
			};

			Vector2 edge_entry = Geometry::get_closest_point_to_segment_2d(p->entry, edge);
			float distance = p->entry.distance_to(edge_entry) + p->distance;

#else

			float distance = p->center.Length(e.C->center) + p->distance;

#endif

			if (e.C->prev_edge != -1)
			{
				//oh this was visited already, can we win the cost?

				if (e.C->distance > distance)
				{
					e.C->prev_edge = e.C_edge;
					e.C->distance = distance;
#ifdef USE_ENTRY_POINT
					e.C->entry = edge_entry;
#endif
				}
			}
			else
			{
				//add to open neighbours

				e.C->prev_edge = e.C_edge;
				e.C->distance = distance;
#ifdef USE_ENTRY_POINT
				e.C->entry = edge_entry;
#endif

				open_list.push_back(e.C);

				if (e.C == end_poly)
				{
					//oh my reached end! stop algorithm
					found_route = true;
					break;
				}
			}
		}

		if (found_route)
		{
			break;
		}

		open_list.remove(least_cost_poly);
	}

	if (found_route)
	{
		//string pulling

		Vector2 apex_point = end_point;
		Vector2 portal_left = apex_point;
		Vector2 portal_right = apex_point;
		Polygon *left_poly = end_poly;
		Polygon *right_poly = end_poly;
		Polygon *p = end_poly;

		while (p)
		{
			Vector2 left;
			Vector2 right;

			//#define CLOCK_TANGENT(m_a,m_b,m_c) ( ((m_a)-(m_c)).cross((m_a)-(m_b)) )
#define CLOCK_TANGENT(m_a, m_b, m_c) ((((m_a).x - (m_c).x) * ((m_b).y - (m_c).y) - ((m_b).x - (m_c).x) * ((m_a).y - (m_c).y)))

			if (p == begin_poly)
			{
				left = begin_point;
				right = begin_point;
			}
			else
			{
				int prev = p->prev_edge;
				int prev_n = (p->prev_edge + 1) % p->edges.size();
				left = p->edges[prev].point;
				right = p->edges[prev_n].point;

				if (p->clockwise)
				{
					Vector2 tmp = left;
					left = right;
					right = tmp;
				}
			}

			bool skip = false;

			if (CLOCK_TANGENT(apex_point, portal_left, left) >= 0)
			{
				//process
				if (portal_left.Length2(apex_point) < 0.01f || CLOCK_TANGENT(apex_point, left, portal_right) > 0)
				{
					left_poly = p;
					portal_left = left;
				}
				else
				{
					apex_point = portal_right;
					p = right_poly;
					left_poly = p;
					portal_left = apex_point;
					portal_right = apex_point;
					if (!path.points.size() || path.points.back().pos.Length(apex_point) > 0.01f)
					{
						path.points.push_back(apex_point);
					}
					skip = true;
				}
			}

			if (!skip && CLOCK_TANGENT(apex_point, portal_right, right) <= 0)
			{
				//process
				if (portal_right.Length2(apex_point) < 0.01f || CLOCK_TANGENT(apex_point, right, portal_left) < 0)
				{
					right_poly = p;
					portal_right = right;
				}
				else
				{
					apex_point = portal_left;
					p = left_poly;
					right_poly = p;
					portal_right = apex_point;
					portal_left = apex_point;
					if (!path.points.size() || path.points.back().pos.Length(apex_point) > 0.01f)
					{
						path.points.push_back(apex_point);
					}
				}
			}

			if (p != begin_poly)
			{
				p = p->edges[p->prev_edge].C;
			}
			else
			{
				p = nullptr;
			}
		}

		if (!path.points.size() || path.points.back().pos.Length2(begin_point) > 0.01f)
		{
			path.points.push_back(begin_point); // Add the begin point
		}
		else
		{
			path.points[path.points.size() - 1] = begin_point; // Replace first midpoint by the exact begin point
		}

		std::reverse(path.points.begin(), path.points.end());

		if (path.points.size() <= 1 || path.points.back().pos.Length2(end_point) > 0.01f)
		{
			path.points.push_back(end_point); // Add the end point
		}
		else
		{
			path.points.back() = end_point; // Replace last midpoint by the exact end point
		}

		return;
	}
}

Vector2 NavMesh2D::GetClosestPoint(const Vector2 &p_point)
{
	Vector2 closest_point = Vector2();
	float closest_point_d = FLT_MAX;

	for (auto& p : nav_mesh.polygons)
	{
		for (int i = 2; i < p.edges.size(); i++)
		{
			if (IsPointInTriangle(p_point, p.edges[0].point, p.edges[i - 1].point, p.edges[i].point))
			{
				return p_point;
			}
		}
	}

	for (auto& p : nav_mesh.polygons)
	{
		int es = (int)p.edges.size();

		for (int i = 0; i < es; i++)
		{
			Vector2 edge[2] = { p.edges[i].point, p.edges[(i + 1) % es].point };

			Vector2 spoint = GetClosestPointToSegment2d(p_point, edge);

			float d = spoint.Length2(p_point);

			if (d < closest_point_d)
			{

				closest_point = spoint;
				closest_point_d = d;
			}
		}
	}

	return closest_point;
}

void NavMesh2D::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::PostProcess, this, (Object::Delegate)&NavMesh2D::Draw);

	pathes.resize(32);
}

bool NavMesh2D::Play()
{
	return true;
}

void NavMesh2D::ConstructNavMesh()
{
	nav_mesh.polygons.clear();

	for (int index = 0; index < instances.size(); index++)
	{
		auto inst_a = instances[index];

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

						for (auto& p : nav_mesh.polygons)
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
							nav_mesh.polygons.push_back(Polygon());
							Polygon& polygon = nav_mesh.polygons.back();

							polygon.edges.resize(3);

							polygon.edges[0].point = inst_a.pos;
							polygon.edges[0].index = index;
							polygon.edges[1].point = inst_b.pos;
							polygon.edges[1].index = link_a.index;
							polygon.edges[2].point = inst_c.pos;
							polygon.edges[2].index = link_b.index;

							float area = 0;

							for (int i = 0; i < 3; i++)
							{
								Vector2 p1 = polygon.edges[i].point;
								Vector2 p2 = polygon.edges[i==2 ? 0 : i + 1].point;

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

	for (int index = 0; index < nav_mesh.polygons.size() - 1; index++)
	{
		auto& polygon_a = nav_mesh.polygons[index];

		for (int index2 = index + 1; index2 < nav_mesh.polygons.size(); index2++)
		{
			auto& polygon_b = nav_mesh.polygons[index2];

			for (int i = 0; i < 3; i++)
			{
				int next_index = i == 2 ? 0 : i + 1;

				int edge_index = polygon_b.FindEdge(polygon_a.edges[i].index, polygon_a.edges[next_index].index);

				if (edge_index != -1)
				{
					polygon_a.edges[i].C = &nav_mesh.polygons[index2];
					polygon_a.edges[i].C_edge = edge_index;

					polygon_b.edges[edge_index].C = &nav_mesh.polygons[index];
					polygon_b.edges[edge_index].C_edge = i;
				}
			}
		}
	}
}

int NavMesh2D::GetPath(float x, float y, float to_x, float to_y)
{
	int index = -1;

	for (int i = 0; i < pathes.size(); i++)
	{
		if (!pathes[i].used)
		{
			pathes[i].used = true;


			index = i;
			break;
		}
	}

	if (index == -1)
	{
		pathes.push_back(Path());
		index = (int)pathes.size() - 1;
	}

	FindPath(index, Vector2(x, y), Vector2(to_x, to_y));

	auto& path = pathes[index];

	for (int i = 1; i < path.points.size(); i++)
	{
		Vector2 dir = path.points[i].pos - path.points[i-1].pos;
		path.points[i].distance = dir.Length();
		path.points[i].angle = atan2(dir.y, dir.x);
	}
	
	path.cur_point = 0;
	path.cur_pos = Vector2(x, y);
	path.cur_dist = 0.0f;
	path.point_dist = 0.0f;
	path.angle = (path.points.size() > 1) ? path.points[0].angle : 0.0f;

	return index;
}

void NavMesh2D::RemovePath(int index)
{
	pathes[index].used = false;
}

int NavMesh2D::MoveAlongPath(int index, float dist)
{
	Path& path = pathes[index];

	if (!path.used || path.finished)
	{
		return 0;
	}

	if (dist < 0.001f)
	{
		return 1;
	}

	path.cur_dist += dist;

	while (path.cur_dist > path.point_dist)
	{
		path.cur_dist -= path.point_dist;

		path.cur_point++;

		if (path.cur_point >= path.points.size())
		{
			path.finished = true;
			path.cur_pos = path.points.back().pos;
			path.angle = path.points.back().angle;

			return 0;
		}
		else
		{
			path.point_dist = path.points[path.cur_point].distance;
		}
	}

	int p1 = 0;
	int p2 = 0;

	p1 = path.cur_point - 1;
	p2 = path.cur_point;

	path.angle = path.points[p2].angle;
	Vector2 dir = path.points[p2].pos - path.points[p1].pos;
	path.cur_pos = dir * path.cur_dist / path.point_dist + path.points[p1].pos;

	return 1;
}

void NavMesh2D::GetPathCurPoint(int index, float& pos_x, float& pos_y, float& angle)
{
	Path& path = pathes[index];

	if (!path.used)
	{
		return;
	}

	pos_x = path.cur_pos.x;
	pos_y = path.cur_pos.y;
	angle = path.angle;
}

void NavMesh2D::ApplyProperties()
{
	ConstructNavMesh();
}

void NavMesh2D::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(NavMesh2D)

	scripts.engine->RegisterObjectMethod(script_class_name, "int GetPath(float x, float y, float to_x, float to_y)", WRAP_MFN(NavMesh2D, GetPath), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void RemovePath(int index)", WRAP_MFN(NavMesh2D, RemovePath), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "int MoveAlongPath(int index, float dist)", WRAP_MFN(NavMesh2D, MoveAlongPath), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void GetPathCurPoint(int index, float&out pos_x, float&out pos_y, float&out angle)", WRAP_MFN(NavMesh2D, GetPathCurPoint), asCALL_GENERIC);
}

void NavMesh2D::Draw(float dt)
{ 
	if (owner->Playing() || GetState() == Invisible)
	{
		return;
	}

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

#ifdef EDITOR
	if (controls.DebugKeyPressed("KEY_M", Controls::Activated))
	{
		ConstructNavMesh();
	}

	if (edited)
	{
		{
			if (sel_inst != -1)
			{
				instances[sel_inst].pos = trans.pos;

				bool need_delete_links = controls.DebugKeyPressed("KEY_U");
				bool need_delete_node = controls.DebugKeyPressed("KEY_I");

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

			bool add_center = controls.DebugKeyPressed("KEY_O");
			bool add_after = controls.DebugKeyPressed("KEY_P");

			if (add_center || add_after)
			{
				Node inst;

				if (sel_inst != -1 && add_after)
				{
					inst.pos = instances[sel_inst].pos + 20.0f;
				}
				else
				{
					float scale = 1024.0f / render.GetDevice()->GetHeight();
					inst.pos = { Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale };
				}

				instances.push_back(inst);

				sel_inst = (int)instances.size() - 1;

				for (auto comp : components)
				{
					comp->InstAdded();
				}

				SetGizmo();
			}
		}
	}

	if (link_mode)
	{
		Node& inst = instances[source_inst];

		Vector2 p1 = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugLine2D(p1, COLOR_WHITE, target_pos, COLOR_WHITE);
	}

	{
		Vector2 p1 = start_pt * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, p1 - 10.0f, 20.0f, COLOR_MAGNETA);
	}

	{
		Vector2 p1 = end_pt * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, p1 - 10.0f, 20.0f, COLOR_RED);
	}

#endif

	for (int i = 0; i<instances.size(); i++)
	{
		Node& inst = instances[i];

		Vector2 p1 = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, p1 - 5.0f, 10.0f, (i == source_inst || i == target_inst) ? COLOR_YELLOW : COLOR_GREEN);

		for (int j = 0; j < inst.links.size(); j++)
		{
			Vector2 p2 = instances[inst.links[j].index].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

			render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);
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
		Vector2 p1 = pathes[0].cur_pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, p1 - 5.0f, 10.0f, COLOR_RED);
	}

	for (int i = 0; i < (int)pathes[0].points.size() - 1; i++)
	{
		Vector2 p1 = pathes[0].points[i].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		Vector2 p2 = pathes[0].points[i + 1].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		render.DebugSprite(nullptr, p2 - 5.0f, 10.0f, COLOR_MAGNETA);

		render.DebugLine2D(p1, COLOR_YELLOW, p2, COLOR_YELLOW);
		p1.x -= 1;
		p2.x -= 1;
		p1.y -= 1;
		p2.y -= 1;

		render.DebugLine2D(p1, COLOR_YELLOW, p2, COLOR_YELLOW);
	}

#ifdef EDITOR
	if (edited)
	{
		if (sel_inst != -1)
		{
			trans.pos = instances[sel_inst].pos;
			trans.BuildMatrices();
		}
	}
#endif
}

#ifdef EDITOR
void NavMesh2D::OnMouseMove(Vector2 delta_ms)
{
	if (link_mode)
	{
		target_pos += delta_ms;

		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		target_inst = -1;

		for (int i = 0; i < instances.size(); i++)
		{
			Node& inst = instances[i];

			Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

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
	if (controls.DebugKeyPressed("KEY_V", Controls::Active))
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;
		start_pt = (ms + Sprite::ed_cam_pos - Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f) / scale;
	}

	if (controls.DebugKeyPressed("KEY_B", Controls::Active))
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;
		end_pt = (ms + Sprite::ed_cam_pos - Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f) / scale;

		pathes[0].used = false;
		GetPath(start_pt.x, start_pt.y, end_pt.x, end_pt.y);
	}

	if (controls.DebugKeyPressed("KEY_C", Controls::Active))
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		source_inst = -1;

		for (int i = 0; i < instances.size(); i++)
		{
			Node& inst = instances[i];

			Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

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
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Node& inst = instances[i];

		Vector2 pos = inst.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

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
		float scale = render.GetDevice()->GetHeight() / 1024.0f;
		trans.size = 60.0f / scale;
		trans.pos = instances[sel_inst].pos;
	}

	Gizmo::inst->SetTrans2D(sel_inst != -1 ? &trans : nullptr, Gizmo::trans_2d_move | Gizmo::trans_2d_scale);

	if (sel_inst != -1)
	{
		Gizmo::inst->SetTrans2DWidgets(instances[sel_inst].GetMetaData()->GetFloatEditBox("x"), instances[sel_inst].GetMetaData()->GetFloatEditBox("y"),
		                               instances[sel_inst].GetMetaData()->GetFloatEditBox("width"), instances[sel_inst].GetMetaData()->GetFloatEditBox("height"));
	}
}

int NavMesh2D::GetInstCount()
{
	return (int)instances.size();
}
#endif
