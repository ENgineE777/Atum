
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteTileAsset.h"
#include "SpriteInst.h"

class SpriteTileInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteTileInst)

	virtual ~SpriteTileInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt) override;

#ifdef EDITOR
	bool IsOccupied(Vector2 pos);
	void CalcIndices();
#endif
};
