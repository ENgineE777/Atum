
#pragma once

#include "Services/Scene/SceneAsset.h"

/**
\page scene_assets_common_SceneScriptAsset Asset

Script Asset

*/


/**
\ingroup gr_code_scene_assets_common
*/

class SceneScriptAsset : public SceneAsset
{
public:

	enum NodeType
	{
		SceneCallback,
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

	enum ParamType
	{
		None,
		Int,
		Float,
	};

	enum CallType
	{
		OnCallback,
		OnInit,
		EveryFrame
	};

	struct NodeScriptMethod : Node
	{
		META_DATA_DECL(NodeScriptMethod)

		ParamType param_type;
		CallType call_type;

		asIScriptFunction* method = nullptr;
		vector<LinkToMethod> links;

		void Load(JSONReader& loader) override;
		void Save(JSONWriter& saver) override;
	};

	bool played = false;

	vector<Node*> nodes;
	string main_class;

	vector<NodeScriptMethod*> on_start_init;
	vector<NodeScriptMethod*> frame_updates;

	asIScriptModule* mod = nullptr;
	asITypeInfo* class_type = nullptr;

	META_DATA_DECL(SceneScriptAsset)

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void SetName(const char* name) override;

	bool Play() override;

	void Release() override;

	bool UsingCamera2DPos() override;

	void GetScriptFileName(string& filename);

#ifdef EDITOR
	class SceneScriptInst* script_inst = nullptr;
	string prev_filename;
	void RenameScriptFile();
	void SetUID(uint32_t uid) override;
	SceneObject* CreateInstance(Scene* scene) override;
	void EditorWork(float dt);
	void EditorWork(float dt, class SceneScriptInst* inst);

	void Copy(SceneObject* src) override;

	void SetEditMode(bool ed) override;

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
