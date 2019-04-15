
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Sprite.h"

class SpriteAsset;

class SpriteGraphAsset : public SceneAsset
{
public:
	META_DATA_DECL(SpriteGraphAsset)

	Transform2D trans_anim;
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
		SpriteAsset* asset = nullptr;
		bool looped = true;
		bool reversed = false;
		Vector2 pos = 0.0f;
		string name;
		uint32_t object_uid = 0;
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

	void Init() override;
	void ApplyProperties() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;

	void PrepareInstance(Instance* inst);

	bool UsingCamera2DPos() override;

	Vector2 GetDefaultSize();
	Vector2 GetDefaultOffset();

#ifdef EDITOR
	Vector2 nodeSize = Vector2(150.0f, 80.0f);
	Vector2 linkSize = Vector2(15.0f, 15.0f);
	int sel_node = -1;
	int target_node = -1;
	int sel_link = -1;
	DragMode drag = None;
	Vector2 mouse_pos;

	void Draw(float dt);

	int GetNodeIndex(Vector2& ms);
	void CreateNode(NodeType type);
	void Delete();
	void MakeNodeAsDefault();
	void ShowProperties(bool show);
	void Copy(SceneObject* src) override;
	void SetEditMode(bool ed) override;
	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	void OnRightMouseDown(Vector2 ms) override;
	void OnPopupMenuItem(int id) override;
	void OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms) override;
#endif
};
