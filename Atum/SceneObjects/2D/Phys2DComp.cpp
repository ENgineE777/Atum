
#include "Phys2DComp.h"
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
INT_PROP(Phys2DComp, group, 1, "Phys2D", "group")
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

void Phys2DCompInst::ScriptProxy::ApplyLinearImpulse(float x, float y)
{
	if (!inst->IsVisible())
	{
		return;
	}

	if (body && body->body)
	{
		Vector2 pos = inst->GetPos();

		if (!body->body->IsActive())
		{
			body->body->SetActive(true);
			Matrix mat;

			mat.Pos() = { pos.x / 50.0f, -pos.y / 50.0f, 0.0f };
			body->body->SetTransform(mat);
		}
		
		body->body->AddForceAt({ pos.x / 50.0f, -pos.y / 50.0f, 0.0f }, { x, -y, 0.0f });
	}
}

void Phys2DCompInst::ScriptProxy::MoveTo(float x, float y)
{
	if (inst)
	{
		inst->SetPos(Vector2(x, y));
	}

	if (body && body->body)
	{
		Matrix mat;
		mat.Pos() = { x / 50.0f, -y / 50.0f, 0.0f };
		body->body->SetTransform(mat);
	}

	if (body && body->controller)
	{
		body->controller->SetPosition({ x / 50.0f, -y / 50.0f, 0.0f });
	}
}

void Phys2DCompInst::ScriptProxy::SetGroup(int group)
{
	if (body)
	{
		if (body->body)
		{
			body->body->SetGroup(group);
		}
		else
		if (body->controller)
		{
			body->controller->SetGroup(group);
		}
	}
}

bool Phys2DCompInst::ScriptProxy::CheckColission(bool under)
{
	if (body && body->controller)
	{
		return body->controller->IsColliding(under ? PhysController::CollideDown : PhysController::CollideUp);
	}

	return false;
}

void Phys2DCompInst::ScriptProxy::MoveController(float dx, float dy)
{
	if (inst)
	{
		inst->dir.x += dx;
		inst->dir.y += dy;
	}
}

void Phys2DCompInst::BindClassToScript()
{
	script_class_name = "Phys2D";
	core.scripts.RegisterObjectType(script_class_name, sizeof(Phys2DCompInst::ScriptProxy), "gr_script_scene_object_components");
	core.scripts.RegisterObjectMethod(script_class_name, "void ApplyLinearImpulse(float x, float y)", WRAP_MFN(Phys2DCompInst::ScriptProxy, ApplyLinearImpulse));
	core.scripts.RegisterObjectMethod(script_class_name, "void MoveTo(float x, float y)", WRAP_MFN(Phys2DCompInst::ScriptProxy, MoveTo));
	core.scripts.RegisterObjectMethod(script_class_name, "void SetGroup(int group)", WRAP_MFN(Phys2DCompInst::ScriptProxy, SetGroup));
	core.scripts.RegisterObjectMethod(script_class_name, "bool CheckColission(bool under)", WRAP_MFN(Phys2DCompInst::ScriptProxy, CheckColission));
	core.scripts.RegisterObjectMethod(script_class_name, "void Move(float dx, float dy)", WRAP_MFN(Phys2DCompInst::ScriptProxy, MoveController));
}

void Phys2DCompInst::InjectIntoScript(asIScriptObject* object, int index, const char* prefix)
{
	if (prop_index == -1)
	{
		char str[256];

		if (prefix[0])
		{
			StringUtils::Printf(str, 256, "%s_phys", prefix);

			for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual(str, object->GetPropertyName(prop)))
				{
					prop_index = prop;
					break;
				}
			}
		}

		if (prop_index == -1)
		{
			for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual("phys", object->GetPropertyName(prop)))
				{
					prop_index = prop;
					break;
				}
			}
		}
	}

	if (prop_index != -1)
	{
		SyncInstances();

		*(asPWORD*)(object->GetAddressOfProperty(prop_index)) = (asPWORD)&script_bodies[index];
	}
}

