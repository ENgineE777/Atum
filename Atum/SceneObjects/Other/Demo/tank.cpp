#include "tank.h"
#include "tankclient.h"
#include "Services/Controls/Controls.h"

CLASSREG(SceneObject, Tank, "Tank")

META_DATA_DESC(Tank)
BASE_SCENE_OBJ_PROP(Tank)
META_DATA_DESC_END()

float Tank::Projectile::maxTimeLife = 4.0f;
float Tank::Projectile::speed = 50.0f;
float Tank::Projectile::splashTime = 0.35f;
float Tank::Projectile::splashMaxRadius = 5.0f;

void Tank::Init()
{
	Tasks(false)->AddTask(0, this, (Object::Delegate)&Tank::Update);
	Tasks(false)->AddTask(0, this, (Object::Delegate)&Tank::SendServerState, 1.0f / 15.0f);
	Tasks(false)->AddTask(0, this, (Object::Delegate)&Tank::SendClientState, 1.0f / 15.0f);
}

void Tank::AddInstance(int id, Vector3 pos, bool is_bot)
{
	client->AddIsntance(id, clientID == id);

	if (!is_server)
	{
		return;
	}

	PhysControllerDesc cdesc;
	cdesc.height = 1.0f;
	cdesc.radius = 1.0f;
	cdesc.pos = pos;
	cdesc.slopeLimit = cosf(Math::Radian * 60.0f);

	ServerState& state = client->instances[client->instances.size() - 1].serverState;

	state.pos = pos;
	state.is_ai = is_bot;
	state.controller = PScene()->CreateController(cdesc, 1);
}

Matrix* Tank::Trans()
{
	return &transform;
}

bool Tank::Is3DObject()
{
	return true;
}

bool Tank::Play()
{
	time = 0.0f;

	vector<Scene::Group*> out_groups;
	GetScene()->GetGroup(out_groups, "TankClient");

	for (auto group : out_groups)
	{
		if (group->objects.size() > 0)
		{
			client = (TankClient*)group->objects[0];
			break;
		}
	}

/*	if (netClient.Connect("192.168.0.19", 7890))
	{
		netClient.delegedate = this;

		is_server = false;
	}
	else*/
	{
		//netServer.Start("192.168.0.19", 7890);

		netServer.delegedate = this;

		is_server = true;

		clientID = 0;
		OnClientConnected(0);
	}

	return true;
}

void Tank::OnClientConnected(int id)
{
	AddInstance(id, Vector3(transform.Pos().x + id * 10.0f, transform.Pos().y, transform.Pos().z), false);

	char packet[256];

	{
		char* ptr = packet;

		*((int*)ptr) = CLIENTID;
		ptr += 4;

		*((int*)ptr) = id;
		ptr += 4;

		*((float*)ptr) = time;
		ptr += 4;

		netServer.Send2Client(id, packet, 12);
	}

	for (auto& inst : client->instances)
	{
		if (inst.id == id)
		{
			continue;
		}

		char* ptr = packet;

		*((int*)ptr) = ADDINSTANCE;
		ptr += 4;

		*((int*)ptr) = inst.id;
		ptr += 4;

		netServer.Send2Client(id, packet, 8);
	}

	{
		char* ptr = packet;

		*((int*)ptr) = ADDINSTANCE;
		ptr += 4;

		*((int*)ptr) = id;
		ptr += 4;

		netServer.Send2All(packet, 8);
	}

	SendServerState(0.0f);
	SendClientState(0.0f);
}

void Tank::OnDataRecieved(int id, void* data, int size)
{
	char* ptr = (char*)data;

	while (ptr - (char*)data < size)
	{
		int packetID = *((int*)ptr);
		ptr += 4;

		switch (packetID)
		{
			case CLIENTID:
			{
				clientID = *((int*)ptr);
				ptr += 4;

				time = *((float*)ptr);
				ptr += 4;

				break;
			}
			case ADDINSTANCE:
			{
				AddInstance(*((int*)ptr), Vector3(transform.Pos().x + *((int*)ptr) * 10.0f, transform.Pos().y, transform.Pos().z), false);
				ptr += 4;
				break;
			}
			case CLIENTSTATE:
			{
				int id = *((int*)ptr);
				ptr += 4;

				for (auto& inst : client->instances)
				{
					if (inst.id != id)
					{
						continue;
					}

					memcpy(&inst.clientState, ptr, sizeof(ClientState));
					ptr += sizeof(ClientState);
					break;
				}

				break;
			}
			case SERVERSTATE:
			{
				int id = *((int*)ptr);
				ptr += 4;

				for (auto& inst : client->instances)
				{
					if (inst.id != id)
					{
						continue;
					}

					memcpy(&inst.serverState, ptr, sizeof(ServerState));
					ptr += sizeof(ServerState);
					break;
				}

				break;
			}
		}
	}
}

