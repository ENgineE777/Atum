
#include "Phys3DComp.h"
#include "MeshAsset.h"

COMPREG(Phys3DComp, "Phys3D")
COMPINCL(MeshAsset)
COMPREG_END(Phys3DComp)

META_DATA_DESC(Phys3DComp)
ENUM_PROP(Phys3DComp, body_type, 0, "Phys3D", "body_type", "Type of a body")
	ENUM_ELEM("StaticBody", 0)
	ENUM_ELEM("DynamicBody", 1)
	ENUM_ELEM("DynamicCCDBody", 2)
	ENUM_ELEM("KineticBody", 3)
	ENUM_ELEM("Trigger", 4)
	ENUM_ELEM("Controller", 5)
ENUM_END
FLOAT_PROP(Phys3DComp, density, 1.25f, "Phys3D", "density", "Density of a body")
FLOAT_PROP(Phys3DComp, friction, 0.25f, "Phys3D", "friction", "Friction of a material of a body")
BOOL_PROP(Phys3DComp, allow_rotate, true, "Phys3D", "allow_rotate", "Allowing physical objectto be rotateable")
BOOL_PROP(Phys3DComp, object_for_submeshes, false, "Phys3D", "object_for_submeshes", "Seprate object for each sub mesh")
INT_PROP(Phys3DComp, group, 1, "Phys3D", "group", "Group of a body")
META_DATA_DESC_END()

COMPREG(Phys3DCompInst, "Phys2DInst")
COMP_INSTANCES()
COMPREG_END(Phys3DCompInst)

META_DATA_DESC(Phys3DCompInst)
META_DATA_DESC_END()

Phys3DComp::Phys3DComp()
{
	inst_class_name = "Phys3DCompInst";
}

void Phys3DCompInst::ScriptProxy::ApplyLinearImpulse(Vector3& impulse)
{
	if (!inst->IsVisible())
	{
		return;
	}

	Matrix trans = inst->mesh->transform;

	for (auto& body : bodies)
	{
		if (!body.body->IsActive())
		{
			body.body->SetActive(true);
			body.body->SetTransform(trans);
		}
		
		body.body->AddForceAt(trans.Pos(), impulse);
	}
}

void Phys3DCompInst::ScriptProxy::AddExplosionForce(Vector3& pos, float force, float radius)
{
	if (!inst->IsVisible())
	{
		return;
	}

	Matrix trans = inst->mesh->transform;

	for (int index = 0; index < bodies.size(); index++)
	{
		auto& body = bodies[index];

		if (!body.body->IsActive())
		{
			body.body->SetActive(true);
			body.body->SetTransform(trans);
		}

		Vector3 center = ((inst->mesh->GetBBMax(index) + inst->mesh->GetBBMin(index)) * 0.5f) * trans;

		Vector3 dir = center - pos;
		float dist = dir.Normalize();

		if (dist > radius)
		{
			continue;
		}

		Vector3 impulse = dir * (1.0f - 0.9f * dist / radius) * force;
		body.body->AddForceAt(center, impulse);
	}
}

void Phys3DCompInst::ScriptProxy::SetTransform(Matrix& trans)
{
	if (inst)
	{
		inst->mesh->transform = trans;
	}

	for (auto& body : bodies)
	{
		if (body.body)
		{
			body.body->SetTransform(trans);
		}

		if (body.controller)
		{
			body.controller->SetPosition(trans.Pos());
		}
	}
}

void Phys3DCompInst::ScriptProxy::SetGroup(int group)
{
	for (auto& body : bodies)
	{
		if (body.body)
		{
			body.body->SetGroup(group);
		}
		else
		if (body.controller)
		{
			body.controller->SetGroup(group);
		}
	}
}

bool Phys3DCompInst::ScriptProxy::CheckColission(bool under)
{
	for (auto& body : bodies)
	{
		if (body.controller)
		{
			return body.controller->IsColliding(under ? PhysController::CollideDown : PhysController::CollideUp);
		}
	}

	return false;
}

void Phys3DCompInst::ScriptProxy::MoveController(Vector3& delta, uint32_t group, uint32_t ignore_group)
{
	if (inst)
	{
		inst->dir += delta;
		inst->collide_group = group;
		inst->ignore_group = ignore_group;
	}
}

void Phys3DCompInst::ScriptProxy::ReleaseBodies()
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
}

void Phys3DCompInst::Init()
{
	object->GetScene()->AddToGroup(object, "Phys3DComp");
}

