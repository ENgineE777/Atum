
#pragma once

#include <vector>
#include <functional>
#include <set>

#include "Vec2.h"

class AStar
{
public:

	using CoordinateList = std::vector<Vec2>;

	struct Node
	{
		int G, H;
		Vec2 pos;
		Node *parent;

		Node(Vec2 pos, Node* parent = nullptr);
		int GetScore();
	};

	enum
	{
		RawSize = 1000 * sizeof(Node),
		DirectionCount = 8
	};

	std::vector<char*> rawNodesVec;
	char* rawNodes;
	char* ptrNodes;

	using NodeSet = std::set<Node*>;

	Node* AllocNode(Vec2 pos, Node* parent = nullptr);

	bool  DetectCollision(Vec2 pos);
	Node* FindNodeOnList(NodeSet& nodes, Vec2 pos);
	int   GetHeuristic(Vec2 src, Vec2 dst);

	CoordinateList direction;
	int   sz;
	char* field;
	char* visited;
	Vec2  worldSize;

public:

	AStar();
	virtual ~AStar();
	void LoadWorld(const char* name, Vec2 worldSize);
	bool FindPath(AStar::CoordinateList& path, Vec2 src, Vec2 dest);
};