
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
	ENUM_ELEM("DynamicCCDBody", 2)
	ENUM_ELEM("KineticBody", 3)
	ENUM_ELEM("Trigger", 4)
	ENUM_ELEM("Controller", 5)
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
		Play((SpriteInst*)object);
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteTileInst"))
	{
		Play((SpriteTileInst*)object);
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteGraphInst"))
	{
		PlayGraphInst((SpriteGraphInst*)object);
	}
}

void Phys2DCompInst::CreatBody(int index, bool visible, Vector2 pos, Vector2 size, Vector2 center, bool allow_rotate)
{
	Matrix body_trans;
	body_trans.Pos() = { pos.x, -pos.y, 0.0f };

	if (body_type == Phys2DComp::BodyType::StaticBody)
	{
		body_trans.Pos() += { center.x, -center.y, 0.0f };
	}

	bodies[index].index = index;
	bodies[index].object = object;

	if (body_type == Phys2DComp::BodyType::Controller)
	{
		PhysControllerDesc desc;
		desc.radius = size.x * 0.5f;
		desc.height = size.y - size.x;
		desc.pos = { pos.x, -pos.y, 0.0f };
		bodies[index].controller = object->PScene()->CreateController(desc);
		bodies[index].controller->SetUserData(&bodies[index]);

		if (!visible)
		{
			bodies[index].controller->SetActive(false);
		}
	}
	else
	{
		Matrix offset;
		offset.Pos() = { center.x, -center.y, 0.0f };

		bodies[index].body = object->PScene()->CreateBox({ size.x, size.y, 1.0f }, body_trans, offset, (PhysObject::BodyType)body_type);
		bodies[index].body->SetUserData(&bodies[index]);

		if (!allow_rotate && body_type == Phys2DComp::BodyType::DynamicBody)
		{
			bodies[index].body->SetFixedRotation(true);
		}

		if (!visible)
		{
			bodies[index].body->SetActive(false);
		}
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
	size *= scale;

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
	size *= scale;

	CreatBody(0, 1, graph_inst->trans.pos * scale, size, center, comp->allow_rotate);

	object->Tasks(false)->AddTask(-50, this, (Object::Delegate)&Phys2DCompInst::UpdateInstances);
}

void Phys2DCompInst::Stop()
{
	for (auto& body : bodies)
	{
		if (body.body)
		{
			RELEASE(body.body);
		}
	}

	bodies.clear();
}

void Phys2DCompInst::UpdateInstances(float dt)
{
	if (StringUtils::IsEqual(object->class_name, "SpriteInst"))
	{
		UpdateInstances((SpriteInst*)object);
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteTileInst"))
	{
		UpdateInstances((SpriteTileInst*)object);
	}
	else
	if (StringUtils::IsEqual(object->class_name, "SpriteGraphInst"))
	{
		UpdateGraphInst((SpriteGraphInst*)object);
	}
}

template<typename T>
void Phys2DCompInst::UpdateInstances(T* sprite_inst)
{
	bool is_active = (sprite_inst->GetState() == SceneObject::State::Active);

	Matrix mat;

	for (int index = 0; index < bodies.size(); index++)
	{
		if (bodies[index].body)
		{
			if (bodies[index].body->IsActive() != (sprite_inst->instances[index].IsVisible() && is_active))
			{
				bodies[index].body->SetActive(sprite_inst->instances[index].IsVisible() && is_active);

				if (body_type != Phys2DComp::BodyType::StaticBody && sprite_inst->instances[index].IsVisible())
				{
					mat.Pos() = { sprite_inst->instances[index].GetPos().x / 50.0f, -sprite_inst->instances[index].GetPos().y / 50.0f, 0.0f };
					bodies[index].body->SetTransform(mat);
				}
			}

			if (is_active)
			{
				if (body_type == Phys2DComp::BodyType::KineticBody)
				{
					mat.Pos() = { sprite_inst->instances[index].GetPos().x / 50.0f, -sprite_inst->instances[index].GetPos().y / 50.0f, 0.0f };
					bodies[index].body->SetTransform(mat);
				}
				else
				if (body_type == Phys2DComp::BodyType::DynamicBody)
				{
					bodies[index].body->GetTransform(mat);
					sprite_inst->instances[index].SetPos({ mat.Pos().x * 50.0f, -mat.Pos().y * 50.0f });
				}
			}
		}
	}
}

void Phys2DCompInst::UpdateGraphInst(SpriteGraphInst* graph_inst)
{
	if (bodies[0].controller)
	{
		bool is_active = (graph_inst->GetState() == SceneObject::State::Active);

		if (bodies[0].controller->IsActive() != (is_active))
		{
			bodies[0].controller->SetActive(is_active);
		}
	}
}