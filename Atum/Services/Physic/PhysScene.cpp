
#include "Physics.h"
#include "Services/Render/Render.h"

PhysObject* PhysScene::CreateBox(Vector size, Matrix trans, bool isStatic)
{
	PhysObject* obj = new PhysObject();

	obj->isStatic = isStatic;

	PxReal density = 1.0f;

	Quaternion q(trans);

	PxTransform transform(PxVec3(trans.Pos().x, trans.Pos().y, trans.Pos().z), PxQuat(q.x, q.y, q.z, q.w));

	PxVec3 dimensions(size.x, size.y, size.z);

	PxBoxGeometry geometry(dimensions);

	if (isStatic)
	{
		obj->actor = physics.physics->createRigidStatic(transform);
		obj->actor->createShape(geometry, *physics.defMaterial);
	}
	else
	{
		obj->actor = PxCreateDynamic(*physics.physics, transform, geometry, *physics.defMaterial, density);
		//obj->actor->setLinearVelocity(PxVec3(0, 0, 0));
	}

	scene->addActor(*obj->actor);

	return obj;
}

PhysController* PhysScene::CreateController(PhysControllerDesc& desc)
{
	PxCapsuleControllerDesc pxdesc;
	pxdesc.height = desc.height;
	pxdesc.radius = desc.radius;
	pxdesc.position = PxExtendedVec3(desc.pos.x, desc.pos.y, desc.pos.z);
	pxdesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
	pxdesc.density = 1.0f;
	pxdesc.slopeLimit = desc.slopeLimit;
	pxdesc.contactOffset = 0.1f;
	pxdesc.stepOffset = 0.5f;
	pxdesc.scaleCoeff = 0.8f;
	pxdesc.material = physics.defMaterial;

	PhysController* controller = new PhysController();
	controller->controller = manager->createController(pxdesc);

	return controller;
}

PhysHeightmap* PhysScene::CreateHeightmap(PhysHeightmap::Desc& desc, const char* name)
{
	PhysHeightmap* hm = new PhysHeightmap();

	Physics::StraemReader reader;
	if (reader.buffer.Load(name))
	{
		hm->heightField = physics.physics->createHeightField(reader);

		if (hm->heightField)
		{
			PxTransform pose = PxTransform(PxVec3(-desc.width * 0.5f * desc.scale.x, 0.0f, -desc.height * 0.5f * desc.scale.x), PxQuat(PxIdentity));

			hm->actor = physics.physics->createRigidStatic(pose);

			PxHeightFieldGeometry hfGeom(hm->heightField, PxMeshGeometryFlags(), desc.scale.y, desc.scale.x, desc.scale.x);
			hm->actor->createShape(hfGeom, *physics.defMaterial);

			scene->addActor(*hm->actor);
		}
	}

	return hm;
}

void PhysScene::SetVisualization(bool set)
{
	scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, set ? 1.0f : -1.0f);
	scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);
}

void PhysScene::DrawVisualization()
{
	const PxRenderBuffer& rb = scene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		render.DebugLine(Vector(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
			Vector(line.pos0.x, line.pos0.y, line.pos0.z), COLOR_GREEN, false);
	}
}

bool PhysScene::RayCast(RaycastDesc& desc)
{
	if (desc.length < 0.005f)
	{
		return false;
	}

	PxRaycastBuffer hit;

	if (scene->raycast(PxVec3(desc.origin.x, desc.origin.y, desc.origin.z), PxVec3(desc.dir.x, desc.dir.y, desc.dir.z), desc.length, hit))
	{
		desc.hitPos = Vector(hit.block.position.x, hit.block.position.y, hit.block.position.z);
		desc.hitNormal = Vector(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);

		return true;
	}

	return false;
}

void PhysScene::Simulate(float dt)
{
	scene->simulate(dt);
}

void PhysScene::FetchResults()
{
	while (!scene->fetchResults())
	{

	}
}

void PhysScene::Release()
{
	scene->release();
	delete this;
}