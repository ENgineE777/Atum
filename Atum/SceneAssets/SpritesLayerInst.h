
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpritesLayerAsset.h"

class SpritesLayerInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpritesLayerInst)

	Transform2D trans;
	float axis_scale = 1.0f;

	virtual ~SpritesLayerInst() = default;

	void Init() override;
	void Draw(float dt);
};
