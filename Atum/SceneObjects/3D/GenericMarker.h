
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Services/Script/Libs/scriptarray.h"

/**
\page scene_object_3D_GenericMarker Generic Marker

Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
be finded via call of Scene::GetGroup

This class ::GenericMarker is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
scene_group       | Name of group which will be used in registration via call Scene::AddToGroup

*/


/**
\ingroup gr_code_scene_objects_3D
*/

/**
\brief Representation of a generic marker in 3D space

Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
be finded via call of Scene::GetGroup

*/

class GenericMarker : public SceneObject
{
public:

	struct Instance
	{
		META_DATA_DECL_BASE(Instance)

		Matrix transform;
		Color color;
		float radius = 1.0f;
	};

	vector<int> mapping;
	std::vector<Instance> instances;

	/**
	\brief Name of group which will be used in registration via class Scene::AddToGroup
	*/
	string scene_group;

#ifndef DOXYGEN_SKIP
	int sel_inst = -1;
	META_DATA_DECL(GenericMarker)

	bool Is3DObject() override;
	void Init() override;
	void ApplyProperties() override;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

	void MakeMapping(asIScriptObject* object, const char* prefix);
	bool InjectIntoScript(const char* type_name, int type, void* property, const char* prefix) override;

	void Draw(float dt);

#ifdef EDITOR
	bool CheckSelection(Vector2 ms, Vector3 start, Vector3 dir) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif

#endif
};
