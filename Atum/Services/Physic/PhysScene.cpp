
#include "Physics.h"
#include "Services/Core/Core.h"
#include "Services/Scene/SceneObject.h"

PhysObject* PhysScene::CreateBox(Vector size, Matrix trans, Matrix offset, PhysObject::BodyType type, uint32_t group)
{
	PhysObject* obj = new PhysObject();

	obj->body_type = type;

	PxReal density = 1.0f;

	Quaternion q(trans);

	PxTransform transform(PxVec3(trans.Pos().x, trans.Pos().y, trans.Pos().z), PxQuat(q.x, q.y, q.z, q.w));

	PxVec3 dimensions(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

	PxBoxGeometry geometry(dimensions);
	PxShape* shape;

	if (type == PhysObject::Static || type == PhysObject::Trigger)
	{
		obj->actor = core.physics.physics->createRigidStatic(transform);
		shape = obj->actor->createShape(geometry, *core.physics.defMaterial);

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

		actor->getShapes(&shape, 1);

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

	if (type != PhysObject::Trigger)
	{
		SetShapeGroup(shape, group);
	}

	scene->addActor(*obj->actor);

	return obj;
}

PhysController* PhysScene::CreateController(PhysControllerDesc& desc, uint32_t group)
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

	auto actor = controller->controller->getActor();
	PxShape* shape;
	actor->getShapes(&shape, 1);

	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	SetShapeGroup(shape, group);

	return controller;
}

PhysHeightmap* PhysScene::CreateHeightmap(PhysHeightmap::Desc& desc, const char* name, uint32_t group)
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
			SetShapeGroup(hm->actor->createShape(hfGeom, *core.physics.defMaterial), group);

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
	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = desc.group;


	if (scene->raycast(PxVec3(desc.origin.x, desc.origin.y, desc.origin.z), PxVec3(desc.dir.x, desc.dir.y, desc.dir.z), desc.length, hit, PxHitFlags(PxHitFlag::eDEFAULT), filterData))
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
		PhysScene::HandleSceneObjectContact(udataA->object, udataA->index, udataB->object, udataB->index, "OnContact");
		PhysScene::HandleSceneObjectContact(udataB->object, udataB->index, udataA->object, udataA->index, "OnContact");
	}
}

void PhysScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		BodyUserData* udataA = static_cast<BodyUserData*>(pairs[i].triggerActor->userData);
		BodyUserData* udataB = static_cast<BodyUserData*>(pairs[i].otherActor->userData);

		//eNOTIFY_TOUCH_FOUND or eNOTIFY_TOUCH_LOST

		if (udataA && udataB)
		{
			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				PhysScene::HandleSceneObjectContact(udataA->object, udataA->index, udataB->object, udataB->index, "OnContactStart");
			}

			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				PhysScene::HandleSceneObjectContact(udataA->object, udataA->index, udataB->object, udataB->index, "OnContactEnd");
			}
		}
	}
}

void PhysScene::Release()
{
	scene->release();
	delete this;
}

void PhysScene::HandleSceneObjectContact(SceneObject* object, int index, SceneObject* contact_object, int contact_index, const char* callback_name)
{
	SceneObject::ScriptCallback* callabck = object->FindScriptCallback(callback_name);

	if (callabck)
	{
		callabck->Call(object->Script(), index, contact_object->GetName(), contact_index);
	}
}