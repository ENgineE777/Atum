
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

	float shoot_cooldown;

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

	struct ClientState
	{
		float  tower_angel = 0.0f;
		int    up = 0;
		int    rotate = 0;
		bool   fired = false;
		float  angle = 0;
		Vector pos = 0.0f;
		Vector gun_pos;
		Vector gun_dir;
	};

	ClientState state;

	float move_speed;
	float strafe_speed;

	bool  showDebug;

	PhysController* controller;

	void Init();

	void Play();
	void Stop();
	void Update(float dt);
	void AddSplash(Vector& pos, float radius, float force);
};