void Phys3DCompInst::BindClassToScript()
{
	const char* brief = "Representation of 2D physical object component\n"
		"\n"
		"This component is adding 2D physical object to each instance of SpriteInst, SpriteGraphInst or SpriteTileInst.\n"
		"2d physical object can be static or dynamic, can have own dimentions.Belonging to particular group also adjustable.\n"
		"\n"
		"This class ::Phys2DCompInst is a representation on C++ side.\n";

	script_class_name = "Phys3D";
	core.scripts.RegisterObjectType(script_class_name, sizeof(Phys3DCompInst::ScriptProxy), "gr_script_scene_object_components", brief);
	core.scripts.RegisterObjectMethod(script_class_name, "void ApplyLinearImpulse(Vector3&in impulsey)", WRAP_MFN(Phys3DCompInst::ScriptProxy, ApplyLinearImpulse), "Apply Linear Impulse at certain point");
	core.scripts.RegisterObjectMethod(script_class_name, "void AddExplosionForce(Vector3&in pos, float force, float radius)", WRAP_MFN(Phys3DCompInst::ScriptProxy, AddExplosionForce), "Apply Linear Impulse at certain point");
	core.scripts.RegisterObjectMethod(script_class_name, "void SetTransform(Matrix&in trans)", WRAP_MFN(Phys3DCompInst::ScriptProxy, SetTransform), "Moving object to certain point");
	core.scripts.RegisterObjectMethod(script_class_name, "void SetGroup(int group)", WRAP_MFN(Phys3DCompInst::ScriptProxy, SetGroup), "Set belonging to a particular physical group");
	core.scripts.RegisterObjectMethod(script_class_name, "bool CheckColission(bool under)", WRAP_MFN(Phys3DCompInst::ScriptProxy, CheckColission), "Check if a physical object has a collision contact");
	core.scripts.RegisterObjectMethod(script_class_name, "void Move(Vector3&in delta, uint group, uint ignore_group)", WRAP_MFN(Phys3DCompInst::ScriptProxy, MoveController), "Move object on particular delta");
}

void Phys3DCompInst::InjectIntoScript(asIScriptObject* object, int index, const char* prefix)
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

void Phys3DCompInst::SyncInstances()
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	if (mesh_inst->instances.size() != script_bodies.size())
	{
		int dif = (int)(mesh_inst->instances.size() - script_bodies.size());

		if (dif < 0)
		{
			for (int index = (int)script_bodies.size() + dif; index < (int)script_bodies.size(); index++)
			{
				script_bodies[index].ReleaseBodies();
			}
		}

		script_bodies.resize(mesh_inst->instances.size());

		if (dif > 0)
		{
			Phys3DComp* comp = (Phys3DComp*)asset_comp;

			/*if (sprite_inst->parent_trans)
			{
				parent = sprite_inst->parent_trans->pos;
			}*/

			for (int index = (int)script_bodies.size() - dif; index < (int)script_bodies.size(); index++)
			{
				CreateBodies(index, mesh_inst->instances[index], comp->allow_rotate);
			}
		}

		for (int index = 0; index < (int)script_bodies.size(); index++)
		{
			script_bodies[index].inst = &mesh_inst->instances[index];
		}
	}
}

void Phys3DCompInst::Play()
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	body_type = ((Phys3DComp*)asset_comp)->body_type;
	group = ((Phys3DComp*)asset_comp)->group;
	object_for_submeshes = ((Phys3DComp*)asset_comp)->object_for_submeshes;

	SyncInstances();

	object->Tasks(false)->AddTask(-50, this, (Object::Delegate)&Phys3DCompInst::UpdateInstances);
}

void Phys3DCompInst::Release()
{
	if (object)
	{
		object->GetScene()->DelFromGroup(object, "Phys3DComp");
	}

	for (auto& script_body : script_bodies)
	{
		script_body.ReleaseBodies();
	}

	script_bodies.clear();

	SceneObjectInstComp::Release();
}

