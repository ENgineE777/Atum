
#include "Phys2DComp.h"
#include "SpriteInst.h"
#include "SpriteTileInst.h"
#include "SpriteGraphAsset.h"
#include "SpriteGraphInst.h"

COMPREG(Phys2DComp, "Phys2D")
COMPINCL(SpriteAsset)
COMPINCL(SpriteTileAsset)
COMPINCL(SpriteGraphAsset)
COMPREG_END(Phys2DComp)

META_DATA_DESC(Phys2DComp)
ENUM_PROP(Phys2DComp, body_type, 0, "Phys2D", "body_type")
	ENUM_ELEM("StaticBody", 0)
	ENUM_ELEM("DynamicBody", 1)
	ENUM_ELEM("KineticBody", 2)
ENUM_END
FLOAT_PROP(Phys2DComp, density, 1.25f, "Phys2D", "density")
FLOAT_PROP(Phys2DComp, friction, 0.25f, "Phys2D", "friction")
BOOL_PROP(Phys2DComp, allow_rotate, true, "Phys2D", "allow_rotate")
BOOL_PROP(Phys2DComp, use_object_size, true, "Phys2D", "use_object_size")
FLOAT_PROP(Phys2DComp, width, 64, "Phys2D", "width")
FLOAT_PROP(Phys2DComp, height, 64, "Phys2D", "height")
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
	if (StringUtils::IsEqual(object->class_name, "SpriteInst"))
	{
		Play(dynamic_cast<SpriteInst*>(object));
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteTileInst"))
	{
		Play(dynamic_cast<SpriteTileInst*>(object));
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteGraphInst"))
	{
		PlayGraphInst(dynamic_cast<SpriteGraphInst*>(object));
	}
}

void Phys2DCompInst::CreatBody(int index, bool visible, Vector2 pos, Vector2 size, Vector2 center, bool allow_rotate)
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

	bodyDef.position.Set(pos.x, pos.y);

	b2PolygonShape box;
	box.SetAsBox(size.x, size.y, b2Vec2(center.x, center.y), 0);

	b2FixtureDef fixtureDef;
	fixtureDef.density = ((Phys2DComp*)asset_comp)->density;
	fixtureDef.friction = ((Phys2DComp*)asset_comp)->friction;
	fixtureDef.shape = &box;

	bodies[index].index = index;
	bodies[index].object = object;
	bodies[index].body = object->PScene2D()->CreateBody(&bodyDef);
	bodies[index].body->CreateFixture(&fixtureDef);
	bodies[index].body->SetUserData(&bodies[index]);

	if (!allow_rotate)
	{
		bodies[index].body->SetFixedRotation(true);
	}

	if (!visible)
	{
		bodies[index].body->SetActive(false);
	}
}

template<typename T>
void Phys2DCompInst::Play(T* sprite_inst)
{
	if (sprite_inst->instances.size() == 0)
	{
		return;
	}

	body_type = ((Phys2DComp*)asset_comp)->body_type;

	float scale = 1.0f / 50.0f;

	bodies.resize(sprite_inst->instances.size());
	Phys2DComp* comp = (Phys2DComp*)asset_comp;

	Vector2 size = (comp->use_object_size ? sprite_inst->trans.size : Vector2(comp->width, comp->height));
	Vector2 center = { size.x * (0.5f - sprite_inst->trans.offset.x) * scale,
	                   size.y * (0.5f - sprite_inst->trans.offset.y) * scale };
	size *= 0.5f * scale;

	for (int index = 0; index < bodies.size(); index++)
	{
		CreatBody(index, sprite_inst->instances[index].IsVisible(), sprite_inst->instances[index].GetPos() * scale, size, center, comp->allow_rotate);
	}

	object->Tasks(false)->AddTask(-150, this, (Object::Delegate)&Phys2DCompInst::UpdateInstances);
}

void Phys2DCompInst::PlayGraphInst(SpriteGraphInst* graph_inst)
{
	body_type = ((Phys2DComp*)asset_comp)->body_type;

	float scale = 1.0f / 50.0f;

	bodies.resize(1);

	Phys2DComp* comp = (Phys2DComp*)asset_comp;

	Vector2 size = (comp->use_object_size ? graph_inst->trans.size : Vector2(comp->width, comp->height));
	Vector2 center = { size.x * (0.5f - graph_inst->trans.offset.x) * scale,
	                   size.y * (0.5f - graph_inst->trans.offset.y) * scale };
	size *= 0.5f * scale;

	CreatBody(0, 1, graph_inst->trans.pos * scale, size, center, comp->allow_rotate);

	object->Tasks(false)->AddTask(-50, this, (Object::Delegate)&Phys2DCompInst::UpdateInstances);
	bodies[0].body->SetFixedRotation(true);
}

void Phys2DCompInst::Stop()
{
	for (auto& body : bodies)
	{
		if (body.body)
		{
			object->PScene2D()->DestroyBody(body.body);
		}
	}

	bodies.clear();
}

void Phys2DCompInst::UpdateInstances(float dt)
{
	if (StringUtils::IsEqual(object->class_name, "SpriteInst"))
	{
		UpdateInstances(dynamic_cast<SpriteInst*>(object));
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteTileInst"))
	{
		UpdateInstances(dynamic_cast<SpriteTileInst*>(object));
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteGraphInst"))
	{
		UpdateGraphInst(dynamic_cast<SpriteGraphInst*>(object));
	}
}

template<typename T>
void Phys2DCompInst::UpdateInstances(T* sprite_inst)
{
	bool is_active = (sprite_inst->GetState() == SceneObject::State::Active);

	for (int index = 0; index < bodies.size(); index++)
	{
		if (bodies[index].body)
		{
			if (bodies[index].body->IsActive() != (sprite_inst->instances[index].IsVisible() && is_active))
			{
				bodies[index].body->SetActive(sprite_inst->instances[index].IsVisible() && is_active);

				if (sprite_inst->instances[index].IsVisible())
				{
					Vector2 pos = sprite_inst->instances[index].GetPos();
					bodies[index].body->SetTransform({ pos.x / 50.0f, pos.y / 50.0f }, 0.0f);
				}
			}

			if (is_active)
			{
				if (body_type == Phys2DComp::BodyType::KineticBody)
				{
					Vector2 pos = sprite_inst->instances[index].GetPos();
					bodies[index].body->SetTransform( { pos.x / 50.0f, pos.y / 50.0f }, 0.0f);
				}
				else
				if (body_type == Phys2DComp::BodyType::DynamicBody)
				{
					b2Transform trasn = bodies[index].body->GetTransform();
					sprite_inst->instances[index].SetPos({ trasn.p.x * 50.0f, trasn.p.y * 50.0f });
				}
			}
		}
	}
}

void Phys2DCompInst::UpdateGraphInst(SpriteGraphInst* graph_inst)
{
	if (bodies[0].body)
	{
		bool is_active = (graph_inst->GetState() == SceneObject::State::Active);

		if (bodies[0].body->IsActive() != (is_active))
		{
			bodies[0].body->SetActive(is_active);
		}

		if (bodies[0].body->IsActive())
		{
			b2Transform trasn = bodies[0].body->GetTransform();

			graph_inst->trans.pos.x = trasn.p.x * 50.0f;
			graph_inst->trans.pos.y = trasn.p.y * 50.0f;
		}
	}
}