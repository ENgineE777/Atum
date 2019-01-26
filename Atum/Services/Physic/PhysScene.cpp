
#include "Physics.h"
#include "Services/Core/Core.h"
#include "Services/Scene/SceneObject.h"

PhysObject* PhysScene::CreateBox(Vector size, Matrix trans, Matrix offset, PhysObject::BodyType type)
{
	PhysObject* obj = new PhysObject();

	obj->body_type = type;

	PxReal density = 1.0f;

	Quaternion q(trans);

	PxTransform transform(PxVec3(trans.Pos().x, trans.Pos().y, trans.Pos().z), PxQuat(q.x, q.y, q.z, q.w));

	PxVec3 dimensions(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

	PxBoxGeometry geometry(dimensions);

	if (type == PhysObject::Static || type == PhysObject::Trigger)
	{
		obj->actor = core.physics.physics->createRigidStatic(transform);
		PxShape* shape = obj->actor->createShape(geometry, *core.physics.defMaterial);

		if (type == PhysObject::Trigger)
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
	}
	else
	{
		Quaternion q_offset(trans);
		PxTransform trans_offset(PxVec3(offset.Pos().x, offset.Pos().y, offset.Pos().z), PxQuat(q_offset.x, q_offset.y, q_offset.z, q_offset.w));

		PxRigidDynamic* actor = PxCreateDynamic(*core.physics.physics, transform, geometry, *core.physics.defMaterial, density, trans_offset);
		actor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);

		if (type == PhysObject::Kinetic)
		{
			actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		}
		else
		if (type == PhysObject::DynamicCCD)
		{
			actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		}

		obj->actor = actor;
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
	pxdesc.material = core.physics.defMaterial;

	PhysController* controller = new PhysController();
	pxdesc.reportCallback = controller;
	pxdesc.behaviorCallback = controller;

	controller->controller = manager->createController(pxdesc);
	controller->height = desc.height + desc.radius * 2.0f + controller->controller->getContactOffset();

	return controller;
}

PhysHeightmap* PhysScene::CreateHeightmap(PhysHeightmap::Desc& desc, const char* name)
{
	PhysHeightmap* hm = new PhysHeightmap();

	Physics::StraemReader reader;
	if (reader.buffer.Load(name))
	{
		hm->heightField = core.physics.physics->createHeightField(reader);

		if (hm->heightField)
		{
			PxTransform pose = PxTransform(PxVec3(-desc.width * 0.5f * desc.scale.x, 0.0f, -desc.height * 0.5f * desc.scale.x), PxQuat(PxIdentity));

			hm->actor = core.physics.physics->createRigidStatic(pose);

			PxHeightFieldGeometry hfGeom(hm->heightField, PxMeshGeometryFlags(), desc.scale.y, desc.scale.x, desc.scale.x);
			hm->actor->createShape(hfGeom, *core.physics.defMaterial);

			scene->addActor(*hm->actor);
		}
	}

	return hm;
}

void PhysScene::SetVisualization(bool set)
{
	scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, set ? 1.0f : -1.0f);
	scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, set ? 1.0f : -1.0f);
}

void PhysScene::DrawVisualization()
{
	const PxRenderBuffer& rb = scene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		core.render.DebugLine(Vector(line.pos1.x, line.pos1.y, line.pos1.z), COLOR_GREEN,
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
		desc.userdata = (BodyUserData*)(hit.block.actor->userData);
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

void PhysScene::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	BodyUserData* udataA = static_cast<BodyUserData*>(pairHeader.actors[0]->userData);
	BodyUserData* udataB = static_cast<BodyUserData*>(pairHeader.actors[1]->userData);

	if (udataA && udataB)
	{
		if (udataA->object->OnContact(udataA->index, udataB->object, udataB->index))
		{
			//contact->SetEnabled(false);
		}

		if (udataB->object->OnContact(udataB->index, udataA->object, udataA->index))
		{
			//contact->SetEnabled(false);
		}
	}
}

void PhysScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	BodyUserData* udataA = static_cast<BodyUserData*>(pairs->triggerActor->userData);
	BodyUserData* udataB = static_cast<BodyUserData*>(pairs->otherActor->userData);

	if (udataA && udataB)
	{
		if (udataA->object->OnContact(udataA->index, udataB->object, udataB->index))
		{
			//contact->SetEnabled(false);
		}
	}
}

void PhysScene::Release()
{
	scene->release();
	delete this;
}