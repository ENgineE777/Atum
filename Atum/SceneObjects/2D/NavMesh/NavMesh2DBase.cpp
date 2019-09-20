
#include "NavMesh2DBase.h"
#include "Services/Render/Render.h"
#include <list>

Vector2 NavMesh2DBase::GetClosestPointToSegment2d(const Vector2 &p_point, const Vector2 *p_segment)
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

bool NavMesh2DBase::IsPointInTriangle(const Vector2 &s, const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
	Vector2 an = a - s;
	Vector2 bn = b - s;
	Vector2 cn = c - s;

	bool orientation = an.Cross(bn) > 0;

	if ((bn.Cross(cn) > 0) != orientation) return false;

	return (cn.Cross(an) > 0) == orientation;
}

void NavMesh2DBase::FindPath(int index, const Vector2 &p_start, const Vector2 &p_end)
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
	for (auto& p : polygons)
	{
		if (begin_d || end_d)
		{
			for (int i = 2; i < p.edges.size(); i++)
			{
				if (begin_d > 0)
				{
					if (IsPointInTriangle(p_start, vertices[p.edges[0].index], vertices[p.edges[i - 1].index], vertices[p.edges[i].index]))
					{
						begin_poly = &p;
						begin_point = p_start;
						begin_d = 0;

						if (end_d == 0) break;
					}
				}

				if (end_d > 0)
				{
					if (IsPointInTriangle(p_end, vertices[p.edges[0].index], vertices[p.edges[i - 1].index], vertices[p.edges[i].index]))
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
		for (auto& p : polygons)
		{
			int es = (int)p.edges.size();

			for (int i = 0; i < es; i++)
			{
				Vector2 edge[2] = { vertices[p.edges[i].index], vertices[p.edges[(i + 1) % es].index] };

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
				left = vertices[p->edges[prev].index];
				right = vertices[p->edges[prev_n].index];

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

Vector2 NavMesh2DBase::GetClosestPoint(const Vector2 &p_point)
{
	Vector2 closest_point = Vector2();
	float closest_point_d = FLT_MAX;

	for (auto& p : polygons)
	{
		for (int i = 2; i < p.edges.size(); i++)
		{
			if (IsPointInTriangle(p_point, vertices[p.edges[0].index], vertices[p.edges[i - 1].index], vertices[p.edges[i].index]))
			{
				return p_point;
			}
		}
	}

	for (auto& p : polygons)
	{
		int es = (int)p.edges.size();

		for (int i = 0; i < es; i++)
		{
			Vector2 edge[2] = { vertices[p.edges[i].index], vertices[p.edges[(i + 1) % es].index] };

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

void NavMesh2DBase::Init()
{
	pathes.resize(32);

	RenderTasks(false)->AddTask(ExecuteLevels::PostProcess, this, (Object::Delegate)&NavMesh2DBase::Draw);
}

bool NavMesh2DBase::Play()
{
	ConstructNavMesh();

	return true;
}

int NavMesh2DBase::GetPath(float x, float y, float to_x, float to_y)
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

void NavMesh2DBase::RemovePath(int index)
{
	pathes[index].used = false;
}

int NavMesh2DBase::MoveAlongPath(int index, float dist)
{
	Path& path = pathes[index];

	if (!path.used || path.finished || path.points.size() == 0)
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

void NavMesh2DBase::GetPathCurPoint(int index, float& pos_x, float& pos_y, float& angle)
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

void NavMesh2DBase::BindClassToScript()
{
	const char* brief = "Representation of camera in 2D space\n"
		"\n"
		"Currently 2D space is a special mode and this class represents a navigation mesh in that special 2D mode.\n"
		"Navigation mesh is not autogenerated and should be created by hands.More details can be found at \ref scene_object_2D_NavMesh2D \"link\".\n"
		"\n"
		"Currently 2D space is a special mode and this class represents a navigation mesh in that special 2D mode.\n"
		"closed point to navigation will be found and final path will contain this points as well.This method will return index of finded\n"
		"path in the pool.To move along finded path NavMesh2D::MoveAlongPath should be called.NavMesh2D::GetPathCurPoint will return current\n"
		"point in the path.NavMesh2D::RemovePath will remove path when it became unnecessary.\n"
		"\n"
		"This class ::NavMesh2D is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(NavMesh2DBase, brief)

	brief = "Find a path bettwen two points\n"
		"\n"
		"\\param[in] x X coordinate of start point\n"
		"\\param[in] y Y coordinate of start point\n"
		"\\param[in] to_x X coordinate of point of destination\n"
		"\\param[in] to_y Y coordinate of point of destination\n"
		"\n"
		"\\return The index of finded path";

	core.scripts.RegisterObjectMethod(script_class_name, "int GetPath(float x, float y, float to_x, float to_y)", WRAP_MFN(NavMesh2DBase, GetPath), brief);

	brief = "Remove a path\n"
		"\n"
		"\\param[in] index Index of a path\n";

	core.scripts.RegisterObjectMethod(script_class_name, "void RemovePath(int index)", WRAP_MFN(NavMesh2DBase, RemovePath), brief);

	brief = "Moving along a path\n"
		"\n"
		"\\param[in] index Index of a path\n"
		"\\param[in] dist Distance at which point should be moved\n"
		"\n"
		"\\return If a final point in the path was reached then 0 will be returned.Otherwise 1 will be returned.";

	core.scripts.RegisterObjectMethod(script_class_name, "int MoveAlongPath(int index, float dist)", WRAP_MFN(NavMesh2DBase, MoveAlongPath), brief);

	brief = "Getting current point in a path\n"
		"\n"
		"\\param[in] index Index of a path\n"
		"\\param[out] pos_x X coordinate of a current point\n"
		"\\param[out] pos_y Y coordinate of a current point\n"
		"\\param[out] angle An angle in radian of rotation\n"
		"\n"
		"\\return If a final point in the a was reached then 0 will be returned.Otherwise 1 will be returned.\n";

	core.scripts.RegisterObjectMethod(script_class_name, "void GetPathCurPoint(int index, float&out pos_x, float&out pos_y, float&out angle)", WRAP_MFN(NavMesh2DBase, GetPathCurPoint), brief);
}
