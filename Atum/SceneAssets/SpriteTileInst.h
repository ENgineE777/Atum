
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteTileAsset.h"
#include "SceneAssets/SpriteInst.h"

class SpriteTileInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteTileInst)

	virtual ~SpriteTileInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void Draw(float dt) override;

#ifdef EDITOR
	bool IsOccupied(Vector2 pos);
	void CalcIndices();
#endif
};
