
#pragma once

#include "Services/Scene/SceneObjectComp.h"
#include "SpriteInst.h"

class Phys2DComp : public SceneAssetComp
{
public:
	
	META_DATA_DECL(Phys2DComp)
	
	enum BodyType
	{
		StaticBody = 0,
		DynamicBody,
		DynamicCCDBody,
		KineticBody,
		TrigerBody,
		Controller
	};

	BodyType body_type;
	float density;
	float friction;
	bool allow_rotate;
	bool use_object_size;
	float width;
	float height;

	Phys2DComp();
};

class Phys2DCompInst : public SceneObjectInstComp
{
public:

	Phys2DComp::BodyType body_type;

	vector<PhysScene::BodyUserData> bodies;

	struct ScriptProxy
	{
		SpriteInst::Instance* inst = nullptr;
		PhysScene::BodyUserData* body = nullptr;
		void ApplyLinearImpulse(float x, float y);
		void MoveTo(float x, float y);
		bool CheckColission(bool under);
		void MoveController(float dx, float dy);
	};

	int prop_index = -1;
	vector<ScriptProxy> script_bodies;

	META_DATA_DECL(Phys2DCompInst)

	void BindClassToScript() override;
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;


	void Play() override;
	void CreatBody(int index, bool visible, Vector2 pos, Vector2 size, Vector2 center, bool allow_rotate);
	void Stop() override;
	void UpdateInstances(float dt);
};