
#pragma once

#include "Services/Scene/SceneObject.h"

class Script : public SceneObject
{
public:

	enum NodeType
	{
		ScnObject,
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

		SceneObject* object = nullptr;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	struct LinkToProperty
	{
		int node;
		Vector2 arrow_pos;

		virtual MetaData* GetMetaData() { return nullptr; };
	};

	struct NodeScriptProperty : Node
	{
		META_DATA_DECL(NodeScriptProperty)

		vector<LinkToProperty*> links;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	struct LinkToMethod : LinkToProperty
	{
		META_DATA_DECL(LinkToMethod)

		string param;
	};

	struct NodeScriptMethod : NodeScriptProperty
	{
		META_DATA_DECL(NodeScriptMethod)

		int param_type;
		asIScriptFunction* method = nullptr;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	vector<Node*> nodes;

	META_DATA_DECL(Script)

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void SetName(const char* name) override;
	void Work(float dt);
	void EditorWork(float dt);

	void Play() override;
	void Stop() override;

	void Release() override;

	Vector2 camera_pos = 0.0f;
	bool UsingCamera2DPos() override;
	Vector2& Camera2DPos() override;

#ifdef EDITOR
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
