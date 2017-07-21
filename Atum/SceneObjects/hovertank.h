
#pragma once

#include "Services/Render/Render.h"
#include "model.h"
#include "Terrain.h"

#include "PxPhysicsAPI.h"

class HoverTank : public Object
{
public:

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
		physx::PxRigidDynamic* box;
		Vector                 size;
		Vector                 pos;
	};

	std::vector<Box> boxes;

	Vector2 angles;
	Matrix  view;
	Matrix  proj;
	float   move_speed;
	float   strafe_speed;

	Terrain* terrain;

	Model hover_model;
	Model::Drawer* hover_drawer;

	Model tower_model;
	Model::Drawer* tower_drawer;

	Model gun_model;
	Model::Drawer* gun_drawer;

	void Init(Terrain* set_terrain);

	void Update(float dt);
	void AddHover(Matrix& mat, Vector offset);
	void AddSplash(Vector& pos, float radius, float force);
};