void Phys3DCompInst::CreateBodies(int index, MeshInstance::Instance& instance, bool allow_rotate)
{
	ScriptProxy& script_proxy = script_bodies[index];

	if (!object_for_submeshes)
	{
		script_proxy.bodies.resize(1);
		auto& body = script_proxy.bodies[0];

		Matrix body_trans = instance.mesh->transform;

		Vector3 size = instance.mesh->GetBBMax() - instance.mesh->GetBBMin();
		Vector3 center = (instance.mesh->GetBBMax() + instance.mesh->GetBBMin()) * 0.5f;

		Matrix offset;
		offset.Pos() = center;

		if (body_type == Phys3DComp::BodyType::StaticBody || body_type == Phys3DComp::BodyType::TrigerBody)
		{
			body_trans = offset * body_trans;
		}

		body.index = index;
		body.object = object;

		if (body_type == Phys3DComp::BodyType::Controller)
		{
			PhysControllerDesc desc;
			desc.radius = size.x * 0.5f;
			desc.height = size.y - size.x;
			desc.pos = body_trans.Pos();

			body.controller = object->PScene()->CreateController(desc, group);
			body.controller->SetUserData(&body);

			if (!instance.mesh->visible)
			{
				body.controller->SetActive(false);
			}
		}
		else
		{
			body.body = object->PScene()->CreateBox(size, body_trans, offset, (PhysObject::BodyType)body_type, group);
			body.body->SetUserData(&body);

			if (!allow_rotate && body_type == Phys3DComp::BodyType::DynamicBody)
			{
				body.body->SetFixedRotation(true);
			}

			if (!instance.mesh->visible)
			{
				body.body->SetActive(false);
			}
		}
	}
	else
	{
		instance.mesh->transforms.resize(instance.mesh->GetSubMeshesCount());

		script_proxy.bodies.resize(instance.mesh->GetSubMeshesCount());

		for (int body_index = 0; body_index < script_proxy.bodies.size(); body_index++)
		{
			auto& body = script_proxy.bodies[body_index];

			Matrix body_trans = instance.mesh->transform;

			instance.mesh->transforms[body_index] = body_trans;

			Vector3 size = instance.mesh->GetBBMax(body_index) - instance.mesh->GetBBMin(body_index);
			Vector3 center = (instance.mesh->GetBBMax(body_index) + instance.mesh->GetBBMin(body_index)) * 0.5f;
			Matrix offset;
			offset.Pos() = center;

			if (body_type == Phys3DComp::BodyType::StaticBody || body_type == Phys3DComp::BodyType::TrigerBody)
			{
				body_trans = offset * body_trans;
			}

			body.index = index;
			body.object = object;

			body.body = object->PScene()->CreateBox(size, body_trans, offset, (PhysObject::BodyType)body_type, group);
			body.body->SetUserData(&body);

			if (!allow_rotate && body_type == Phys3DComp::BodyType::DynamicBody)
			{
				body.body->SetFixedRotation(true);
			}

			if (!instance.mesh->visible)
			{
				body.body->SetActive(false);
			}
		}
	}
}

void Phys3DCompInst::UpdateInstances(float dt)
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	Vector2 parent = 0.0f;
	if (mesh_inst->parent_trans)
	{
		parent = mesh_inst->parent_trans->pos;
	}

	bool is_active = (mesh_inst->GetState() == SceneObject::State::Active);

	for (int index = 0; index < script_bodies.size(); index++)
	{
		auto& inst = mesh_inst->instances[index];

		if (!object_for_submeshes && body_type == Phys3DComp::BodyType::Controller)
		{
			auto& body = script_bodies[index].bodies[0];

			if (body.controller->IsActive() != (inst.IsVisible() && is_active))
			{
				body.controller->SetActive(inst.IsVisible() && is_active);

				if (inst.IsVisible())
				{
					body.controller->SetPosition(inst.mesh->transform.Pos());
				}
			}

			if (is_active)
			{
				if (body.controller)
				{
					if (inst.dir.Length() > 0.001f)
					{
						body.controller->Move(inst.dir, inst.collide_group, inst.ignore_group);

						Vector3 pos;
						body.controller->GetPosition(pos);

						inst.mesh->transform.Pos() = pos;

						inst.dir = 0.0f;
					}
					else
					{
						body.controller->SetPosition(inst.mesh->transform.Pos());
					}
				}
			}
		}
		else
		{
			for (auto body : script_bodies[index].bodies)
			{
				if (body.body)
				{
					if (body.body->IsActive() != (inst.IsVisible() && is_active))
					{
						body.body->SetActive(inst.IsVisible() && is_active);

						if (inst.IsVisible())
						{
							script_bodies[index].SetTransform(inst.mesh->transform);
						}
					}
				}
			}

			if (body_type == Phys3DComp::BodyType::DynamicBody && is_active && inst.IsVisible())
			{
				if (object_for_submeshes)
				{
					for (int i = 0; i < script_bodies[index].bodies.size(); i++)
					{
						script_bodies[index].bodies[i].body->GetTransform(inst.mesh->transforms[i]);
					}
				}
				else
				{
					script_bodies[index].bodies[0].body->GetTransform(inst.mesh->transform);
				}
			}
		}
	}
}