
#pragma once

#include "Services/Scene/SceneObjectComp.h"

class Phys2DComp : public SceneAssetComp
{
public:
	
	META_DATA_DECL(Phys2DComp)
	
	enum BodyType
	{
		StaticBody = 0,
		DynamicBody,
		KineticBody
	};

	BodyType body_type;
	float density;
	float friction;
	bool use_object_size;
	float width;
	float height;

	Phys2DComp();
};

class Phys2DCompInst : public SceneObjectInstComp
{
public:

	Phys2DComp::BodyType body_type;
	vector<b2Body*> bodies;
	META_DATA_DECL(Phys2DCompInst)

	void Play() override;
	template<typename T>
	void Play(T* sprite_inst);
	void PlayGraphInst(class SpriteGraphInst* graph_inst);
	void CreatBody(int index, Vector2 pos, Vector2 size, Vector2 center);
	void Stop() override;
	void UpdateInstances(float dt);
	template<typename T>
	void UpdateInstances(T* sprite_inst);
	void UpdateGraphInst(class SpriteGraphInst* graph_inst);
};