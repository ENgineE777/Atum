
#include "PathFinder.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, PathFinder, "PathFinder")

META_DATA_DESC(PathFinder)
BASE_SCENE_OBJ_PROP(PathFinder)
FILENAME_PROP(PathFinder, world, "", "Common", "World")
META_DATA_DESC_END()

PathFinder::PathFinder() : SceneObject()
{
}

PathFinder::~PathFinder()
{
}

void PathFinder::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&PathFinder::Draw);
}

void PathFinder::ApplyProperties()
{
	src = Vec2(20, 20);

	//dest = Vec2(75, 89);
	dest = Vec2(120, 10);

	astar.LoadWorld(world.c_str(), Vec2(126, 126));

	astar.FindPath(path, src, dest);
}

void PathFinder::DrawBlock(int x, int y, Color color)
{
	Matrix tr;
	tr.Pos() = Vector(x * 0.1f, 0, (astar.worldSize.y - y) * 0.1f);
	render.DebugBox(tr, color, Vector(0.1f));
}

void PathFinder::Draw(float dt)
{
	for (auto& pos : path)
	{
		DrawBlock(pos.x, pos.y, COLOR_GREEN);
	}

	for (int y = 0; y < astar.worldSize.y; y++)
	{
		for (int x = 0; x < astar.worldSize.x; x++)
		{
			if (astar.field[y * (astar.worldSize.x + 1) + x] == '*')
			{
				DrawBlock(x, y, COLOR_RED);
			}
		}
	}

	DrawBlock(src.x, src.y, COLOR_CYAN);
	DrawBlock(dest.x, dest.y, COLOR_CYAN);

	Matrix tr;
	tr.Pos() = Vector((float)astar.worldSize.x * 0.05f,-0.05f, (float)astar.worldSize.y * 0.05f);
	render.DebugBox(tr, COLOR_WHITE, Vector((float)astar.worldSize.x * 0.1f, 0.1f, (float)astar.worldSize.y * 0.1f));
}