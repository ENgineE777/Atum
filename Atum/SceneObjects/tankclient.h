
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "terrain.h"
#include "PhysBox.h"

#include "Services/Physic/Physics.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"
#include "tank.h"
#include "VirtualJoystick.h"

class TankClient : public SceneObject
{
public:

	META_DATA_DECL(TankClient)

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

	VirtualJoystick* vjoy = nullptr;
	string vjoy_name;

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

	virtual void Init();
	virtual void ApplyProperties();

	void AddIsntance(int id, bool  is_contralable);

	bool Play();
	void Stop();
	void Update(float dt);

	void SendClientState(float dt);
};
