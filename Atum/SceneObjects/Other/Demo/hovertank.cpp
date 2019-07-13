#include "hovertank.h"
#include "Services/Controls/Controls.h"

CLASSREG(SceneObject, HoverTank, "HoverTank")

META_DATA_DESC(HoverTank)
BASE_SCENE_OBJ_PROP(HoverTank)
META_DATA_DESC_END()

using namespace physx;

PxFoundation* mFoundation;
PxPhysics* mPhysics;
PxCooking* mCooking;
PxScene* mScene;

PxRigidDynamic* box;
PxHeightField*  heightField;

PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxTolerancesScale tolerancesScale;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;


PxHeightFieldSample* hsamples;

float HoverTank::Projectile::maxTimeLife = 4.0f;
float HoverTank::Projectile::speed = 50.0f;
float HoverTank::Projectile::splashTime = 0.35f;
float HoverTank::Projectile::splashMaxRadius = 5.0f;

Matrix& HoverTank::Trans()
{
	return transform;
}

bool HoverTank::Is3DObject()
{
	return true;
}

void HoverTank::Init()
{
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, tolerancesScale, true);

#ifdef PLATFORM_PC
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(tolerancesScale));
#endif

	alias_forward = core.controls.GetAlias("MOVE_FORWARD");
	alias_strafe = core.controls.GetAlias("MOVE_STRAFE");
	alias_fast = core.controls.GetAlias("MOVE_FAST");
	alias_rotate_active = core.controls.GetAlias("ROTATE_ACTIVE");
	alias_rotate_x = core.controls.GetAlias("ROTATE_X");
	alias_rotate_y = core.controls.GetAlias("ROTATE_Y");

	hover_model.LoadModelMS3D("Media//hover.ms3d");

	hover_drawer = new Model::Drawer;
	hover_drawer->Init(&hover_model, RenderTasks(false));

	Vector4 color(0.0f, 1.0f, 1.0f);
	hover_drawer->SetColor(color);

	tower_model.LoadModelMS3D("Media//tower.ms3d");

	tower_drawer = new Model::Drawer;
	tower_drawer->Init(&tower_model, RenderTasks(false));

	tower_drawer->SetColor(color);

	gun_model.LoadModelMS3D("Media//gun.ms3d");

	gun_drawer = new Model::Drawer;
	gun_drawer->Init(&gun_model, RenderTasks(false));

	gun_drawer->SetColor(color);

	Tasks(false)->AddTask(0, this, (Object::Delegate)&HoverTank::Update);
}

bool HoverTank::Play()
{
	vector<Scene::Group*> out_group;
	GetScene()->GetGroup(out_group, "Terrain");

	for (auto group : out_group)
	{
		if (group->objects.size() > 0)
		{
			terrain = (Terrain*)group->objects[0];
			break;
		}
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

	//mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);

	PxMaterial* mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.95f);

	PxReal density = 1.0f;

	Quaternion q(transform);

	PxTransform trans(PxVec3(transform.Pos().x, transform.Pos().y, transform.Pos().z), PxQuat(q.x, q.y, q.z, q.w));
	PxVec3 dimensions(1.5, 0.5, 1.0);
	PxBoxGeometry geometry(dimensions);

	box = PxCreateDynamic(*mPhysics, trans, geometry, *mMaterial, density);
	box->setAngularDamping(5.75f);
	box->setLinearDamping(1.25f);
	box->setLinearVelocity(PxVec3(0, 0, 0));
	mScene->addActor(*box);

	hsamples = new PxHeightFieldSample[terrain->hwidth * terrain->hheight];

	for (int x = 0; x < terrain->hwidth; x++)
		for (int y = 0; y < terrain->hheight; y++)
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
	PxShape* hfShape = mPhysics->createShape(hfGeom, *mMat);
	mScene->addActor(*hf);

	angles = Vector2(0.0f, 0.0f);

	vector<Scene::Group*> out_group_pb;
	GetScene()->GetGroup(out_group_pb, "PhysBox");

	for (auto group : out_group_pb)
	{
		for (int i = 0; i < group->objects.size(); i++)
		{
			boxes.push_back(Box());

			Box& box = boxes[boxes.size() - 1];

			box.obj = (PhysBox*)group->objects[i];

			PxReal density = 1.0f;

			Quaternion q(box.obj->Trans());

			PxTransform transform(PxVec3(box.obj->Trans().Pos().x, box.obj->Trans().Pos().y, box.obj->Trans().Pos().z), PxQuat(q.x, q.y, q.z, q.w));

			PxVec3 dimensions(box.obj->sizeX * 0.5f, box.obj->sizeY * 0.5f, box.obj->sizeZ * 0.5f);
			PxBoxGeometry geometry(dimensions);

			if (box.obj->isStatic)
			{
				PxRigidStatic* plane = mPhysics->createRigidStatic(transform);
				PxShape* shape = mPhysics->createShape(geometry, *mMaterial);
				mScene->addActor(*plane);

				box.box = nullptr;
			}
			else
			{
				box.box = PxCreateDynamic(*mPhysics, transform, geometry, *mMaterial, density);
				box.box->setLinearVelocity(PxVec3(0, 0, 0));
				mScene->addActor(*box.box);
			}
		}
	}

	move_speed = 0.0f;
	strafe_speed = 0.0f;

	accum_dt = 0.0f;
	showDebug = false;

	return true;
}