void Phys2DCompInst::SyncInstances()
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->instances.size() != bodies.size())
	{
		int dif = (int)(sprite_inst->instances.size() - bodies.size());

		if (dif < 0)
		{
			for (int index = (int)bodies.size() + dif; index < (int)bodies.size(); index++)
			{
				if (bodies[index].body)
				{
					RELEASE(bodies[index].body);
				}

				if (bodies[index].controller)
				{
					RELEASE(bodies[index].controller);
				}
			}
		}
		
		bodies.resize(sprite_inst->instances.size());
		script_bodies.resize(sprite_inst->instances.size());

		if (dif > 0)
		{
			Phys2DComp* comp = (Phys2DComp*)asset_comp;

			float scale = 1.0f / 50.0f;

			Vector2 size = (comp->use_object_size ? sprite_inst->trans.size : Vector2(comp->width, comp->height));
			Vector2 center = { size.x * (0.5f - sprite_inst->trans.offset.x) * scale,
			size.y * (0.5f - sprite_inst->trans.offset.y) * scale };
			size *= scale;

			for (int index = (int)bodies.size() - dif; index < (int)bodies.size(); index++)
			{
				CreateBody(index, sprite_inst->instances[index].IsVisible(), sprite_inst->instances[index].GetPos() * scale, size, center, comp->allow_rotate);
			}
		}

		for (int index = 0; index < (int)bodies.size(); index++)
		{
			script_bodies[index].inst = &sprite_inst->instances[index];
			script_bodies[index].body = &bodies[index];

			if (bodies[index].body)
			{
				bodies[index].body->SetUserData(&bodies[index]);
			}
			else
			if (bodies[index].controller)
			{
				bodies[index].controller->SetUserData(&bodies[index]);
			}
		}
	}
}

void Phys2DCompInst::Play()
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	body_type = ((Phys2DComp*)asset_comp)->body_type;
	group = ((Phys2DComp*)asset_comp)->group;

	SyncInstances();

	object->Tasks(false)->AddTask(-150, this, (Object::Delegate)&Phys2DCompInst::UpdateInstances);
}

void Phys2DCompInst::Stop()
{
	for (auto& body : bodies)
	{
		if (body.body)
		{
			RELEASE(body.body);
		}

		if (body.controller)
		{
			RELEASE(body.controller);
		}
	}

	bodies.clear();
	script_bodies.clear();
}

void Phys2DCompInst::CreateBody(int index, bool visible, Vector2 pos, Vector2 size, Vector2 center, bool allow_rotate)
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
		desc.group = group;
		bodies[index].controller = object->PScene()->CreateController(desc, group);
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

		bodies[index].body = object->PScene()->CreateBox({ size.x, size.y, 1.0f }, body_trans, offset, (PhysObject::BodyType)body_type, group);
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

void Phys2DCompInst::UpdateInstances(float dt)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

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
		else
		if (bodies[index].controller)
		{
			if (bodies[index].controller->IsActive() != (sprite_inst->instances[index].IsVisible() && is_active))
			{
				bodies[index].controller->SetActive(sprite_inst->instances[index].IsVisible() && is_active);

				if (sprite_inst->instances[index].IsVisible())
				{
					bodies[index].controller->SetPosition({ sprite_inst->instances[index].GetPos().x / 50.0f, -sprite_inst->instances[index].GetPos().y / 50.0f, 0.0f });
				}
			}

			if (is_active)
			{
				if (bodies[index].controller)
				{
					if (sprite_inst->instances[index].dir.Length() > 0.001f)
					{
						bodies[index].controller->Move({ sprite_inst->instances[index].dir.x / 50.0f, -sprite_inst->instances[index].dir.y / 50.0f, 0.0f });

						Vector pos;
						bodies[index].controller->GetPosition(pos);

						sprite_inst->instances[index].SetPos({ pos.x * 50.0f, -pos.y * 50.0f });

						sprite_inst->instances[index].dir = 0.0f;
					}
					else
					if (sprite_inst->instances[index].hack_marker)
					{
						bodies[index].controller->SetPosition({ sprite_inst->instances[index].GetPos().x / 50.0f, -sprite_inst->instances[index].GetPos().y / 50.0f, 0.0f });
						sprite_inst->instances[index].hack_marker = false;
					}
				}
			}
		}
	}
}