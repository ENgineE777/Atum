
#include "Phys2DComp.h"
#include "SpriteInst.h"

COMPREG(Phys2DComp, "Phys2D")
COMPINCL(SpriteAsset)
COMPREG_END(Phys2DComp)

META_DATA_DESC(Phys2DComp)
ENUM_PROP(Phys2DComp, body_type, 0, "Phys2D", "body_type")
	ENUM_ELEM("StaticBody", 0)
	ENUM_ELEM("DynamicBody", 1)
	ENUM_ELEM("KineticBody", 2)
ENUM_END
META_DATA_DESC_END()

COMPREG(Phys2DCompInst, "Phys2DInst")
COMP_INSTANCES()
COMPREG_END(Phys2DCompInst)

META_DATA_DESC(Phys2DCompInst)
META_DATA_DESC_END()

Phys2DComp::Phys2DComp()
{
	inst_class_name = "Phys2DCompInst";
}

void Phys2DCompInst::Play()
{
	sprite_inst = (SpriteInst*)object;

	if (sprite_inst->instances.size() == 0)
	{
		return;
	}

	object->Tasks(false)->AddTask(-50, this, (Object::Delegate)&Phys2DCompInst::UpdateInstances);

	body_type = ((Phys2DComp*)asset_comp)->body_type;

	float scale = 1.0f / 50.0f;

	bodies.resize(sprite_inst->instances.size());

	for (int index = 0; index < bodies.size(); index++)
	{
		b2BodyDef bodyDef;

		switch (body_type)
		{
			case Phys2DComp::BodyType::StaticBody:
				bodyDef.type = b2_staticBody;
				break;
			case Phys2DComp::BodyType::KineticBody:
				bodyDef.type = b2_kinematicBody;
				break;
			case Phys2DComp::BodyType::DynamicBody:
				bodyDef.type = b2_dynamicBody;
				break;
		}

		bodyDef.position.Set(sprite_inst->instances[index].pos.x * scale, sprite_inst->instances[index].pos.y * scale);

		b2PolygonShape box;
		box.SetAsBox(sprite_inst->trans.size.x * 0.5f * scale, sprite_inst->trans.size.y * 0.5f * scale);

		bodies[index] = object->PScene2D()->CreateBody(&bodyDef);
		bodies[index]->CreateFixture(&box, 0.0f);
	}
}

void Phys2DCompInst::Stop()
{
	for (auto body : bodies)
	{
		if (body)
		{
			object->PScene2D()->DestroyBody(body);
		}
	}

	bodies.clear();
}

void Phys2DCompInst::UpdateInstances(float dt)
{
	if (body_type == Phys2DComp::BodyType::KineticBody)
	{
		for (int index = 0; index < bodies.size(); index++)
		{
			if (bodies[index])
			{
				bodies[index]->SetTransform({ sprite_inst->instances[index].pos.x / 50.0f, sprite_inst->instances[index].pos.y / 50.0f }, 0.0f);
			}
		}
	}
	else
	if (body_type == Phys2DComp::BodyType::DynamicBody)
	{
		for (int index = 0; index < bodies.size(); index++)
		{
			if (bodies[index])
			{
				b2Transform trasn = bodies[index]->GetTransform();

				sprite_inst->instances[index].pos.x = trasn.p.x * 50.0f;
				sprite_inst->instances[index].pos.y = trasn.p.y * 50.0f;
			}
		}
	}
}