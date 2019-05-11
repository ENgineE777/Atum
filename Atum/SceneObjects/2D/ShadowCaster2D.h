
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_2D_ShadowCaster2D ShadowCaster2D

Currently 2D space is a special mode and this class represents a omni shadow casted from a particular point in a that
special space. 2D scene objects from particular scene group are playing role of shadow casters. Point of a shadow caster
set via script.

This class ::ShadowCaster2D is a representation on C++ side.

This class Script::ShadowCaster2D is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
CasterGroup       | Scene group of shadow casters
x                 | X coordinate of a camera position
y                 | Y coordinate of a camera position
radius            | Radius of a shadow
alpha             | Transparency of a shadow
Depth             | Z value of a shadow

*/

/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of omni shadow in 2D space

Currently 2D space is a special mode and this class represents a omni shadow casted from a particular point in a that
special space. 2D scene objects from particular scene group are playing role of shadow casters. Point of a shadow caster
set via script.

This class Script::ShadowCaster2D is a representation in a script.

*/

class ShadowCaster2D : public SceneObject
{
public:
	META_DATA_DECL(ShadowCaster2D)

	/**
		\brief Radius of a shadow
	*/
	float radius;

	/**
	\brief A shadow caster point
	*/
	Vector2 pos;

	/**
	\brief Transparency of a shadow
	*/
	float alpha;

	/**
	\brief Z value of a shadow
	*/
	float depth;

	/**
	\brief Name of scene group of shadow casters
	*/
	string caster_group;

#ifndef DOXYGEN_SKIP

	virtual ~ShadowCaster2D() = default;

	void Init() override;

	void BindClassToScript() override;

	void Draw(float dt);
#endif
};
