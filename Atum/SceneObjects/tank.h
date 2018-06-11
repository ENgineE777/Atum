
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "terrain.h"
#include "PhysBox.h"

#include "Services/Physics/Physics.h"

#include "Services/Render/Render.h"
#include "Services/Scene/SceneObject.h"

#include "Services/Network/Network.h"

class Tank : public SceneObject, public NetworkDelegate
{
public:

	enum PACKETID
	{
		CLIENTID = 0,
		ADDINSTANCE,
		CLIENTSTATE,
		SERVERSTATE
	};

	META_DATA_DECL(Tank)

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
 
	struct ServerState
	{
		float  angle = 0;
		Vector pos = 0.0f;
		float  move_speed = 0.0f;
		float  strafe_speed = 0.0f;
		float  shoot_cooldown = 0.0f;
		float  tower_angel = 0.0f;
		float  timeStamp = 0.0f;
		PhysController* controller = nullptr;
	};

	struct ClientState
	{
		int    up = 0;
		int    rotate = 0;
		bool   fired = false;
		float  needed_tower_angel = 0.0f;
		float  timeStamp = 0.0f;
		Vector gun_pos;
		Vector gun_dir;
	};

	class TankClient* client;

	bool is_server = false;

	int  clientID;
	NetworkServer netServer;
	NetworkClient netClient;

	float time = 0.0f;
	bool  showDebug;

	void Init();

	void AddIsntance(int id);

	void Play();
	void Stop();
	void Update(float dt);
	void AddSplash(Vector& pos, float radius, float force);

	void SendServerState(float dt);
	void SendClientState(float dt);

	virtual void OnDataRecieved(void* data, int size);
	virtual void OnClientConnected(int id);
};
