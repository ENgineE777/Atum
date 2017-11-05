
#pragma once

#define WIN32_LEAN_AND_MEAN

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

	Vector2 angles;
	Matrix  view;
	Matrix  proj;

	int alias_forward;
	int alias_strafe;
	int alias_rotate_active;
	int alias_rotate_x;
	int alias_rotate_y;
	int alias_fire;

	Model hover_model;
	Model tower_model;
	Model gun_model;

	struct Instance
	{
		bool is_contralable;
		int id;
		Tank::ClientState clientState;
		Tank::ServerState serverState;
		Model::Drawer* hover_drawer;
		Model::Drawer* tower_drawer;
		Model::Drawer* gun_drawer;
	};

	std::vector<Instance> instances;

	void Init();

	void AddIsntance(int id, bool  is_contralable);

	void Play();
	void Stop();
	void Update(float dt);

	void SendClientState(float dt);
};
