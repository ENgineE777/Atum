
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_2D_ShadowCaster2D ShadowCaster2D

ShadowCaster2D

*/


/**
\ingroup gr_code_scene_objects_2D
*/

class ShadowCaster2D : public SceneObject
{
public:
	META_DATA_DECL(ShadowCaster2D)

	float   radius;
	Vector2 pos;
	float   alpha;
	float   depth;
	string  caster_group;

	virtual ~ShadowCaster2D() = default;

	void Init() override;

	void BindClassToScript() override;

	void Draw(float dt);
};
