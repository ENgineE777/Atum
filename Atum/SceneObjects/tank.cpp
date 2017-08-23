#include "Tank.h"
#include "Services/Controls/Controls.h"

CLASSDECLDECL(SceneObject, Tank)

META_DATA_DESC(Tank)
META_DATA_DESC_END()

using namespace physx;

extern PxFoundation*							mFoundation;
extern PxPhysics*								mPhysics;
extern PxCooking* mCooking;
extern PxScene*								mScene;

extern PxHeightField*  heightField;

extern PxDefaultErrorCallback gDefaultErrorCallback;
extern PxDefaultAllocator gDefaultAllocatorCallback;
extern PxTolerancesScale tolerancesScale;
extern PxSimulationFilterShader gDefaultFilterShader;

PxControllerManager* manager;
PxController* controller;

extern PxHeightFieldSample* hsamples;

float Tank::Projectile::maxTimeLife = 4.0f;
float Tank::Projectile::speed = 50.0f;
float Tank::Projectile::splashTime = 0.35f;
float Tank::Projectile::splashMaxRadius = 5.0f;

void Tank::Init()
{
	mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, tolerancesScale, true);

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(tolerancesScale));

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

	PxTolerancesScale scl = mPhysics->getTolerancesScale();
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = gDefaultFilterShader;
	}

	mScene = mPhysics->createScene(sceneDesc);

	manager = PxCreateControllerManager(*mScene);

	//mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);

	PxMaterial* mMaterial = mPhysics->createMaterial(0.5, 0.5, 0.95);

	PxCapsuleControllerDesc desc;
	desc.height = 1.0f;
	desc.radius = 1.0f;
	desc.position = PxExtendedVec3(transform.Pos().x, transform.Pos().y, transform.Pos().z);
	desc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
	desc.density = 1.0f;
	desc.slopeLimit = cosf(RADIAN * 60.0f);
	desc.contactOffset = 0.1;
	desc.stepOffset = 0.5;
	desc.scaleCoeff = 0.8;
	desc.material = mMaterial;


	controller = manager->createController(desc);

	hsamples = new PxHeightFieldSample[terrain->hwidth * terrain->hheight];

	for (PxU32 x = 0; x < terrain->hwidth; x++)
		for (PxU32 y = 0; y < terrain->hheight; y++)
		{
			hsamples[x + y*terrain->hwidth].height = PxI16(terrain->hmap[((x)* terrain->hwidth + y)]);
			hsamples[x + y*terrain->hwidth].setTessFlag();
			hsamples[x + y*terrain->hwidth].materialIndex0 = 1;
			hsamples[x + y*terrain->hwidth].materialIndex1 = 1;
		}

	PxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightFieldDesc.nbColumns = terrain->hwidth;
	heightFieldDesc.nbRows = terrain->hheight;
	heightFieldDesc.samples.data = hsamples;
	heightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);

	heightField = mCooking->createHeightField(heightFieldDesc, mPhysics->getPhysicsInsertionCallback());

	PxTransform pose = PxTransform(PxVec3(-terrain->hwidth * 0.5f * terrain->scaleh, 0.0f, -terrain->hheight * 0.5f * terrain->scaleh), PxQuat(PxIdentity));

	PxRigidActor* hf = mPhysics->createRigidStatic(pose);

	const PxMaterial* mMat = mPhysics->createMaterial(0.9f, 0.9f, 0.001f);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), terrain->scalev, terrain->scaleh, terrain->scaleh);
	PxShape* hfShape = hf->createShape(hfGeom, *mMat);
	mScene->addActor(*hf);

	angles = Vector(0.0f);

	Scene::Group& bgroup = owner->GetGroup("PhysBox");
	boxes.resize(bgroup.objects.size());

	for (int i = 0; i < bgroup.objects.size(); i++)
	{
		boxes[i].obj = (PhysBox*)bgroup.objects[i];

		PxReal density = 1.0f;

		Quaternion q(boxes[i].obj->Trans());

		PxTransform transform(PxVec3(boxes[i].obj->Trans().Pos().x, boxes[i].obj->Trans().Pos().y, boxes[i].obj->Trans().Pos().z), PxQuat(q.x, q.y, q.z, q.w));

		PxVec3 dimensions(boxes[i].obj->sizeX * 0.5f, boxes[i].obj->sizeY * 0.5f, boxes[i].obj->sizeZ * 0.5f);
		PxBoxGeometry geometry(dimensions);

		if (boxes[i].obj->isStatic)
		{
			PxRigidStatic* plane = mPhysics->createRigidStatic(transform);
			PxShape* shape = plane->createShape(geometry, *mMaterial);
			mScene->addActor(*plane);

			boxes[i].box = NULL;
		}
		else
		{
			boxes[i].box = PxCreateDynamic(*mPhysics, transform, geometry, *mMaterial, density);
			boxes[i].box->setLinearVelocity(PxVec3(0, 0, 0));
			mScene->addActor(*boxes[i].box);
		}
	}

	move_speed = 0.0f;
	strafe_speed = 0.0f;

	accum_dt = 0.0f;
	showDebug = false;
}

