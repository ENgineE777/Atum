
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteInst.h"

/**
\page scene_assets_2D_SpriteGraphInst Instance

Sprite Anim Graph Instance

*/


/**
\ingroup gr_code_scene_objects_2D
*/

class SpriteGraphInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteGraphInst)

	float hack_height = 0.0f;

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt);

#ifdef EDITOR
	void ShowProperties(bool show);
#endif
};
