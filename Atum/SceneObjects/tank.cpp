#include "Tank.h"
#include "Services/Controls/Controls.h"

CLASSDECLDECL(SceneObject, Tank)

META_DATA_DESC(Tank)
META_DATA_DESC_END()

float Tank::Projectile::maxTimeLife = 4.0f;
float Tank::Projectile::speed = 50.0f;
float Tank::Projectile::splashTime = 0.35f;
float Tank::Projectile::splashMaxRadius = 5.0f;

void Tank::Init()
{
	owner->AddToGroup(this, "Tank");

	Tasks()->AddTask(0, this, (Object::Delegate)&Tank::Update);
}

void Tank::Play()
{

	PhysControllerDesc cdesc;
	cdesc.height = 1.0f;
	cdesc.radius = 1.0f;
	cdesc.pos = transform.Pos();
	cdesc.slopeLimit = cosf(RADIAN * 60.0f);

	state.pos = transform.Pos();

	controller = PScene()->CreateController(cdesc);

	shoot_cooldown = 0.0f;

	move_speed = 0.0f;
	strafe_speed = 0.0f;

	showDebug = false;
}

void Tank::Stop()
{
	projectiles.clear();
}

void Tank::Update(float dt)
{
	if (!Playing())
	{
		return;
	}

	float speed = 25.0f;

	Matrix mat;
	mat.RotateY(state.angle);
	controller->GetPosition(mat.Pos());

	state.pos = mat.Pos();

	Vector target_pt = 0.0f;

	Vector dir = mat.Vx();
	dir.y = 0;
	dir.Normalize();

	if (state.up == 1)
	{
		move_speed += 20 * dt;

		if (move_speed > 30)
		{
			move_speed = 30;
		}
	}
	else
	if (state.up == -1)
	{
		move_speed -= 15 * dt;

		if (move_speed < -20)
		{
			move_speed = -20;
		}
	}
	else
	{
		if (move_speed > 0.1f)
		{
			move_speed -= 30 * dt;

			if (move_speed < 0)
			{
				move_speed = 0;
			}
		}
		else
		if (move_speed < 0.1f)
		{
			move_speed += 30 * dt;

			if (move_speed > 0)
			{
				move_speed = 0;
			}
		}
	}

	if (state.rotate == 1)
	{
		strafe_speed += 15 * dt;

		if (strafe_speed > 3)
		{
			strafe_speed = 3;
		}
	}
	else
	if (state.rotate == -1)
	{
		strafe_speed -= 15 * dt;

		if (strafe_speed < -3)
		{
			strafe_speed = -3;
		}
	}
	else
	{
		if (strafe_speed > 0.1f)
		{
			strafe_speed -= 15 * dt;

			if (strafe_speed < 0)
			{
				strafe_speed = 0;
			}
		}
		else
		if (strafe_speed < 0.1f)
		{
			strafe_speed += 15 * dt;

			if (strafe_speed > 0)
			{
				strafe_speed = 0;
			}
		}
	}

	state.angle += strafe_speed * dt;

	dir *= move_speed;
	dir.y = -9.8f;
	dir *= dt;

	controller->Move(dir);

	if (shoot_cooldown > 0.0f)
	{
		shoot_cooldown -= dt;
	}

	if (state.fired && shoot_cooldown <= 0.0f)
	{
		projectiles.push_back(Projectile());
		Projectile& proj = projectiles[projectiles.size() - 1];

		proj.pos = state.gun_pos;
		proj.dir = state.gun_dir;
		proj.lifetime = Projectile::maxTimeLife;
		proj.state = 0;

		shoot_cooldown = 1.5f;
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

				if (PScene()->RayCast(rcdesc))
				{
					proj.pos = rcdesc.hitPos;

					proj.state = 1;
					proj.lifetime = Projectile::splashTime;

					AddSplash(proj.pos, Projectile::splashMaxRadius, 400);
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

		render.DebugSphere(proj.pos, COLOR_RED, r);
	}
}

void Tank::AddSplash(Vector& pos, float radius, float force)
{
	Scene::Group& bgroup = owner->GetGroup("PhysBox");

	for (int i = 0; i < bgroup.objects.size(); i++)
	{
		PhysBox* box = (PhysBox*)bgroup.objects[i];

		if (box->isStatic)
		{
			continue;
		}

		Vector dir = box->Trans().Pos() - pos;
		float len = dir.Normalize();

		if (len < radius)
		{
			box->obj->AddForceAt(pos, dir * len / radius * force);
		}
	}
}