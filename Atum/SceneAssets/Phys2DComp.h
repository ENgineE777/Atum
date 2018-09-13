
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

	Phys2DComp::BodyType body_type;
	vector<b2Body*> bodies;
	META_DATA_DECL(Phys2DCompInst)

	void Play() override;
	template<typename T>
	void Play(T* sprite_inst);
	void Stop() override;
	void UpdateInstances(float dt);
	template<typename T>
	void UpdateInstances(T* sprite_inst, float dt);
};