
#pragma once

#include "Services/Scene/SceneObject.h"
#include <list>

/**
\page scene_object_2D_NavMesh2D NavMesh2D

Currently 2D space is a special mode and this class represents a navigation mesh in that special 2D mode.
Navigation mesh is not autogenerated and should be created by hands.

NavMesh itself consist from nodes. Nodes can be linked. Every linked nodes that creates triangle ara counted as a part of NavMesh.

To add new node firstly scene object needed to be selected. After that press hot key 'O' to add new node in center of screen.
If another node is selected then pressing hot key 'P' will add another node of a sprite near previosly selected one. Press hot key 'I' to
delete selected node. Press hot key 'U' to delete all links of selected node.

To add link to node firstly select needed node and then hold key 'C' and then drag link to desired need.

To test constraction of past hold down key 'V' and click in needed point for set start point. Then hold down key 'B' and click in
needed point for set end point.

![Caption text](../images/NavMesh.jpg "Image")

To find path firstly NavMesh2D::GetPath should be called. If either start point or end point are outside of a navigation mesh then
closed point to navigation will be found and final path will contain this points as well. This method will return index of finded
path in the pool. To move along finded path NavMesh2D::MoveAlongPath should be called. NavMesh2D::GetPathCurPoint will return current
point in the path. NavMesh2D::RemovePath will remove path when it became unnecessary.

This class ::NavMesh2D is a representation on C++ side.

This class Script::NavMesh2D is a representation in a script.

Instance property parameters
----------------------------

Name              | Description
------------------| -------------
x                 | X coordinate of a position
y                 | Y coordinate of a position
links             | Array of links

Parameters
----------

Name              | Description
------------------| -------------
inst              | Array of nodes

*/


/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of navigation mesh in 2D space

Currently 2D space is a special mode and this class represents a navigation mesh in that special 2D mode.
Navigation mesh is not autogenerated and should be created by hands. More details can be found at \ref scene_object_2D_NavMesh2D "link".

To find path firstly NavMesh2D::GetPath should be called. If either start point or end point are outside of a navigation mesh then
closed point to navigation will be found and final path will contain this points as well. This method will return index of finded
path in the pool. To move along finded path NavMesh2D::MoveAlongPath should be called. NavMesh2D::GetPathCurPoint will return current
point in the path. NavMesh2D::RemovePath will remove path when it became unnecessary.

This class Script::NavMesh2D is a representation in a script.

*/

class NavMesh2D : public SceneObject
{
public:

#ifndef DOXYGEN_SKIP

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
		META_DATA_DECL_BASE(Link)

		Link() {};
		Link(int set_index)
		{
			index = set_index;
		}

		int index = -1;
	};

	struct Node
	{
		META_DATA_DECL_BASE(Node)

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
#endif

	/**
	\brief Find a path bettwen two points

	\param[in] x X coordinate of start point
	\param[in] y Y coordinate of start point
	\param[in] to_x X coordinate of point of destination
	\param[in] to_y Y coordinate of point of destination

	\return The index of finded path

	*/

	int GetPath(float x, float y, float to_x, float to_y);

	/**
	\brief Remove a path

	\param[in] index Index of a path

	*/

	void RemovePath(int index);

	/**
	\brief Moving along a path

	\param[in] index Index of a path
	\param[in] dist Distance at which point should be moved

	\return If a final point in the path was reached then 0 will be returned. Otherwise 1 will be returned.

	*/

	int MoveAlongPath(int index, float dist);

	/**
	\brief Getting current point in a path

	\param[in] index Index of a path
	\param[out] pos_x X coordinate of a current point
	\param[out] pos_y Y coordinate of a current point
	\param[out] angle An angle in radian of rotation


	\return If a final point in the a was reached then 0 will be returned. Otherwise 1 will be returned.

	*/
	void GetPathCurPoint(int index, float& pos_x, float& pos_y, float& angle);

#ifndef DOXYGEN_SKIP

#ifdef EDITOR
	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
	int  GetInstCount();
#endif

#endif
};
