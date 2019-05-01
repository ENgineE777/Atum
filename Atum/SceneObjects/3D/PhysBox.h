
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_PhysBox Physic Box

Physic Box

*/


/**
\ingroup gr_code_scene_objects_3D
*/

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

	bool Play();
	void Stop();
};
