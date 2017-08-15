
#pragma once

#include "Services/Scene/SceneObject.h"
#include "AStar.h"

class PathFinder : public SceneObject
{
public:
	META_DATA_DECL(PathFinder)

	std::string world;

	Vec2 src;
	Vec2 dest;
	AStar astar;
	AStar::CoordinateList path;

	CLASSDECLDIF(SceneObject, PathFinder)
	PathFinder();
	virtual ~PathFinder();

	virtual void Init();
	virtual void ApplyProperties();
	void DrawBlock(int x, int y, Color color);
	void Draw(float dt);
};
