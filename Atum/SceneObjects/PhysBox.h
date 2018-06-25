
#pragma once

#include "Services/Scene/SceneObject.h"

class PhysBox : public SceneObject
{
public:

	Matrix transform;
	META_DATA_DECL(PhysBox)

	float sizeX;
	float sizeY;
	float sizeZ;
	bool  isStatic;
	Color color;

	PhysObject* obj = nullptr;

	virtual ~PhysBox() = default;

	Matrix& Trans() override;
	bool Is3DObject() override;
	void Init() override;
	void Draw(float dt);

	void Play();
	void Stop();
};
