
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteInst.h"

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
};
