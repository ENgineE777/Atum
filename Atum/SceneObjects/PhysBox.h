
#pragma once

#include "Services/Scene/SceneObject.h"

class PhysBox : public SceneObject
{
public:
	META_DATA_DECL(PhysBox)

	float sizeX;
	float sizeY;
	float sizeZ;
	bool  isStatic;
	Color color;

	PhysObject* obj = nullptr;

	PhysBox();
	virtual ~PhysBox();

	virtual void Init();
	void Draw(float dt);

	void Play();
	void Stop();
};
