
#pragma once

#include "model.h"
#include "Terrain.h"
#include "PhysBox.h"

#include "Services/Physics/Physics.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"
#include "Tank.h"

class TankClient : public SceneObject
{
public:

	META_DATA_DECL(TankClient)
	CLASSDECLDIF(SceneObject, TankClient)

	Tank::ClientState* state;

	Vector2 angles;
	Matrix  view;
	Matrix  proj;

	int alias_forward;
	int alias_strafe;
	int alias_fast;
	int alias_rotate_active;
	int alias_rotate_x;
	int alias_rotate_y;

	Model hover_model;
	Model::Drawer* hover_drawer;

	Model tower_model;
	Model::Drawer* tower_drawer;

	Model gun_model;
	Model::Drawer* gun_drawer;

	void Init();

	void Play();
	void Stop();
	void Update(float dt);
};
