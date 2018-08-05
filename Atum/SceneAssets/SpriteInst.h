
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteAsset.h"

class SpriteInst : public SceneObject
{
public:
	META_DATA_DECL(SpriteInst)

	uint32_t asset_uid = 0;
	SpriteAsset* asset = nullptr;
	Transform2D trans;
	Sprite::FrameState frame_state;
	bool allow_instances = false;
	float axis_scale = 1.0f;

	struct Instance
	{
		float x, y;
		Sprite::FrameState frame_state;
	};

	vector<Instance> instances;

	b2Body* body = nullptr;

	virtual ~SpriteInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void Draw(float dt);

	void Play() override;
	void Stop() override;

	void AddInstance(float x, float y);
#ifdef EDITOR
	bool CheckSelection(Vector2 ms) override;
	void Copy(SceneObject* src) override;
	void SetEditMode(bool ed) override;
#endif
};
