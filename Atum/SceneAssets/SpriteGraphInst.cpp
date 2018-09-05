
#include "SpriteGraphInst.h"
#include "SpriteAsset.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpriteGraphInst, "SpriteGraph")

META_DATA_DESC(SpriteGraphInst)
BASE_SCENE_OBJ_NAME_PROP(SpriteGraphInst)
BASE_SCENE_OBJ_STATE_PROP(SpriteGraphInst)
FLOAT_PROP(SpriteGraphInst, trans.pos.x, 100.0f, "Geometry", "x")
FLOAT_PROP(SpriteGraphInst, trans.pos.y, 100.0f, "Geometry", "y")
FLOAT_PROP(SpriteGraphInst, trans.depth, 0.5f, "Geometry", "Depth")
BOOL_PROP(SpriteGraphInst, graph_instance.state.horz_flipped, false, "Node", "horz_flipped")
META_DATA_DESC_END()

void SpriteGraphInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteGraphInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void ActivateLink(string&in)", WRAP_MFN(SpriteGraphInst, ActivateLink), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void GotoNode(string&in)", WRAP_MFN(SpriteGraphInst, GotoNode), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "float GetLinearVelocityX()", WRAP_MFN(SpriteGraphInst, GetLinearVelocityX), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "float GetLinearVelocityY()", WRAP_MFN(SpriteGraphInst, GetLinearVelocityY), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void SetLinearVelocity(float x, float y)", WRAP_MFN(SpriteGraphInst, SetLinearVelocity), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void ApplyLinearImpulse(float x, float y)", WRAP_MFN(SpriteGraphInst, ApplyLinearImpulse), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "bool CheckColissionNormal(float x, float y)", WRAP_MFN(SpriteGraphInst, CheckColissionNormal), asCALL_GENERIC);
}

void SpriteGraphInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteGraphInst::Draw);
}

void SpriteGraphInst::Load(JSONReader& loader)
{
	SceneObjectInst::Load(loader);
	
	if (asset)
	{
		((SpriteGraphAsset*)asset)->PrepareInstance(&graph_instance);
		graph_instance.Reset();
		trans.size = graph_instance.cur_node->asset->trans.size;
	}
}

void SpriteGraphInst::Play()
{
	float scale = 1.0f / 50.0f;

	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(trans.pos.x * scale, trans.pos.y * scale);
	
		body = PScene2D()->CreateBody(&bodyDef);

		trans.size = { 64.0f, 64.0f};

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(trans.size.x * 0.5f * scale, trans.size.y * 0.5f * scale);

		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.25f;
		fixtureDef.friction = 0.25f;
		fixtureDef.shape = &dynamicBox;

		body->CreateFixture(&fixtureDef);

		body->SetFixedRotation(true);
	}
}

void SpriteGraphInst::Stop()
{
	if (body)
	{
		PScene2D()->DestroyBody(body);
		body = nullptr;
	}
}

void SpriteGraphInst::Draw(float dt)
{
	if (state == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	trans.size = asset->trans.size;
	trans.BuildMatrices();

	if (state == Active)
	{
		graph_instance.Update(dt);
	}

	Sprite::Draw(&trans, COLOR_WHITE, &graph_instance.cur_node->asset->sprite, &graph_instance.state, true, false);

	if (body)
	{
		b2Vec2 position = body->GetPosition();
		trans.pos.x = position.x * 50.0f;
		trans.pos.y = position.y * 50.0f - 30;

		trans.rotation = body->GetAngle();
	}
}

void SpriteGraphInst::ActivateLink(string& link)
{
	graph_instance.ActivateLink(link.c_str());
}

void SpriteGraphInst::GotoNode(string& node)
{
	graph_instance.GotoNode(node.c_str());
}

void SpriteGraphInst::ApplyLinearImpulse(float x, float y)
{
	if (body)
	{
		return body->ApplyLinearImpulseToCenter({x, y}, true);
	}
}

float SpriteGraphInst::GetLinearVelocityX()
{
	if (body)
	{
		return body->GetLinearVelocity().x;
	}

	return 0.0f;
}

float SpriteGraphInst::GetLinearVelocityY()
{
	if (body)
	{
		return body->GetLinearVelocity().y;
	}

	return 0.0f;
}

void SpriteGraphInst::SetLinearVelocity(float x, float y)
{
	if (body)
	{
		body->SetLinearVelocity({ x, y });
	}
}

bool SpriteGraphInst::CheckColissionNormal(float x, float y)
{
	if (body)
	{
		b2ContactEdge* contacts = body->GetContactList();

		if (contacts)
		{
			for (b2ContactEdge* contact = contacts; contact; contact = contact->next)
			{
				if (fabs(contact->contact->GetManifold()->localNormal.x - x) < 0.01f &&
					fabs(contact->contact->GetManifold()->localNormal.y - y) < 0.01f)
				{
					return true;
				}
			}
		}
	}

	return false;
}

#ifdef EDITOR
void SpriteGraphInst::SetEditMode(bool ed)
{
	Gizmo::inst->trans2D = ed ? &trans : nullptr;
	Gizmo::inst->enabled = ed;
}
#endif