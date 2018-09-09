
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

	Phys2DComp();
};

class Phys2DCompInst : public SceneObjectInstComp
{
public:

	class SpriteInst* sprite_inst = nullptr;
	Phys2DComp::BodyType body_type;
	vector<b2Body*> bodies;
	META_DATA_DECL(Phys2DCompInst)

	void Play() override;
	void Stop() override;
	void UpdateInstances(float dt);
};