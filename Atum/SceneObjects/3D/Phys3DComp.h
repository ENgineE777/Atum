
#pragma once

#include "Services/Scene/SceneObjectComp.h"
#include "MeshInstance.h"

/**
\page scene_assets_components_phys3d Physic 3D

This component is adding 3D physical object to each instance of SpriteInst, SpriteGraphInst or SpriteTileInst.
3D physical object can be static or dynamic, can have own dimentions. Belonging to particular group also adjustable.

This class ::Phys3DComp is a representation on C++ side.

This class Script::Phys3D is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
body_type         | Type of a body
density           | Density of a body
friction          | Friction of a material of a body
allow_rotate      | Allowing to be rotateable
use_object_size   | Should be used size of affected body
width             | Width of a body
height            | Height of a body
group             | Group of a body 

*/

/**
\ingroup gr_code_scene_assets_components
*/

/**
\brief Representation of 3D physical object component

This component is adding 3D physical object to each instance of SpriteInst, SpriteGraphInst or SpriteTileInst.
3D physical object can be static or dynamic, can have own dimentions. Belonging to particular group also adjustable.

This class Script::Phys3D is a representation in a script.

*/

class Phys3DComp : public SceneAssetComp
{
public:
	
	META_DATA_DECL(Phys3DComp)
	
	enum BodyType
	{
		StaticBody = 0 /*!< Static body */,
		DynamicBody /*!< Dynamic body */,
		DynamicCCDBody /*!< Dynamic body with enabled CCD */,
		KineticBody /*!< Kinetic body */,
		TrigerBody /*!< Triger body */,
		Controller /*!< Character controller */
	};

	/**
	\brief Type of a body
	*/

	BodyType body_type;

	/**
	\brief Density of a body
	*/

	float density;

	/**
	\brief Friction of a material of a body
	*/

	float friction;

	/**
	\brief Allowing to be a physical object rotateable
	*/

	bool allow_rotate;

	/**
	\brief Group of a body 
	*/

	uint32_t group;

#ifndef DOXYGEN_SKIP

	Phys3DComp();

#endif
};

class Phys3DCompInst : public SceneObjectInstComp
{
public:

	Phys3DComp::BodyType body_type;
	uint32_t group;

	vector<PhysScene::BodyUserData> bodies;

	struct ScriptProxy
	{
		MeshInstance::Instance* inst = nullptr;
		PhysScene::BodyUserData* body = nullptr;
		void ApplyLinearImpulse(Vector3& impulse);
		void SetTransform(Matrix& trans);
		void SetGroup(int group);
		bool CheckColission(bool under);
		void MoveController(Vector3& delta, uint32_t group, uint32_t ignore_group);
	};

	int prop_index = -1;
	vector<ScriptProxy> script_bodies;

	META_DATA_DECL(Phys3DCompInst)

	void Init() override;

	void BindClassToScript() override;
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;

	void SyncInstances();

	void Play() override;
	void CreateBody(int index, bool visible, Matrix& transform, Vector3 size, Vector3 center, bool allow_rotate);
	void UpdateInstances(float dt);

	void Release() override;
};