
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

class SpriteAsset;

class SpriteGraphAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteGraphAsset)

	Transform2D trans;

	Sprite::Data sprite;
	static Sprite::FrameState state;

	enum DragMode
	{
		None,
		MoveNode,
		MoveField,
		AddLink
	};

	enum NodeType
	{
		AnimNode,
		LogicNode,
		GroupNode
	};

	class Link : public Object
	{
	public:
		META_DATA_DECL(Link)

		int     index;
		string  name;
		bool    def_link = false;
		Vector2 arrow_pos;
	};

	class Node : public Object
	{
	public:
		META_DATA_DECL(Node)

		NodeType type = AnimNode;
		SpriteAsset* asset;
		bool looped = true;
		bool reversed = false;
		Vector2 pos = 0.0f;
		string name;
		string asset_name;
		int def_link = -1;
		vector<Link> links;
	};

	bool valid = false;
	int def_node = -1;
	vector<Node> nodes;

	class Instance
	{
	public:
		SpriteGraphAsset* asset = nullptr;
		Sprite::FrameState state;
		Node* cur_node = nullptr;

		void Reset();
		bool ActivateLink(const char* link);
		void Update(float dt);
		void GotoNode(int index);
		bool GotoNode(const char* node);
	};

	SpriteGraphAsset();
	virtual ~SpriteGraphAsset();

	virtual void Init();
	virtual void ApplyProperties();
	virtual void Load(JSONReader& loader);
	virtual void Save(JSONWriter& saver);

	void PrepareInstance(Instance* inst);

	Vector2 camera_pos = 0.0f;
	bool UsingCamera2DPos() override;
	Vector2& Camera2DPos() override;

#ifdef EDITOR
	Vector2 nodeSize = Vector2(100.0f, 50.0f);
	Vector2 linkSize = Vector2(15.0f, 15.0f);
	int selNode = -1;
	int targetNode = -1;
	int selLink = -1;
	DragMode drag = None;
	Vector2 mouse_pos;

	void Draw(float dt);

	int GetNodeIndex(Vector2& ms);
	void CreateNode(NodeType type);
	void Delete();
	void MakeNodeAsDefault();
	void ShowProperties(bool show);
	virtual void Copy(SceneObject* src);
	virtual void SetEditMode(bool ed);
	virtual void OnMouseMove(Vector2 delta_ms);
	virtual void OnLeftMouseDown(Vector2 ms);
	virtual void OnLeftMouseUp();
	virtual void OnRightMouseDown(Vector2 ms);
	virtual void OnRightMouseUp();
#endif
};
