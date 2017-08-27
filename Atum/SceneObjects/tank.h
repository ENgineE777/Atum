
#pragma once

#include "model.h"
#include "Terrain.h"
#include "PhysBox.h"

#include "Services/Physics/Physics.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

class Tank : public SceneObject
{
public:

	META_DATA_DECL(Tank)
	CLASSDECLDIF(SceneObject, Tank)

	struct Projectile
	{
		static float maxTimeLife;
		static float speed;
		static float splashTime;
		static float splashMaxRadius;

		Vector pos;
		Vector dir;
		int    stage;
		float  lifetime;
		int    state;
	};

	std::vector<Projectile> projectiles;

	struct Box
	{
		PhysObject* obj;
		PhysBox*    box;
	};

	std::vector<Box> boxes;

	Vector  angles;
	Matrix  view;
	Matrix  proj;
	float   move_speed;
	float   strafe_speed;

	int alias_forward;
	int alias_strafe;
	int alias_fast;
	int alias_rotate_active;
	int alias_rotate_x;
	int alias_rotate_y;

	bool  showDebug;

	Terrain* terrain;

	Model hover_model;
	Model::Drawer* hover_drawer;

	Model tower_model;
	Model::Drawer* tower_drawer;

	Model gun_model;
	Model::Drawer* gun_drawer;

	PhysHeightmap*  hm;
	PhysController* controller;
	PhysScene* pscene;

	void Init();

	void Play();
	void Stop();
	void Update(float dt);
	void AddSplash(Vector& pos, float radius, float force);
};
