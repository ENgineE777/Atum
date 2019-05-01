
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpritesLayerAsset.h"

/**
\page scene_assets_2D_SpritesLayerInst Instance

Sprites Layer

*/


/**
\ingroup gr_code_scene_objects_2D
*/

class SpritesLayerInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpritesLayerInst)

	Transform2D trans;
	Vector2 axis_scale = 1.0f;

	virtual ~SpritesLayerInst() = default;

	void Init() override;
	void Draw(float dt);
};
