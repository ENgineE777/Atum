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
	alias_forward = controls.GetAlias("MOVE_FORWARD");
	alias_strafe = controls.GetAlias("MOVE_STRAFE");
	alias_fast = controls.GetAlias("MOVE_FAST");
	alias_rotate_active = controls.GetAlias("ROTATE_ACTIVE");
	alias_rotate_x = controls.GetAlias("ROTATE_X");
	alias_rotate_y = controls.GetAlias("ROTATE_Y");

	hover_model.LoadModelMS3D("Media//tank_base.ms3d");

	hover_drawer = new Model::Drawer;
	hover_drawer->Init(&hover_model, RenderTasks());

	Vector4 color(0.0f, 1.0f, 1.0f);
	hover_drawer->SetColor(color);

	tower_model.LoadModelMS3D("Media//tank_tower.ms3d");

	tower_drawer = new Model::Drawer;
	tower_drawer->Init(&tower_model, RenderTasks());

	tower_drawer->SetColor(color);

	gun_model.LoadModelMS3D("Media//tank_gun.ms3d");

	gun_drawer = new Model::Drawer;
	gun_drawer->Init(&gun_model, RenderTasks());

	gun_drawer->SetColor(color);

	Tasks()->AddTask(0, this, (Object::Delegate)&Tank::Update);
}

void Tank::Play()
{
	Scene::Group& group = owner->GetGroup("Terrain");

	if (group.objects.size() > 0)
	{
		terrain = (Terrain*)group.objects[0];
	}

	pscene = physics.CreateScene();

	PhysControllerDesc cdesc;
	cdesc.height = 1.0f;
	cdesc.radius = 1.0f;
	cdesc.pos = transform.Pos();
	cdesc.slopeLimit = cosf(RADIAN * 60.0f);

	controller = pscene->CreateController(cdesc);

	PhysHeightmapDesc hdesc;
	hdesc.width = terrain->hwidth;
	hdesc.height = terrain->hheight;
	hdesc.hmap = terrain->hmap;
	hdesc.scale = Vector2(terrain->scaleh, terrain->scalev);

	hm = pscene->CreateHeightmap(hdesc);

	angles = Vector(0.0f);

	Scene::Group& bgroup = owner->GetGroup("PhysBox");
	boxes.resize(bgroup.objects.size());

	for (int i = 0; i < bgroup.objects.size(); i++)
	{
		boxes[i].box = (PhysBox*)bgroup.objects[i];
		boxes[i].obj = pscene->CreateBox(Vector(boxes[i].box->sizeX * 0.5f, boxes[i].box->sizeY * 0.5f, boxes[i].box->sizeZ * 0.5f),
										 boxes[i].box->Trans(), boxes[i].box->isStatic);
	}

	move_speed = 0.0f;
	strafe_speed = 0.0f;

	showDebug = false;
}

void Tank::Stop()
{
	physics.DestroyScene(pscene);

	for (auto box : boxes)
	{
		RELEASE(box.obj);
	}

	boxes.clear();

	RELEASE(hm);

	projectiles.clear();

	hover_drawer->Show(true);
	tower_drawer->Show(true);
	gun_drawer->Show(true);
}

