
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneScriptAsset.h"

/**
\page scene_assets_common_SceneScriptInst Instance

This asset instance creates instance of script class from script asset and links with scene object
via link builder.

Via link builder 'Property Node' or 'Callback Node' can be linked with scene object in scene.
To make linkage firstly select needed node then hold key 'Left Ctrl' and drag needed scene object
from list to node. In case of 'Callback Node' needed callback can be selected from a list.

More details how to work with scripts can be found at \ref doc_editor_scripting.

![Caption text](../images/ScriptInst.jpg "Image")

This class ::SceneScriptInst is a representation on C++ side.

*/


/**
\ingroup gr_code_scene_objects_common
*/

/**
\brief Representation of a instance of a script

This class creates instance of script class from script asset and links with scene object via link builder.
More detail how to work with link builder can be found at \ref scene_assets_common_SceneScriptInst "link".

*/

class SceneScriptInst : public SceneObjectInst
{
public:

#ifndef DOXYGEN_SKIP

	struct WrapperSceneObjectRef
	{
		META_DATA_DECL_BASE(WrapperSceneObjectRef)

		SceneObjectRef ref;

		void Load(JSONReader& loader);
		void Save(JSONWriter& saver);
	};

	struct Node
	{
		META_DATA_DECL_BASE(Node)

		vector<WrapperSceneObjectRef> objects;

		string callback_type;

		Node()
		{
			objects.resize(1);
		};

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
	bool Play() override;
	void InjectIntoScript();
	void CallOnInitScript();
	void Release() override;

	bool UsingCamera2DPos() override;

	void CallMethods(vector<SceneScriptAsset::NodeScriptMethod*> methods, bool use_dt);

#ifdef EDITOR
	void SetScene(Scene* scene) override;
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

#endif
};
