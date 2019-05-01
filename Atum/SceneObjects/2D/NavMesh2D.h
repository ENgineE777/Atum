
#pragma once

#include "Services/Scene/SceneObject.h"
#include <list>

/**
\page scene_object_2D_NavMesh2D NavMesh2D

NavMesh2D

*/


/**
\ingroup gr_code_scene_objects_2D
*/

class NavMesh2D : public SceneObject
{
public:

	struct Polygon
	{
		struct Edge 
		{
			Vector2 point;
			int index = -1;
			Polygon* C = nullptr;
			int C_edge = -1;
		};

		std::vector<Edge> edges;

		int FindEdge(int index_a, int index_b)
		{
			for (int i = 0; i < 3; i++)
			{
				int next_index = i == 2 ? 0 : i + 1;

				if ((edges[i].index == index_a && edges[next_index].index == index_b) ||
					(edges[i].index == index_b && edges[next_index].index == index_a))
				{
					return i;
				}
			}

			return -1;
		}

		Vector2 center;
		Vector2 entry;

		float distance;
		int prev_edge;

		bool clockwise;
	};

	struct NavMesh
	{
		std::vector<Polygon> polygons;
	};

	NavMesh nav_mesh;

	META_DATA_DECL(NavMesh2D)

	Transform2D trans;

	struct Link
	{
		META_DATA_DECL(Link)

		Link() {};
		Link(int set_index)
		{
			index = set_index;
		}

		int index = -1;
	};

	struct Node
	{
		META_DATA_DECL(Node)

		Vector2 pos = 0.0f;

		std::vector<Link> links;
	};

	int sel_inst = -1;
	int source_inst = -1;
	int target_inst = -1;
	bool link_mode = false;
	Vector2 target_pos;
	vector<Node> instances;

	Vector2 start_pt = 0.0f;
	Vector2 end_pt = 0.0f;

	struct PathPoint
	{
		Vector2 pos;
		float distance = 0.0f;
		float angle = 0.0f;

		PathPoint()
		{

		}

		PathPoint(Vector2 set_pos)
		{
			pos = set_pos;
		}
	};

	struct Path
	{
		bool used = false;
		bool finished = false;
		int cur_point = -1;
		float cur_dist = 0.0f;
		float point_dist = 0.0f;
		Vector2 cur_pos;
		float angle = 0.0f;
		std::vector<PathPoint> points;
	};

	vector<Path> pathes;

	virtual ~NavMesh2D() = default;

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;

	virtual void Draw(float dt);

	bool Play() override;

	void ConstructNavMesh();

	void FindPath(int index, const Vector2 &p_start, const Vector2 &p_end);
	Vector2 GetClosestPoint(const Vector2 &p_point);

	int  GetPath(float x, float y, float to_x, float to_y);
	void RemovePath(int index);
	int  MoveAlongPath(int index, float dist);
	void GetPathCurPoint(int index, float& pos_x, float& pos_y, float& angle);

#ifdef EDITOR
	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
	int  GetInstCount();
#endif
};
