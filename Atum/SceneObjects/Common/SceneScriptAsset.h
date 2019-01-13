
#pragma once

#include "Services/Scene/SceneAsset.h"

class SceneScriptAsset : public SceneAsset
{
public:

	enum NodeType
	{
		ScnCallback,
		ScriptProperty,
		ScriptMethod
	};

	struct Node
	{
		Vector2 pos;
		string name;
		NodeType type;

		virtual void Load(JSONReader& loader);
		virtual void Save(JSONWriter& saver);
		virtual MetaData* GetMetaData() = 0;
	};


	struct NodeSceneObject : Node
	{
		META_DATA_DECL(NodeSceneObject)

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	struct NodeScriptProperty : NodeSceneObject
	{
		META_DATA_DECL(NodeScriptProperty)

		string prefix;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	struct LinkToMethod
	{
		META_DATA_DECL(LinkToMethod)

		int node;
		Vector2 arrow_pos;
		string param;
	};

	struct NodeScriptMethod : Node
	{
		META_DATA_DECL(NodeScriptMethod)

		int param_type;
		asIScriptFunction* method = nullptr;
		vector<LinkToMethod*> links;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	vector<Node*> nodes;
	string main_class;

	asIScriptModule* mod = nullptr;
	asITypeInfo* class_type = nullptr;

	META_DATA_DECL(SceneScriptAsset)

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void SetName(const char* name) override;

	bool Play() override;

	void Release() override;

	Vector2 camera_pos = 0.0f;
	bool UsingCamera2DPos() override;
	Vector2& Camera2DPos() override;

	void GetScriptFileName(uint32_t id, string& filename);

#ifdef EDITOR
	SceneObject* CreateInstance() override;
	void EditorWork(float dt);
	void EditorWork(float dt, class SceneScriptInst* inst);

	Vector2 nodeSize = Vector2(150.0f, 80.0f);
	Vector2 linkSize = Vector2(15.0f, 15.0f);
	int sel_node = -1;
	int sel_link = -1;
	int target_link = -1;
	Vector2 ms_pos = 0.0f;
	bool in_drag = false;
	bool link_drag = false;
	void ShowProperties(bool show);
	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	void OnRightMouseDown(Vector2 ms) override;
	void OnPopupMenuItem(int id) override;
#endif
};
