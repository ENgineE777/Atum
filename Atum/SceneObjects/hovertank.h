
#pragma once

#include "model.h"
#include "terrain.h"
#include "PhysBox.h"

#include "PxPhysicsAPI.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

class HoverTank : public SceneObject
{
public:

	Matrix transform;
	META_DATA_DECL(HoverTank)

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
		PhysBox*               obj;
	};

	std::vector<Box> boxes;

	Vector2 angles;
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

	float physStep = 1.0f / 60.0f;
	float accum_dt;
	bool  showDebug;

	Terrain* terrain;

	Model hover_model;
	Model::Drawer* hover_drawer;

	Model tower_model;
	Model::Drawer* tower_drawer;

	Model gun_model;
	Model::Drawer* gun_drawer;

	virtual ~HoverTank() = default;

	Matrix& Trans() override;
	bool Is3DObject() override;
	void Init() override;

	void Play() override;
	void Stop() override;
	void Update(float dt);
	void AddHover(Matrix& mat, Vector offset);
	void AddSplash(Vector& pos, float radius, float force);
};