void Tank::Update(float dt)
{
	if (!Playing())
	{
		hover_drawer->SetTransform(transform);
		tower_drawer->SetTransform(transform);
		gun_drawer->SetTransform(transform);

		return;
	}

	if (controls.GetAliasState(alias_rotate_active, false, Controls::Active))
	{
		angles.x -= controls.GetAliasValue(alias_rotate_x, true) * 0.01f;
		angles.y -= controls.GetAliasValue(alias_rotate_y, true) * 0.01f;

		if (angles.y > HALF_PI)
		{
			angles.y = HALF_PI;
		}

		if (angles.y < -HALF_PI)
		{
			angles.y = -HALF_PI;
		}
	}

	angles.y = -HALF_PI;

	Vector dir = Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x));

	float speed = 25.0f;

	Matrix mat;
	mat.RotateY(angles.z);
	controller->GetPosition(mat.Pos());


	view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 55, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));
	proj.BuildProjection(45.0f * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 1.0f, 1000.0f);

	render.SetTransform(Render::View, view);
	render.SetTransform(Render::Projection, proj);

	float tower_angel = angles.z;

	Vector target_pt = 0.0f;

	PhysScene::RaycastDesc rcdesc;

	{
		Vector2 screepos = Vector2((float)controls.GetAliasValue(alias_rotate_x, false) / (float)render.GetDevice()->GetWidth(),
									(float)controls.GetAliasValue(alias_rotate_y, false) / (float)render.GetDevice()->GetHeight());

		Vector v;
		v.x = (2.0f * screepos.x - 1) / proj._11;
		v.y = -(2.0f * screepos.y - 1) / proj._22;
		v.z = 1.0f;

		Matrix inv_view = view;
		inv_view.InverseComplette();

		Vector camPos = inv_view.Pos();

		Vector dir;
		dir.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		dir.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		dir.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		
		Vector screen = camPos + dir;
		dir.Normalize();

		rcdesc.origin = camPos;
		rcdesc.dir = dir;
		rcdesc.length = 500;

		if (pscene->RayCast(rcdesc))
		{
			target_pt = rcdesc.hitPos;
			render.DebugSphere(target_pt, COLOR_RED, 0.5f);

			dir = target_pt - mat.Pos();
			dir.Normalize();

			tower_angel = acosf(dir.Dot(Vector(1,0,0)));

			if (dir.Dot(Vector(0, 0, 1)) > 0.0f)
			{
				tower_angel = PI * 2 - tower_angel;
			}
		}
	}

	dir = mat.Vx();
	dir.y = 0;
	dir.Normalize();

	if (controls.DebugKeyPressed("KEY_G"))
	{
		showDebug = !showDebug;
		hover_drawer->Show(!showDebug);
		tower_drawer->Show(!showDebug);
		gun_drawer->Show(!showDebug);
	}

	if (controls.DebugKeyPressed("KEY_W", Controls::Active))
	{
		move_speed += 20 * dt;

		if (move_speed > 30)
		{
			move_speed = 30;
		}
	}
	else
	if (controls.DebugKeyPressed("KEY_S", Controls::Active))
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

	if (controls.DebugKeyPressed("KEY_D", Controls::Active))
	{
		strafe_speed += 15 * dt;

		if (strafe_speed > 3)
		{
			strafe_speed = 3;
		}
	}
	else
	if (controls.DebugKeyPressed("KEY_A", Controls::Active))
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

	angles.z += strafe_speed * dt;

	dir *= move_speed;
	dir.y = -9.8f;
	dir *= dt;

	controller->Move(dir);

	if (showDebug)
	{
		render.DebugBox(mat, COLOR_YELLOW, Vector(3.0, 1.0, 2.0f));
	}
	else
	{
		float under = 1.0f;

		rcdesc.dir = Vector(0, -1, 0);
		rcdesc.length = under + 2.0f;

		Vector org = mat.Pos();
		org.y += under;

		Vector p1 = org + mat.Vx() * 1.75f;
		rcdesc.origin = p1;

		if (pscene->RayCast(rcdesc))
		{
			p1 = rcdesc.hitPos;
		}
		else
		{
			p1.y -= under;
		}

		Vector p2 = org - mat.Vx() * 1.75f - mat.Vz();
		rcdesc.origin = p2;

		if (pscene->RayCast(rcdesc))
		{
			p2 = rcdesc.hitPos;
		}
		else
		{
			p2.y -= under;
		}

		Vector p3 = org - mat.Vx() * 1.75f + mat.Vz();
		rcdesc.origin = p3;

		if (pscene->RayCast(rcdesc))
		{
			p3 = rcdesc.hitPos;
		}
		else
		{
			p3.y -= under;
		}

		mat.Vz() = p3 - p2;
		mat.Vz().Normalize();

		mat.Vx() = p1 - (p3 + p2)*0.5f;
		mat.Vx().Normalize();

		mat.Vy() = mat.Vz().Cross(mat.Vx());

		render.DebugSphere(p1, COLOR_RED, 0.5f);
		render.DebugSphere(p2, COLOR_RED, 0.5f);
		render.DebugSphere(p3, COLOR_RED, 0.5f);


		Matrix mdl = mat;
		hover_drawer->SetTransform(mdl);

		mdl = Matrix().RotateY(tower_angel - angles.z) * Matrix().Move(hover_model.locator) * mdl;
		tower_drawer->SetTransform(mdl);

		mdl = Matrix().Move(tower_model.locator) * mdl;
		gun_drawer->SetTransform(mdl);

		if (controls.DebugKeyPressed("MS_BTN0"))
		{
			mdl = Matrix().Move(gun_model.locator) * mdl;

			projectiles.push_back(Projectile());
			Projectile& proj = projectiles[projectiles.size() - 1];

			proj.pos = mdl.Pos();
			proj.dir = target_pt - proj.pos;
			proj.dir.Normalize();
			proj.lifetime = Projectile::maxTimeLife;
			proj.state = 0;
		}
	}

	for (int i = 0; i < boxes.size(); i++)
	{
		if (boxes[i].box->isStatic)
		{
			continue;
		}

		boxes[i].obj->GetTransform(boxes[i].box->Trans());
	}

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

				if (pscene->RayCast(rcdesc))
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
	for (int i = 0; i < boxes.size(); i++)
	{
		if (boxes[i].box->isStatic)
		{
			continue;
		}

		Vector dir = boxes[i].box->Trans().Pos() - pos;
		float len = dir.Normalize();

		if (len < radius)
		{
			boxes[i].obj->AddForceAt(pos, dir * len / radius * force);
		}
	}
}