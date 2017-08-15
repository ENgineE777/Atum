#include "AStar.h"
#include <algorithm>

AStar::Node::Node(Vec2 set_pos, Node* set_parent)
{
	pos = set_pos;
	parent = set_parent;
	G = H = 0;
}

int AStar::Node::GetScore()
{
	return G + H;
}

AStar::AStar()
{
	direction = { { 0, 1 },{ 1, 0 },{ 0, -1 },{ -1, 0 },
	{ -1, -1 },{ 1, 1 },{ -1, 1 },{ 1, -1 } };
	field = nullptr;
	visited = nullptr;

	rawNodes = (char*)0x10;
	ptrNodes = rawNodes + RawSize;
}

AStar::~AStar()
{
	if (field)
	{
		free(field);
	}

	if (visited)
	{
		free(visited);
	}
}

void AStar::LoadWorld(const char* name, Vec2& set_worldSize)
{
	if (field)
	{
		free(field);
	}

	worldSize = set_worldSize;

	FILE* f = fopen(name, "r");

	if (!f)
	{
		return;
	}

	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	field = (char*)malloc(sz);
	fread(field, sz, 1, f);

	fclose(f);

	visited = (char*)malloc(sz);
}

AStar::Node* AStar::AllocNode(Vec2 pos, Node* parent)
{
	if ((ptrNodes - rawNodes) == RawSize)
	{
		rawNodes = (char*)malloc(RawSize);
		ptrNodes = rawNodes;

		rawNodesVec.push_back(rawNodes);
	}

	Node* node = (Node*)ptrNodes;
	ptrNodes += sizeof(Node);

	node->pos = pos;
	node->parent = parent;
	node->G = node->H = 0;

	return node;
}

bool AStar::FindPath(AStar::CoordinateList& path, Vec2& src, Vec2& dest)
{
	path.clear();

	if (!field)
	{
		return false;
	}

	memset(visited, 0, sz);

	Node *current = nullptr;
	NodeSet openSet;
	openSet.insert(AllocNode(src));

	while (!openSet.empty())
	{
		current = *openSet.begin();

		for (auto node : openSet)
		{
			if (node->GetScore() <= current->GetScore())
			{
				current = node;
			}
		}

		if (current->pos == dest)
		{
			break;
		}

		visited[current->pos.y * (worldSize.x + 1) + current->pos.x] = 1;

		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		for (int i = 0; i < DirectionCount; i++)
		{
			Vec2 pos(current->pos + direction[i]);

			if (DetectCollision(pos) ||
				visited[pos.y * (worldSize.x + 1) + pos.x])
			{
				continue;
			}

			int totalCost = current->G + ((i < 4) ? 10 : 14);

			Node *successor = FindNodeOnList(openSet, pos);

			if (successor == nullptr)
			{
				successor = AllocNode(pos, current);

				successor->G = totalCost;
				successor->H = GetHeuristic(successor->pos, dest);
				openSet.insert(successor);
			}
			else
				if (totalCost < successor->G)
				{
					successor->parent = current;
					successor->G = totalCost;
				}
		}
	}

	if (current)
	{
		if (current->pos == dest)
		{
			while (current != nullptr)
			{
				path.push_back(current->pos);
				current = current->parent;
			}
		}
	}

	for (auto chunk : rawNodesVec)
	{
		free(chunk);
	}

	rawNodesVec.clear();

	return path.size() > 0;
}

AStar::Node* AStar::FindNodeOnList(NodeSet& nodes, Vec2 pos)
{
	for (auto node : nodes)
	{
		if (node->pos == pos)
		{
			return node;
		}
	}

	return nullptr;
}

int AStar::GetHeuristic(Vec2& src, Vec2& dest)
{
	auto delta = std::move(src.GetDelta(dest));
	return 10 * (delta.x + delta.y) + (-6) * std::min(delta.x, delta.y);
}

bool AStar::DetectCollision(Vec2& pos)
{
	if (pos.x < 0 || pos.x >= worldSize.x ||
		pos.y < 0 || pos.y >= worldSize.y)
	{
		return true;
	}

	if (field && field[pos.y * (worldSize.x + 1) + pos.x] == '*')
	{
		return true;
	}

	return false;
}