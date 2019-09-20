
#pragma once

#include "Services/Scene/SceneObject.h"

class NavMesh2DBase : public SceneObject
{
public:

#ifndef DOXYGEN_SKIP

	struct Polygon
	{
		struct Edge 
		{
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

	std::vector<Polygon> polygons;
	std::vector<Vector2> vertices;

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

	virtual ~NavMesh2DBase() = default;

	void Init() override;

	void BindClassToScript() override;

	virtual void Draw(float dt) = 0;

	bool Play() override;

	virtual void ConstructNavMesh() = 0;

	static Vector2 GetClosestPointToSegment2d(const Vector2 &p_point, const Vector2 *p_segment);
	static bool IsPointInTriangle(const Vector2 &s, const Vector2 &a, const Vector2 &b, const Vector2 &c);

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
};
