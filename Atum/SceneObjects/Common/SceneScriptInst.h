
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneScriptAsset.h"

class SceneScriptInst : public SceneObjectInst
{
public:

	struct Node
	{
		uint32_t object_uid = 0;
		uint32_t object_child_uid = 0;

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

	bool PostPlay() override;
	void Stop() override;

	Vector2 camera_pos = 0.0f;
	bool UsingCamera2DPos() override;
	Vector2& Camera2DPos() override;

#ifdef EDITOR

	void EditorWork(float dt);

	void OnDragObjectFromTreeView(bool is_scene_tree, SceneObject* object, Vector2 ms) override;

	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;
	void OnRightMouseDown(Vector2 ms) override;
	void OnPopupMenuItem(int id) override;
#endif
};
