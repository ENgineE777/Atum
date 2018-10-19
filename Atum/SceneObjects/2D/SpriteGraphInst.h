
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteGraphAsset.h"

class SpriteGraphInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpriteGraphInst)

	SpriteGraphAsset::Instance graph_instance;
	string asset_name; 
	Transform2D trans;
	Vector2 dir = 0.0f;

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void Load(JSONReader& loader) override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt);
	void ActivateLink(string& link);
	void GotoNode(string& node);
	bool CheckColission(bool under);
	void Move(float dx, float dy);
	void MoveTo(float x, float y);
	PhysController* HackGetBody();

#ifdef EDITOR
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
#endif
};
