
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteGraphAsset.h"

class SpriteGraphInst : public SceneObject
{
public:
	META_DATA_DECL(SpriteGraphInst)

	uint32_t asset_uid;
	SpriteGraphAsset* asset = nullptr;
	SpriteGraphAsset::Instance graph_instance;
	string asset_name; 
	Transform2D trans;
	b2Body* body = nullptr;

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void Draw(float dt);
	void ActivateLink(string& link);
	void GotoNode(string& node);
	float GetLinearVelocityX();
	float GetLinearVelocityY();
	void ApplyLinearImpulse(float x, float y);
	void SetLinearVelocity(float x, float y);
	bool CheckColissionNormal(float x, float y);

	void Play() override;
	void Stop() override;

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};
