#include "hovertank.h"
#include "Services/Controls/Controls.h"

using namespace physx;

PxFoundation*							mFoundation;
PxPhysics*								mPhysics;
PxCooking* mCooking;
PxScene*								mScene;

PxRigidDynamic* box;
PxShape** shapes;

PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxTolerancesScale tolerancesScale;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;


PxHeightFieldSample* hsamples;

float HoverTank::Projectile::maxTimeLife = 4.0f;
float HoverTank::Projectile::speed = 50.0f;
float HoverTank::Projectile::splashTime = 0.35f;
float HoverTank::Projectile::splashMaxRadius = 5.0f;

void HoverTank::Init(Terrain* set_terrain)
{
	terrain = set_terrain;

	mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, tolerancesScale, true);

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(tolerancesScale));

	PxTolerancesScale scl = mPhysics->getTolerancesScale();
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;

	mScene = mPhysics->createScene(sceneDesc);

	//mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);

	PxMaterial* mMaterial = mPhysics->createMaterial(0.5, 0.5, 0.95);

	/*PxReal d = 0.0f;
	PxTransform pose = PxTransform(PxVec3(0.0f, 5.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = mPhysics->createRigidStatic(pose);

	PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
	mScene->addActor(*plane);*/

	PxReal density = 1.0f;
	PxTransform transform(PxVec3(0.0f, 20.0f, 0.0f), PxQuat(PxIdentity));
	PxVec3 dimensions(1.5, 0.5, 1.0);
	PxBoxGeometry geometry(dimensions);

	PxRigidDynamic *actor = PxCreateDynamic(*mPhysics, transform, geometry, *mMaterial, density);
	actor->setAngularDamping(5.75f);
	actor->setLinearDamping(1.25f);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	mScene->addActor(*actor);

	box = actor;

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

	PxHeightField* heightField = mCooking->createHeightField(heightFieldDesc, mPhysics->getPhysicsInsertionCallback());

	PxTransform pose = PxTransform(PxVec3(-terrain->hwidth * 0.5f * terrain->hscale.x, 0.0f, -terrain->hheight * 0.5f * terrain->hscale.x), PxQuat(PxIdentity));

	PxRigidActor* hf = mPhysics->createRigidStatic(pose);

	const PxMaterial* mMat = mPhysics->createMaterial(0.9f, 0.9f, 0.001f);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), terrain->hscale.y, terrain->hscale.x, terrain->hscale.x);
	PxShape* hfShape = hf->createShape(hfGeom, *mMat);
	mScene->addActor(*hf);


	PxU32 nShapes = actor->getNbShapes();
	shapes = new PxShape*[nShapes];

	actor->getShapes(shapes, nShapes);

	proj.BuildProjection(45.0f * RADIAN, 600.0f / 800.0f, 1.0f, 1000.0f);

	angles = Vector2(0.0f, 0.0f);

	hover_model.LoadModelMS3D("Media//hover.ms3d");

	hover_drawer = new Model::Drawer;
	hover_drawer->Init(&hover_model);

	Vector4 color(0.0f, 1.0f, 1.0f);
	hover_drawer->SetColor(color);

	tower_model.LoadModelMS3D("Media//tower.ms3d");

	tower_drawer = new Model::Drawer;
	tower_drawer->Init(&tower_model);

	tower_drawer->SetColor(color);

	gun_model.LoadModelMS3D("Media//gun.ms3d");

	gun_drawer = new Model::Drawer;
	gun_drawer->Init(&gun_model);

	gun_drawer->SetColor(color);

	int num_boxes = 15;
	boxes.resize(num_boxes);

	for (int i = 0; i < num_boxes; i++)
	{
		boxes[i].size = Vector(rnd_range(1.0f, 3.0f), rnd_range(1.0f, 3.0f), rnd_range(1.0f, 3.0f));

		PxReal density = 1.0f;
		PxTransform transform(PxVec3(rnd_range(-40.0f, 40.0f), rnd_range(20.0f, 25.0f), rnd_range(-40.0f, 40.0f)), PxQuat(PxIdentity));
		PxVec3 dimensions(boxes[i].size.x * 0.5f, boxes[i].size.y * 0.5f, boxes[i].size.z * 0.5f);
		PxBoxGeometry geometry(dimensions);

		boxes[i].box = PxCreateDynamic(*mPhysics, transform, geometry, *mMaterial, density);
		boxes[i].box->setLinearVelocity(PxVec3(0, 0, 0));
		mScene->addActor(*boxes[i].box);
	}


	move_speed = 0.0f;
	strafe_speed = 0.0f;

	render.AddDelegate("camera", this, (Object::Delegate)&HoverTank::Update, 0);
}

