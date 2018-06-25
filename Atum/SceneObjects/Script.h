
#pragma once

#include "Services/Scene/SceneObject.h"

class Script : public SceneObject
{
public:

	struct NodeSceneObject
	{
		Vector2 pos;
		string name;
		SceneObject* object = nullptr;
	};

	vector<NodeSceneObject> link_objects;

	struct Link
	{
		int node;
		string param;
	};

	struct NodeScriptObject
	{
		vector<Link> linked_nodes;
		int type = 0;
		Vector2 pos;
		string name;
		string def;
		string param;
		asIScriptFunction* method = nullptr;
	};

	vector<NodeScriptObject> script_objects;

	META_DATA_DECL(Script)

	void Init() override;
	void SetName(const char* name) override;
	void Work(float dt);
	void EditorWork(float dt);

	void Play() override;
};
