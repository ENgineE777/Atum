
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteTileAsset.h"

class SpriteTileInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpriteTileInst)

	Transform2D trans;
	bool allow_instances = false;
	float axis_scale = 1.0f;

	struct Instance
	{
		META_DATA_DECL(Instance)

		Vector2 pos;
		int index;
		Sprite::FrameState frame_state;
	};

	vector<Instance> instances;

	virtual ~SpriteTileInst() = default;

	void Init() override;
	void Draw(float dt);

	void Play() override;

#ifdef EDITOR
	int sel_inst = -1;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
	bool IsOccupied(Vector2 pos);
	void CalcIndices();
#endif
};