void HoverTank::Update(float dt)
{
	if (controls.GetAliasState(1001, false, Controls::Active))
	{
		angles.x -= controls.GetAliasValue(1500, true) * 0.01f;
		angles.y -= controls.GetAliasValue(1501, true) * 0.01f;

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

	render.SetTransform(Render::View, view);
	render.SetTransform(Render::Projection, proj);

	if (dt > 0)
	{
		mScene->simulate(dt);

		while (!mScene->fetchResults())
		{
			// do something useful        
		}
	}

	const PxRenderBuffer& rb = mScene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		render.DebugLine(Vector(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
						 Vector(line.pos0.x, line.pos0.y, line.pos0.z), COLOR_GREEN, false);

		if (i > 50) break;
	}


	PxTransform pT = PxShapeExt::getGlobalPose(*shapes[0], *box);
	PxBoxGeometry bg;
	shapes[0]->getBoxGeometry(bg);
	PxMat33 m = PxMat33(pT.q);
	mat.Identity();
	mat.Vx() = Vector(m.column0.x, m.column0.y,m.column0.z);
	mat.Vy() = Vector(m.column1.x, m.column1.y, m.column1.z);
	mat.Vz() = Vector(m.column2.x, m.column2.y, m.column2.z);

	mat.Pos() = Vector(pT.p.x, pT.p.y, pT.p.z);

	//render.DebugBox(mat, COLOR_YELLOW, Vector(3.0, 1.0, 2.0f));

	AddHover(mat, -mat.Vy() * 0.5001f + mat.Vx() * 1.5f + mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f + mat.Vx() * 1.5f - mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f - mat.Vx() * 1.5f + mat.Vz());
	AddHover(mat, -mat.Vy() * 0.5001f - mat.Vx() * 1.5f - mat.Vz());

	view.BuildView(mat.Pos() + Vector(0, 4.5f, 0.0f) - Vector(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 15, mat.Pos() + Vector(0,4.5f,0.0f), Vector(0, 1, 0));

	dir = mat.Vx();
	dir.y = 0;
	dir.Normalize();

	if (controls.GetAliasState(DIK_W, false, Controls::Active))
	{
		move_speed += 200 * dt;

		if (move_speed > 300)
		{
			move_speed = 300;
		}
	}
	else
	if (controls.GetAliasState(DIK_S, false, Controls::Active))
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

	if (controls.GetAliasState(DIK_D, false, Controls::Active))
	{
		strafe_speed += 75 * dt;

		if (strafe_speed > 100)
		{
			strafe_speed = 100;
		}
	}
	else
	if (controls.GetAliasState(DIK_A, false, Controls::Active))
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

	mat.Pos().y -= 1.0f;
	hover_drawer->SetTransform(mat);

	mat = Matrix().RotateZ(angles.y) * Matrix().RotateY(-angles.x) * Matrix().Move(hover_model.locator + mat.Pos());// *mat;
	tower_drawer->SetTransform(mat);

	mat = Matrix().Move(tower_model.locator) * mat;
	gun_drawer->SetTransform(mat);

	mat = Matrix().Move(gun_model.locator) * mat;
	//render.DebugSphere(mat.Pos(), COLOR_MAGNETA, 1.0f);

	for (int i = 0; i < boxes.size(); i++)
	{
		PxTransform pT = boxes[i].box->getGlobalPose();
		PxBoxGeometry bg;
		shapes[0]->getBoxGeometry(bg);
		PxMat33 m = PxMat33(pT.q);

		Matrix mat;
		mat.Vx() = Vector(m.column0.x, m.column0.y, m.column0.z);
		mat.Vy() = Vector(m.column1.x, m.column1.y, m.column1.z);
		mat.Vz() = Vector(m.column2.x, m.column2.y, m.column2.z);

		mat.Pos() = Vector(pT.p.x, pT.p.y, pT.p.z);

		render.DebugBox(mat, COLOR_YELLOW, Vector(3.0, 1.0, 2.0f));

		boxes[i].pos = mat.Pos();
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

	if (controls.GetAliasState(DIK_F, false, Controls::Activated))
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
		Vector dir = boxes[i].pos - pos;
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

		//render.DebugLine(org, COLOR_RED, org - Vector(0, len, 0.0f) * (1 - k), COLOR_RED, false);

		PxRigidBodyExt::addForceAtPos(*box, hit.block.normal * k * 85,
									  hit.block.position, PxForceMode::eFORCE, true);
	}
	else
	{
		//render.DebugLine(org, COLOR_YELLOW, org - Vector(0, len, 0.0f), COLOR_YELLOW, false);
	}
}