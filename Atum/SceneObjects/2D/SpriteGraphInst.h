
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

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void Load(JSONReader& loader) override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt);
	void ActivateLink(string& link);
	void GotoNode(string& node);
	float GetLinearVelocityX();
	float GetLinearVelocityY();
	void ApplyLinearImpulse(float x, float y);
	void SetLinearVelocity(float x, float y);
	bool CheckColissionNormal(float x, float y);
	void Move(float x, float y);
	b2Body* HackGetBody();

#ifdef EDITOR
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
#endif
};
