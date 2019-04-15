
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneScriptAsset.h"

class SceneScriptInst : public SceneObjectInst
{
public:

	struct Node
	{
		META_DATA_DECL(Node)

		uint32_t object_uid = 0;
		uint32_t object_child_uid = 0;
		string   callback_type;

		SceneObject* object = nullptr;

		void Load(JSONReader& loader);
		void Save(JSONWriter& saver);
	};

	inline SceneScriptAsset* Asset()
	{
		return static_cast<SceneScriptAsset*>(asset);
	}

	vector<Node> nodes;
	asIScriptObject* class_inst = nullptr;

	META_DATA_DECL(SceneScriptInst)

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void Work(float dt);

	bool InjectIntoScript(const char* type, void* property, const char* prefix) override;
	bool PostPlay() override;
	void Stop() override;

	Vector2 camera_pos = 0.0f;
	bool UsingCamera2DPos() override;

#ifdef EDITOR

	void SetOwner(Scene* owner) override;
	void Copy(SceneObject* src) override;

	void EditorWork(float dt);

	void SetEditMode(bool ed) override;

	void OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms) override;

	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	void OnRightMouseDown(Vector2 ms) override;
	void OnPopupMenuItem(int id) override;
	void ShowProperties(bool show);
#endif
};