void HoverTank::Release()
{
	if (mScene)
	{
		mScene->release();
	}

	boxes.clear();

	if (heightField)
	{
		heightField->release();
	}

	if (hsamples)
	{
		delete[] hsamples;
	}

	projectiles.clear();

	if (hover_drawer)
	{
		delete hover_drawer;
	}

	if (tower_drawer)
	{
		delete tower_drawer;
	}

	if (gun_drawer)
	{
		delete gun_drawer;
	}

	SceneObject::Release();
}

void HoverTank::Update(float dt)
{
	if (!GetScene()->Playing())
	{
		hover_drawer->SetTransform(transform);
		tower_drawer->SetTransform(transform);
		gun_drawer->SetTransform(transform);

		return;
	}

	if (core.controls.GetAliasState(alias_rotate_active, Controls::Active))
	{
		angles.x -= core.controls.GetAliasValue(alias_rotate_x, true) * 0.01f;
		angles.y -= core.controls.GetAliasValue(alias_rotate_y, true) * 0.01f;

		if (angles.y > HALF_PI)
		{
			angles.y = HALF_PI;
		}

		if (angles.y < -HALF_PI)
		{
			angles.y = -HALF_PI;
		}
	}

	Vector dir = Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x));

	float speed = 25.0f;

	Matrix mat;

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
		core.render.DebugLine(Vector(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
		                      Vector(line.pos0.x, line.pos0.y, line.pos0.z), COLOR_GREEN, false);
	}

	PxTransform pT = box->getGlobalPose();

	PxMat33 m = PxMat33(pT.q);
	mat.Identity();
	mat.Vx() = Vector(m.column0.x, m.column0.y,m.column0.z);
	mat.Vy() = Vector(m.column1.x, m.column1.y, m.column1.z);
	mat.Vz() = Vector(m.column2.x, m.column2.y, m.column2.z);

	mat.Pos() = Vector(pT.p.x, pT.p.y, pT.p.z);

	AddHover(mat, -mat.Vy() * 0.5001f + mat.Vx() * 1.5f + mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f + mat.Vx() * 1.5f - mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f - mat.Vx() * 1.5f + mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f - mat.Vx() * 1.5f - mat.Vz());

	view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 15, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));
	proj.BuildProjection(45.0f * RADIAN, 600.0f / 800.0f, 1.0f, 1000.0f);

	core.render.SetTransform(Render::View, view);
	core.render.SetTransform(Render::Projection, proj);

	dir = mat.Vx();
	dir.y = 0;
	dir.Normalize();

	if (core.controls.DebugKeyPressed("KEY_G"))
	{
		showDebug = !showDebug;
		hover_drawer->Show(!showDebug);
		tower_drawer->Show(!showDebug);
		gun_drawer->Show(!showDebug);
	}

	if (core.controls.DebugKeyPressed("KEY_W", Controls::Active))
	{
		move_speed += 200 * dt;

		if (move_speed > 300)
		{
			move_speed = 300;
		}
	}
	else
	if (core.controls.DebugKeyPressed("KEY_S", Controls::Active))
	{
		move_speed -= 150 * dt;

		if (move_speed < -200)
		{
			move_speed = -200;
		}
	}
	else
	{
		if (move_speed > 0.1f)
		{
			move_speed -= 300 * dt;

			if (move_speed < 0)
			{
				move_speed = 0;
			}
		}
		else
		if (move_speed < 0.1f)
		{
			move_speed += 300 * dt;

			if (move_speed > 0)
			{
				move_speed = 0;
			}
		}
	}

	if (core.controls.DebugKeyPressed("KEY_D", Controls::Active))
	{
		strafe_speed += 75 * dt;

		if (strafe_speed > 100)
		{
			strafe_speed = 100;
		}
	}
	else
	if (core.controls.DebugKeyPressed("KEY_A", Controls::Active))
	{
		strafe_speed -= 75 * dt;

		if (strafe_speed < -100)
		{
			strafe_speed = -100;
		}
	}
	else
	{
		if (strafe_speed > 0.1f)
		{
			strafe_speed -= 40 * dt;

			if (strafe_speed < 0)
			{
				strafe_speed = 0;
			}
		}
		else
			if (strafe_speed < 0.1f)
			{
				strafe_speed += 40 * dt;

				if (strafe_speed > 0)
				{
					strafe_speed = 0;
				}
			}
	}

	box->addForce(PxVec3(dir.x * move_speed, 0.0f, dir.z * move_speed), PxForceMode::eFORCE, true);
	box->addTorque(PxVec3(0, strafe_speed, -0), PxForceMode::eFORCE, true);

	if (showDebug)
	{
		core.render.DebugBox(mat, COLOR_YELLOW, Vector(3.0, 1.0, 2.0f));
	}
	else
	{
		mat.Pos() -= mat.Vy();
		hover_drawer->SetTransform(mat);

		mat = Matrix().RotateZ(angles.y) * Matrix().RotateY(-angles.x) * Matrix().Move(hover_model.locator + mat.Pos());// *mat;
		tower_drawer->SetTransform(mat);

		mat = Matrix().Move(tower_model.locator) * mat;
		gun_drawer->SetTransform(mat);

		mat = Matrix().Move(gun_model.locator) * mat;
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

		core.render.DebugSphere(proj.pos, COLOR_RED, r);
	}

	if (core.controls.DebugKeyPressed("MS_BTN0"))
	{
		projectiles.push_back(Projectile());
		Projectile& proj = projectiles[projectiles.size() - 1];

		proj.pos = mat.Pos();
		proj.dir = mat.Vx();
		proj.lifetime = Projectile::maxTimeLife;
		proj.state = 0;
	}
}

void HoverTank::AddSplash(Vector& pos, float radius, float force)
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

void HoverTank::AddHover(Matrix& mat, Vector offset)
{
	PxRaycastBuffer hit;

	float len = 1.5f;

	Vector org = offset + mat.Pos();
	bool status = mScene->raycast(PxVec3(org.x, org.y, org.z), PxVec3(-mat.Vy().x, -mat.Vy().y, -mat.Vy().z), len, hit);

	if (status && hit.block.actor != box)
	{
		Vector hit_pos(hit.block.position.x, hit.block.position.y, hit.block.position.z);

		float k = 1.0f - (org - hit_pos).Length() / len;

		if (showDebug)
		{
			core.render.DebugLine(org, COLOR_RED, org - Vector(0, len, 0.0f) * (1 - k), COLOR_RED, false);
		}

		PxRigidBodyExt::addForceAtPos(*box, hit.block.normal * k * 85,
		                              hit.block.position, PxForceMode::eFORCE, true);
	}
	else
	{
		//render.DebugLine(org, COLOR_YELLOW, org - Vector(0, len, 0.0f), COLOR_YELLOW, false);
	}
}