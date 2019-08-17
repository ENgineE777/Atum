
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "SceneObjects/3D/terrain.h"
#include "SceneObjects/3D/PhysBox.h"

#include "Services/Physic/Physics.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"
#include "tank.h"
#include "SceneObjects/Common/VirtualJoystick.h"

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

	string base_model_name;
	string tower_model_name;
	string gun_model_name;

	string aliases_name;

	Model base_model;
	Model tower_model;
	Model gun_model;

	SceneObjectRef vjoy_ref;
	VirtualJoystick* vjoy = nullptr;

	struct Instance
	{
		bool is_contralable;
		int id;
		Tank::ClientState clientState;
		Tank::ServerState serverState;
		Model::Drawer* base_drawer;
		Model::Drawer* tower_drawer;
		Model::Drawer* gun_drawer;
	};

	std::vector<Instance> instances;

	bool Is3DObject() override;

	virtual void Init();
	virtual void ApplyProperties();

	void AddIsntance(int id, bool  is_contralable);

	bool Play() override;
	void Stop();
	void Update(float dt);

	void SendClientState(float dt);
};
