
#include "SpriteGraphInst.h"
#include "SpriteAsset.h"
#include "Services/Render/Render.h"
#include "Phys2DComp.h"
#include "SpriteGraphInst.h"

CLASSREG(SceneObject, SpriteGraphInst, "SpriteGraph")

META_DATA_DESC(SpriteGraphInst)
BASE_SCENE_OBJ_PROP(SpriteGraphInst)
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
	scripts.engine->RegisterObjectMethod(script_class_name, "bool CheckColission(bool under)", WRAP_MFN(SpriteGraphInst, CheckColission), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void Move(float dx, float dy)", WRAP_MFN(SpriteGraphInst, Move), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void MoveTo(float x, float y)", WRAP_MFN(SpriteGraphInst, MoveTo), asCALL_GENERIC);
}

void SpriteGraphInst::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpriteGraphInst::Draw);

	script_callbacks.push_back(ScriptCallback("OnContact", "int ", "%i%s%i"));
}

void SpriteGraphInst::Load(JSONReader& loader)
{
	SceneObjectInst::Load(loader);
}

void SpriteGraphInst::ApplyProperties()
{
	if (asset)
	{
		((SpriteGraphAsset*)asset)->PrepareInstance(&graph_instance);
		graph_instance.Reset();
		trans.size = graph_instance.cur_node->asset->trans.size;
	}
}

bool SpriteGraphInst::Play()
{
	trans.size = ((SpriteGraphAsset*)asset)->trans.size;
	trans.offset = graph_instance.cur_node->asset->trans.offset;

	return true;
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

	trans.size = ((SpriteGraphAsset*)asset)->trans.size;
	trans.offset = graph_instance.cur_node->asset->trans.offset;
	trans.BuildMatrices();

	if (state == Active)
	{
		graph_instance.Update(dt);
	}

	Sprite::Draw(&trans, COLOR_WHITE, &graph_instance.cur_node->asset->sprite, &graph_instance.state, true, false);

	if (owner->Playing())
	{
		PhysController* body = HackGetBody();

		if (body && dir.Length() > 0.001f)
		{
			body->Move({ dir.x / 50.0f, -dir.y / 50.0f, 0.0f });

			Vector pos;
			body->GetPosition(pos);

			trans.pos.x = pos.x * 50.0f;
			trans.pos.y = -pos.y * 50.0f;

			dir = 0.0f;
		}
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

PhysController* SpriteGraphInst::HackGetBody()
{
	for (auto comp : components)
	{
		Phys2DCompInst* phys_comp = (Phys2DCompInst*)comp;

		if (phys_comp)
		{
			return phys_comp->bodies[0].controller;
		}
	}

	return nullptr;
}

bool SpriteGraphInst::CheckColission(bool under)
{
	PhysController* body = HackGetBody();

	if (body)
	{
		return body->IsColliding(under ? PhysController::CollideDown : PhysController::CollideUp);
	}

	return false;
}

void SpriteGraphInst::Move(float dx, float dy)
{
	dir.x += dx;
	dir.y += dy;
}

void SpriteGraphInst::MoveTo(float x, float y)
{
	PhysController* body = HackGetBody();

	if (body)
	{
		trans.pos = { x, y };
		Vector pos = { x / 50.0f, -y / 50.0f, 0.0f };
		body->SetPosition(pos);
	}
}

#ifdef EDITOR
bool SpriteGraphInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	Vector2 pos = (trans.pos + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

	if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
		pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
	{
		return true;
	}

	return false;
}

void SpriteGraphInst::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	Gizmo::inst->SetTrans2D(ed ? &trans : nullptr, Gizmo::trans_2d_move);
}
#endif