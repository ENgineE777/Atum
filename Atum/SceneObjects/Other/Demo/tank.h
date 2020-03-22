
#pragma once

#define WIN32_LEAN_AND_MEAN

#include "model.h"
#include "SceneObjects/3D/terrain.h"
#include "SceneObjects/3D/PhysBox.h"

#include "Services/Physic/Physics.h"

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

	Matrix transform;

	META_DATA_DECL(Tank)

	struct Projectile
	{
		static float maxTimeLife;
		static float speed;
		static float splashTime;
		static float splashMaxRadius;

		Vector3 pos;
		Vector3 claster_pos[5];
		Vector3 dir;
		int     stage;
		float   lifetime;
		int     state;
		bool    special;
	};

	std::vector<Projectile> projectiles;
 
	struct ServerState
	{
		int    hp = 100;
		int    ammo = 50;
		bool   is_ai = false;
		int    special = 0;
		float  angle = 0;
		Vector3 pos = 0.0f;
		float  move_speed = 0.0f;
		float  strafe_speed = 0.0f;
		float  shoot_cooldown = 0.0f;
		float  tower_angel = 0.0f;
		float  timeStamp = 0.0f;
		PhysController* controller = nullptr;
	};

	struct ClientState
	{
		int     up = 0;
		int     rotate = 0;
		bool    fired = false;
		bool    special_fired = false;
		float   needed_tower_angel = 0.0f;
		float   timeStamp = 0.0f;
		Vector3 gun_pos;
		Vector3 gun_dir;
	};

	class TankClient* client;

	bool is_server = false;

	bool bonuses_places = false;

	struct Bonus
	{
		int type = 0;
		Vector3 pos;
		float cooldown = -1.0f;
	};

	vector<Bonus> bonuses;

	int  clientID;
	NetworkServer netServer;
	NetworkClient netClient;

	float time = 0.0f;

	void Init();

	void AddInstance(int id, Vector3 pos, bool is_bot);

	Matrix* Trans() override;
	bool Is3DObject() override;

	bool Play() override;
	void Update(float dt);
	void AddSplash(Vector3& pos, float radius, float force);

	void SendServerState(float dt);
	void SendClientState(float dt);

	void OnDataRecieved(int id, void* data, int size) override;
	void OnClientConnected(int id) override;
};