void Tank::Stop()
{
	mScene->release();

	boxes.clear();

	heightField->release();

	delete[] hsamples;

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

	accum_dt += dt;

	if (accum_dt > 0.5f)
	{
		accum_dt = 0.5f;
	}

	while (accum_dt > physStep)
	{
		mScene->simulate(fmin(dt, physStep));

		while (!mScene->fetchResults())
		{
			// do something useful
		}

		accum_dt -= physStep;
	}

	const PxRenderBuffer& rb = mScene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		render.DebugLine(Vector(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
						 Vector(line.pos0.x, line.pos0.y, line.pos0.z), COLOR_GREEN, false);
	}

	PxExtendedVec3 cpos = controller->getFootPosition();

	Matrix mat;
	mat.RotateY(angles.z);
	mat.Pos() = Vector(cpos.x, cpos.y, cpos.z);


	view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 55, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));
	proj.BuildProjection(45.0f * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 1.0f, 1000.0f);

	render.SetTransform(Render::View, view);
	render.SetTransform(Render::Projection, proj);

	float tower_angel = angles.z;

	Vector target_pt = 0.0f;

	{
		PxRaycastBuffer hit;

		Vector2 screepos = Vector2((float)controls.GetAliasValue(alias_rotate_x, false) / (float)render.GetDevice()->GetWidth(),
									(float)controls.GetAliasValue(alias_rotate_y, false) / (float)render.GetDevice()->GetHeight());

		//screepos.x = screepos.y = 0.25f;

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

		if (mScene->raycast(PxVec3(camPos.x, camPos.y, camPos.z), PxVec3(dir.x, dir.y, dir.z), 500, hit))
		{
			target_pt = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);
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
	//box->addForce(PxVec3(dir.x * move_speed, 0.0f, dir.z * move_speed), PxForceMode::eFORCE, true);
	//box->addTorque(PxVec3(0, strafe_speed, -0), PxForceMode::eFORCE, true);

	const PxU32 flags = controller->move(PxVec3(dir.x * move_speed * dt,-9.8f* dt, dir.z * move_speed * dt), 0.01f, dt, PxControllerFilters(), NULL);

	if (showDebug)
	{
		render.DebugBox(mat, COLOR_YELLOW, Vector(3.0, 1.0, 2.0f));
	}
	else
	{
		PxRaycastBuffer hit;
		Vector org = mat.Pos();

		float under = 1.0f;


		org.y += under;

		Vector p1 = org + mat.Vx() * 1.75f;

		if (mScene->raycast(PxVec3(p1.x, p1.y, p1.z), PxVec3(0, -1, 0), under + 2.0f, hit))
		{
			p1 = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);
		}
		else
		{
			p1.y -= under;
		}

		Vector p2 = org - mat.Vx() * 1.75f - mat.Vz();

		if (mScene->raycast(PxVec3(p2.x, p2.y, p2.z), PxVec3(0, -1, 0), under + 2.0f, hit))
		{
			p2 = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);
		}
		else
		{
			p2.y -= under;
		}

		Vector p3 = org - mat.Vx() * 1.75f + mat.Vz();

		if (mScene->raycast(PxVec3(p3.x, p3.y, p3.z), PxVec3(0, -1, 0), under + 2.0f, hit))
		{
			p3 = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);
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

		//mat = Matrix().RotateZ(angles.y) * Matrix().RotateY(-angles.x) * Matrix().Move(hover_model.locator + mat.Pos());

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
		if (!boxes[i].box)
		{
			continue;
		}

		PxTransform pT = boxes[i].box->getGlobalPose();
		PxMat33 m = PxMat33(pT.q);

		Matrix mat;
		mat.Vx() = Vector(m.column0.x, m.column0.y, m.column0.z);
		mat.Vy() = Vector(m.column1.x, m.column1.y, m.column1.z);
		mat.Vz() = Vector(m.column2.x, m.column2.y, m.column2.z);

		mat.Pos() = Vector(pT.p.x, pT.p.y, pT.p.z);

		boxes[i].obj->Trans() = mat;
	}

	for (int i = 0; i < projectiles.size(); i++)
	{
		Projectile& proj = projectiles[i];

		Vector delta = proj.dir * Projectile::speed * dt;

		PxRaycastBuffer hit;

		proj.lifetime -= dt;

		float r = 0.35f;

		if (proj.state == 0)
		{
			if (proj.lifetime > 0.0f)
			{
				if (mScene->raycast(PxVec3(proj.pos.x, proj.pos.y, proj.pos.z), PxVec3(proj.dir.x, proj.dir.y, proj.dir.z), delta.Length(), hit))
				{
					proj.pos = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);

					proj.state = 1;
					proj.lifetime = Projectile::splashTime;

					AddSplash(proj.pos, Projectile::splashMaxRadius, 400);
				}
				else
				{
					proj.pos += proj.dir * Projectile::speed * dt;
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
		if (!boxes[i].box)
		{
			continue;
		}

		Vector dir = boxes[i].obj->Trans().Pos() - pos;
		float len = dir.Normalize();

		if (len < radius)
		{
			PxRigidBodyExt::addForceAtPos(*boxes[i].box, PxVec3(dir.x, dir.y, dir.z) * len / radius * force,
										  PxVec3(pos.x, pos.y, pos.z), PxForceMode::eIMPULSE, true);

		}
	}
}