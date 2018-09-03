
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteAsset.h"
#include "Services/Script/Libs/scriptarray.h"

class SpriteInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpriteInst)

	Transform2D trans;
	Sprite::FrameState frame_state;
	bool allow_instances = false;
	float axis_scale = 1.0f;

	struct Instance
	{
		b2Body* body = nullptr;
		Vector2 pos;
		int     visible = 1;
		Sprite::FrameState frame_state;
	};

	CScriptArray* array = nullptr;

	vector<Instance> instances;

	virtual ~SpriteInst() = default;

	SpriteAsset* Asset() { return (SpriteAsset*)asset; };

	void BindClassToScript() override;

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void Draw(float dt);

	void Play() override;
	void Stop() override;

	void AddInstance(float x, float y);
#ifdef EDITOR
	int sel_inst = -1;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif
};