void Tank::SendServerState(float dt)
{
	if (!GetScene()->Playing())
	{
		return;
	}

	if (!is_server)
	{
		return;
	}

	char packet[256];

	for (auto& inst : client->instances)
	{
		char* ptr = packet;

		*((int*)ptr) = SERVERSTATE;
		ptr += 4;

		*((int*)ptr) = inst.id;
		ptr += 4;

		inst.serverState.timeStamp = time;
		memcpy(ptr, &inst.serverState, sizeof(ServerState));
		
		netServer.Send2All(packet, sizeof(ServerState) + 8);
	}
}

void Tank::SendClientState(float dt)
{
	if (!GetScene()->Playing())
	{
		return;
	}

	if (is_server)
	{
		return;
	}
	
	char packet[256];

	for (auto& inst : client->instances)
	{
		if (!inst.is_contralable)
		{
			continue;
		}

		char* ptr = packet;

		*((int*)ptr) = CLIENTSTATE;
		ptr += 4;

		*((int*)ptr) = inst.id;
		ptr += 4;

		inst.clientState.timeStamp = time;
		memcpy(ptr, &inst.clientState, sizeof(ClientState));

		netClient.Send(packet, sizeof(ClientState) + 8);
	}
}

void Tank::Update(float dt)
{
	if (!GetScene()->Playing())
	{
		return;
	}

	if (!bonuses_places)
	{
		vector<Scene::Group*> out_group_bot;
		GetScene()->GetGroup(out_group_bot, "Bot");

		int index = 100;

		for (auto group : out_group_bot)
		{
			for (auto& obj : group->objects)
			{
				AddInstance(index, obj->Trans()->Pos(), false);

				index++;
			}
		}

		vector<Scene::Group*> out_group;
		GetScene()->GetGroup(out_group, "Terrain");

		PhysScene::RaycastDesc rcdesc;

		for (auto group : out_group)
		{
			if (group->objects.size() > 0)
			{
				Terrain* terrain = (Terrain*)group->objects[0];

				float square = 25.0f;
				float gap = 10.0f;

				float z = -terrain->hheight * 0.5f * terrain->scaleh + gap;

				while (z + square < terrain->hheight * 0.5f * terrain->scaleh)
				{
					float x = -terrain->hwidth * 0.5f * terrain->scaleh + gap;

					while (x + square < terrain->hwidth * 0.5f * terrain->scaleh)
					{
						bonuses.push_back(Bonus());

						Bonus& bonus = bonuses[bonuses.size() - 1];

						bonus.type = (int)(3.0f * Math::Rand() * 0.999f);
						bonus.pos = Vector3(x + square * Math::Rand(), 25.0f, z + square * Math::Rand());

						rcdesc.origin = bonus.pos;
						rcdesc.dir = Vector3(0.0f, -1.0f, 0.0f);
						rcdesc.length = 100.0f;
						rcdesc.group = 1;

						if (PScene()->RayCast(rcdesc))
						{
							bonus.pos.y = rcdesc.hitPos.y + 0.75f;
						}

						x += square + gap;
					}

					z += square + gap;
				}
			}
		}

		bonuses_places = true;
	}

	time += dt;

	if (is_server)
	{
		netServer.Update();
	}
	else
	{
		netClient.Update();
		return;
	}

	for (auto& inst : client->instances)
	{
		Matrix mat;
		mat.RotateY(inst.serverState.angle);
		inst.serverState.controller->GetPosition(mat.Pos());

		inst.serverState.pos = mat.Pos();

		Vector3 dir = mat.Vx();
		dir.y = 0;
		dir.Normalize();

		if (inst.clientState.up == 1)
		{
			inst.serverState.move_speed += 20 * dt;

			if (inst.serverState.move_speed > 30)
			{
				inst.serverState.move_speed = 30;
			}
		}
		else
		if (inst.clientState.up == -1)
		{
			inst.serverState.move_speed -= 15 * dt;

			if (inst.serverState.move_speed < -20)
			{
				inst.serverState.move_speed = -20;
			}
		}
		else
		{
			if (inst.serverState.move_speed > 0.1f)
			{
				inst.serverState.move_speed -= 30 * dt;

				if (inst.serverState.move_speed < 0)
				{
					inst.serverState.move_speed = 0;
				}
			}
			else
			if (inst.serverState.move_speed < 0.1f)
			{
				inst.serverState.move_speed += 30 * dt;

				if (inst.serverState.move_speed > 0)
				{
					inst.serverState.move_speed = 0;
				}
			}
		}

		if (inst.clientState.rotate == 1)
		{
			inst.serverState.strafe_speed += 15 * dt;

			if (inst.serverState.strafe_speed > 3)
			{
				inst.serverState.strafe_speed = 3;
			}
		}
		else
		if (inst.clientState.rotate == -1)
		{
			inst.serverState.strafe_speed -= 15 * dt;

			if (inst.serverState.strafe_speed < -3)
			{
				inst.serverState.strafe_speed = -3;
			}
		}
		else
		{
			if (inst.serverState.strafe_speed > 0.1f)
			{
				inst.serverState.strafe_speed -= 15 * dt;

				if (inst.serverState.strafe_speed < 0)
				{
					inst.serverState.strafe_speed = 0;
				}
			}
			else
			if (inst.serverState.strafe_speed < 0.1f)
			{
				inst.serverState.strafe_speed += 15 * dt;

				if (inst.serverState.strafe_speed > 0)
				{
					inst.serverState.strafe_speed = 0;
				}
			}
		}

		inst.serverState.angle += inst.serverState.strafe_speed * dt;

		if (inst.clientState.needed_tower_angel > -999.9f)
		{
			if (inst.serverState.tower_angel < Math::PI * 0.5f && inst.clientState.needed_tower_angel > Math::PI)
			{
				inst.clientState.needed_tower_angel -= Math::PI * 2.0f;
			}

			if (inst.clientState.needed_tower_angel > inst.serverState.tower_angel)
			{
				inst.serverState.tower_angel += 1.0f * dt;

				if (inst.clientState.needed_tower_angel < inst.serverState.tower_angel)
				{
					inst.clientState.needed_tower_angel = inst.serverState.tower_angel;
				}
			}
			else
			{
				inst.serverState.tower_angel -= 1.0f * dt;

				if (inst.clientState.needed_tower_angel > inst.serverState.tower_angel)
				{
					inst.clientState.needed_tower_angel = inst.serverState.tower_angel;
				}
			}

			inst.serverState.angle += inst.serverState.strafe_speed * dt;
		}
		else
		{
			inst.serverState.tower_angel = inst.serverState.angle;
		}

		core.render.DebugPrintText(Vector2(10.0f, 30.0f), COLOR_GREEN, "%4.3f %4.3f \n", inst.clientState.needed_tower_angel, inst.serverState.tower_angel);

		dir *= inst.serverState.move_speed;
		dir.y = -9.8f;
		dir *= dt;

		inst.serverState.controller->Move(dir, 1, 0);

		if (inst.serverState.shoot_cooldown > 0.0f)
		{
			inst.serverState.shoot_cooldown -= dt;
		}

		if (((inst.clientState.special_fired && inst.serverState.special == 100) || (inst.clientState.fired && inst.serverState.ammo > 0)) && inst.serverState.shoot_cooldown <= 0.0f)
		{
			projectiles.push_back(Projectile());
			Projectile& proj = projectiles[projectiles.size() - 1];

			proj.pos = inst.clientState.gun_pos;
			proj.dir = inst.clientState.gun_dir;
			proj.lifetime = Projectile::maxTimeLife;
			proj.state = 0;

			inst.serverState.shoot_cooldown = 1.5f;

			proj.special = inst.clientState.special_fired;

			if (proj.special)
			{
				inst.serverState.special = 1;
				proj.special = true;
			}
			else
			{
				inst.serverState.ammo--;
				proj.special = false;
			}
		}
	}

	PhysScene::RaycastDesc rcdesc;

	for (int i = 0; i < projectiles.size(); i++)
	{
		Projectile& proj = projectiles[i];

		float len = Projectile::speed * dt * 1.01f;

		proj.lifetime -= dt;

		float r = 0.35f;

		if (proj.state == 0)
		{
			if (proj.lifetime > 0.0f)
			{
				rcdesc.origin = proj.pos;
				rcdesc.dir = proj.dir;
				rcdesc.length = len;
				rcdesc.group = 1;

				if (PScene()->RayCast(rcdesc))
				{
					proj.pos = rcdesc.hitPos;

					proj.state = 1;
					proj.lifetime = Projectile::splashTime;

					if (proj.special)
					{
						for (int i = 0; i < 5; i++)
						{
							proj.claster_pos[i] = proj.pos + Vector3(-2.0f + 4.0f * Math::Rand(), 0.0f, -2.0f + 4.0f * Math::Rand());
							AddSplash(proj.claster_pos[i], Projectile::splashMaxRadius, 200);
						}
					}
					else
					{
						AddSplash(proj.pos, Projectile::splashMaxRadius, 400);
					}
				}
				else
				{
					proj.pos += proj.dir * len;
				}
			}
			else
			{
				proj.state = 1;
				proj.lifetime = Projectile::splashTime;
			}
		}
		else
		{
			if (proj.lifetime > 0.0f)
			{
				r = (1.0f - proj.lifetime / Projectile::splashTime) * Projectile::splashMaxRadius;
			}
			else
			{
				projectiles[i] = projectiles[projectiles.size() - 1];
				projectiles.pop_back();
				i--;
				continue;
			}
		}

		if (proj.special && proj.state == 1)
		{
			for (int i = 0; i < 5; i++)
			{
				core.render.DebugSphere(proj.claster_pos[i], COLOR_MAGNETA, r * 0.5f);
			}
		}
		else
		{
			core.render.DebugSphere(proj.pos, COLOR_RED, r);
		}
	}

	for (auto& bonus : bonuses)
	{
		if (bonus.cooldown > 0.0f)
		{
			bonus.cooldown -= dt;

			if (bonus.cooldown < 0.0f)
			{
				bonus.cooldown = -1.0f;
			}

			continue;
		}

		for (auto& inst : client->instances)
		{
			if (inst.serverState.hp <= 0)
			{
				continue;
			}

			if ((inst.serverState.pos - bonus.pos).Length2() < 9)
			{
				if (bonus.type == 0 && inst.serverState.hp < 100)
				{
					inst.serverState.hp = (int)fmin(inst.serverState.hp + 40, 100);
					bonus.cooldown = 10.0f;
				}
				else
				if (bonus.type == 1 && inst.serverState.ammo < 100)
				{
					inst.serverState.ammo = (int)fmin(inst.serverState.ammo + 40, 100);
					bonus.cooldown = 10.0f;
				}
				else
				if (bonus.type == 2 && inst.serverState.special < 100)
				{
					inst.serverState.special = (int)fmin(inst.serverState.special + 40, 100);
					bonus.cooldown = 10.0f;
				}
			}
		}

		Color colors[] = { COLOR_GREEN, COLOR_CYAN, COLOR_YELLOW };

		core.render.DebugSphere(bonus.pos, colors[bonus.type], 0.5f);
	}

}

void Tank::AddSplash(Vector3& pos, float radius, float force)
{
	vector<Scene::Group*> out_group;
	GetScene()->GetGroup(out_group, "PhysBox");

	for (auto group : out_group)
	{
		for (int i = 0; i < group->objects.size(); i++)
		{
			PhysBox* box = (PhysBox*)group->objects[i];

			if (box->isStatic)
			{
				continue;
			}

			Vector3 dir = box->Trans()->Pos() - pos;
			float len = dir.Normalize();

			if (len < radius)
			{
				box->body.body->AddForceAt(pos, dir * len / radius * force);
			}
		}
	}
}