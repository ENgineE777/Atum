
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteAsset.h"
#include "Services/Script/Libs/scriptarray.h"

class SpriteInst : public SceneObjectInst
{
public:
	META_DATA_DECL(SpriteInst)

	Transform2D trans;
	float axis_scale = 1.0f;

	struct Instance
	{
		META_DATA_DECL(Instance)

		int index = 0;
		Sprite::FrameState frame_state;

		void SetObject(asIScriptObject* object, vector<int>* mapping);
		void SetPos(Vector2 pos);
		Vector2 GetPos();
		void SetFlipped(int horz_flipped);
		int  GetFlipped();
		void SetVisible(int visible);
		bool IsVisible();

		Color color;
		float alpha = 1.0f;

	private:
		vector<int>* mapping = nullptr;
		asIScriptObject* object = nullptr;
		Vector2 pos;
		int     visible = 1;
	};

	vector<int> mapping;
	CScriptArray* array = nullptr;

	vector<Instance> instances;

	virtual ~SpriteInst() = default;

	void BindClassToScript() override;
	bool InjectIntoScript(const char* type, void* property) override;

	void Init() override;
	virtual void Draw(float dt);

	bool Play() override;

	b2Body* HackGetBody(int index);
	void AddInstance(float x, float y);
	void ClearInstances();
	void ApplyLinearImpulse(int index, float x, float y);
	void Move(int index, float x, float y);

#ifdef EDITOR
	int sel_inst = -1;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
	int  GetInstCount();
#endif
};
