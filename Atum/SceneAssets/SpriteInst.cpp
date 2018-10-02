
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"
#include "Phys2DComp.h"

META_DATA_DESC(SpriteInst::Instance)
FLOAT_PROP(SpriteInst::Instance, pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteInst::Instance, pos.y, 0.0f, "Prop", "y")
INT_PROP(SpriteInst::Instance, index, -1, "Prop", "index")
INT_PROP(SpriteInst::Instance, visible, 1, "Prop", "visible")
COLOR_PROP(SpriteInst::Instance, color, COLOR_WHITE, "Prop", "color")
FLOAT_PROP(SpriteInst::Instance, alpha, 0.0f, "Prop", "alpha")
META_DATA_DESC_END()

CLASSREG(SceneObject, SpriteInst, "Sprite")

META_DATA_DESC(SpriteInst)
BASE_SCENE_OBJ_PROP(SpriteInst)
FLOAT_PROP(SpriteInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpriteInst, trans.depth, 0.5f, "Geometry", "Depth")
ARRAY_PROP(SpriteInst, instances, Instance, "Prop", "inst")
META_DATA_DESC_END()

void SpriteInst::Instance::SetObject(asIScriptObject* set_object, vector<int>* set_mapping)
{
	object = set_object;
	mapping = set_mapping;

	SetPos(pos);
	SetFlipped(frame_state.horz_flipped);
	SetVisible(visible);
}

void SpriteInst::Instance::SetPos(Vector2 set_pos)
{
	if (object)
	{
		int prop_x = mapping[0][0];
		int prop_y = mapping[0][1];

		if (prop_x != -1 && prop_y != -1)
		{
			*((float*)(object->GetAddressOfProperty(prop_x))) = set_pos.x;
			*((float*)(object->GetAddressOfProperty(prop_y))) = set_pos.y;
		}

		return;
	}

	pos = set_pos;
}

Vector2 SpriteInst::Instance::GetPos()
{
	if (object)
	{
		int prop_x = mapping[0][0];
		int prop_y = mapping[0][1];

		if (prop_x != -1 && prop_y != -1)
		{
			return Vector2(*((float*)object->GetAddressOfProperty(prop_x)), *((float*)object->GetAddressOfProperty(prop_y)));
		}
	}

	return pos;
}

void SpriteInst::Instance::SetFlipped(int set_horz_flipped)
{
	if (object)
	{
		int prop = mapping[0][2];

		if (prop != -1)
		{
			*((int*)object->GetAddressOfProperty(prop)) = set_horz_flipped;
		}

		return;
	}

	frame_state.horz_flipped = set_horz_flipped;
}

int SpriteInst::Instance::GetFlipped()
{
	if (object)
	{
		int prop = mapping[0][2];

		if (prop != -1)
		{
			return *((int*)object->GetAddressOfProperty(prop));
		}
	}

	return (frame_state.horz_flipped != 0);
}

void SpriteInst::Instance::SetVisible(int set_visible)
{
	if (object)
	{
		int prop = mapping[0][3];

		if (prop != -1)
		{
			*((int*)object->GetAddressOfProperty(prop)) = set_visible;
		}

		return;
	}

	visible = set_visible;
}

bool SpriteInst::Instance::IsVisible()
{
	if (object)
	{
		int prop = mapping[0][3];

		if (prop != -1)
		{
			return (*((int*)object->GetAddressOfProperty(prop)) != 0);
		}
	}

	return (visible != 0);
}

void SpriteInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void AddInstance(float x, float y)", WRAP_MFN(SpriteInst, AddInstance), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void ClearInstances()", WRAP_MFN(SpriteInst, ClearInstances), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void ApplyLinearImpulse(int index, float x, float y)", WRAP_MFN(SpriteInst, ApplyLinearImpulse), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void Move(int index, float x, float y)", WRAP_MFN(SpriteInst, Move), asCALL_GENERIC);
}

bool SpriteInst::InjectIntoScript(const char* type, void* property)
{
	if (StringUtils::IsEqual(type, "array"))
	{
		if (instances.size() > 0)
		{
			array = (CScriptArray*)property;
			array->Resize((uint32_t)instances.size());
			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			mapping.resize(4);

			for (int value_index = 0; value_index < 4; value_index++)
			{
				mapping[value_index] = -1;

				const char* names[] = { "x", "y", "horz_flipped", "visible" };

				for (int prop = 0; prop < (int)objects[0]->GetPropertyCount(); prop++)
				{
					if (StringUtils::IsEqual(names[value_index], objects[0]->GetPropertyName(prop)))
					{
						mapping[value_index] = prop;
						break;
					}
				}
			}

			int index = 0;
			for (auto& inst : instances)
			{
				inst.SetObject(objects[index], &mapping);

				index++;
			}

			for (auto comp : components)
			{
				comp->InjectIntoScript(type, property);
			}
		}

		return true;
	}
	
	return SceneObject::InjectIntoScript(type, property);
}

void SpriteInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteInst::Draw);

	script_callbacks.push_back(ScriptCallback("OnContact", "int", "%i%s%i"));
}

bool SpriteInst::Play()
{
	trans.size = ((SpriteAsset*)asset)->trans.size;
	trans.offset = ((SpriteAsset*)asset)->trans.offset;

	return true;
}

void SpriteInst::Draw(float dt)
{
	if (state == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	SpriteAsset* sprite_asset = (SpriteAsset*)asset;

#ifdef EDITOR
	if (edited)
	{
		if (sel_inst != -1)
		{
			instances[sel_inst].SetPos(trans.pos);
		}

		if (controls.DebugKeyPressed("KEY_I") && sel_inst !=-1)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(sel_inst);
			}

			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
		}

		bool add_center = controls.DebugKeyPressed("KEY_O");
		bool add_after = controls.DebugKeyPressed("KEY_P");

		if (add_center || add_after)
		{
			Instance inst;

			if (sel_inst != -1 && add_after)
			{
				inst.SetPos(instances[sel_inst].GetPos() + 20.0f);
			}
			else
			{
				float scale = 1024.0f / render.GetDevice()->GetHeight();
				inst.SetPos({ Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale });
			}

			instances.push_back(inst);

			sel_inst = (int)instances.size() - 1;

			for (auto comp : components)
			{
				comp->InstAdded();
			}

			SetGizmo();
		}
	}
#endif

	trans.offset = sprite_asset->trans.offset;
	trans.size = sprite_asset->trans.size;

	Vector2 pos = trans.pos;
	trans.pos *= axis_scale;

	Vector2 cam_pos = 0.0f;
	if (Sprite::use_ed_cam)
	{
		cam_pos = Sprite::ed_cam_pos;
		Sprite::ed_cam_pos *= axis_scale;
	}
	else
	{
		cam_pos = Sprite::cam_pos;
		Sprite::cam_pos *= axis_scale;
	}

	for (int i = 0; i<instances.size(); i++)
	{
		Instance& inst = instances[i];

		if (!inst.IsVisible())
		{
			continue;
		}

		if (state == Active)
		{
			Sprite::UpdateFrame(&sprite_asset->sprite, &inst.frame_state, dt);
		}

		if (inst.frame_state.finished)
		{
			instances.erase(instances.begin() + i);
			i--;
			continue;
		}

		if (inst.color.a < 0.01f)
		{
			continue;
		}

		Vector2 pos = inst.GetPos();
		trans.mat_global.Pos() = { pos.x, pos.y, trans.depth };
		inst.frame_state.horz_flipped = inst.GetFlipped();
		inst.color.a = inst.alpha;

		Sprite::Draw(&trans, inst.color, &sprite_asset->sprite, &inst.frame_state, true, false);
	}

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}

b2Body* SpriteInst::HackGetBody(int index)
{
	for (auto comp : components)
	{
		Phys2DCompInst* phys_comp = dynamic_cast<Phys2DCompInst*>(comp);

		if (phys_comp)
		{
			phys_comp->UpdateInstances(0.0f);
			return phys_comp->bodies[index].body;
		}
	}

	return nullptr;
}

void SpriteInst::AddInstance(float x, float y)
{
	Instance inst;
	inst.SetPos({ x, y });
	inst.frame_state.looped = false;
	instances.push_back(inst);
}

void SpriteInst::ClearInstances()
{
	instances.clear();
}

void SpriteInst::ApplyLinearImpulse(int index, float x, float y)
{
	b2Body* body = HackGetBody(index);

	if (body)
	{
		body->SetLinearVelocity({ x, y });
	}
}

void SpriteInst::Move(int index, float x, float y)
{
	b2Body* body = HackGetBody(index);

	if (body)
	{
		b2Vec2 pos(0.0f, 0.0f);
		body->SetLinearVelocity(pos);

		instances[index].SetPos({ x, y });
		pos.x = x / 50.0f;
		pos.y = y / 50.0f;
		body->SetTransform(pos, 0.0f);
	}
}

#ifdef EDITOR
bool SpriteInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Instance& inst = instances[i];

		Vector2 pos = (inst.GetPos() + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
			pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
		{
			sel_inst = i;

			if (edited)
			{
				SetGizmo();
			}

			return true;
		}
	}

	return false;
}

void SpriteInst::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		SetGizmo();
	}
}

void SpriteInst::SetGizmo()
{
	if (sel_inst != -1)
	{
		trans.pos = instances[sel_inst].GetPos();
	}

	Gizmo::inst->SetTrans2D(sel_inst != -1 ? &trans : nullptr, Gizmo::trans_2d_move);
}

int SpriteInst::GetInstCount()
{
	return (int)instances.size();
}
#endif