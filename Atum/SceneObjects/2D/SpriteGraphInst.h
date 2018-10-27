
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteInst.h"

class SpriteGraphInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteGraphInst)

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt);
	void ActivateLink(int index, string& link);
	void GotoNode(int index, string& node);
	PhysController* HackGetController(int index);
	bool CheckColission(int index, bool under);
	void MoveController(int index, float dx, float dy);
	void MoveControllerTo(int index, float x, float y);
};
