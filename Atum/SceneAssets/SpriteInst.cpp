
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

META_DATA_DESC(SpriteInst::Instance)
FLOAT_PROP(SpriteInst::Instance, pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteInst::Instance, pos.y, 0.0f, "Prop", "y")
META_DATA_DESC_END()

CLASSREG(SceneObject, SpriteInst, "Sprite")

META_DATA_DESC(SpriteInst)
BASE_SCENE_OBJ_NAME_PROP(SpriteInst)
BASE_SCENE_OBJ_STATE_PROP(SpriteInst)
FLOAT_PROP(SpriteInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpriteInst, trans.depth, 0.5f, "Geometry", "Depth")
ARRAY_PROP(SpriteInst, instances, Instance, "Prop", "inst")
META_DATA_DESC_END()

void SpriteInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void AddInstance(float x, float y)", WRAP_MFN(SpriteInst, AddInstance), asCALL_GENERIC);
}

void SpriteInst::InjectIntoScript(const char* type, void* property)
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
				for (int value_index = 0; value_index < 4; value_index++)
				{
					int prop = mapping[value_index];

					if (prop == -1)
					{
						continue;
					}

					switch (value_index)
					{
						case 0:
							*((float*)objects[index]->GetAddressOfProperty(prop)) = inst.pos.x;
							break;
						case 1:
							*((float*)objects[index]->GetAddressOfProperty(prop)) = inst.pos.y;
							break;
						case 2:
							*((int*)objects[index]->GetAddressOfProperty(prop)) = inst.frame_state.horz_flipped ? 1 : 0;
							break;
						case 3:
							*((int*)objects[index]->GetAddressOfProperty(prop)) = inst.visible;
							break;
					}
				}

				index++;
			}

			for (auto comp : components)
			{
				comp->InjectIntoScript(type, property);
			}
		}
	}
	else
	{
		SceneObject::InjectIntoScript(type, property);
	}
}

void SpriteInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteInst::Draw);
}

void SpriteInst::Play()
{
	trans.size = ((SpriteAsset*)asset)->trans.size;
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
			instances[sel_inst].pos = trans.pos;
		}

		if (controls.DebugKeyPressed("KEY_O") && sel_inst !=-1)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(sel_inst);
			}

			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
		}

		if (controls.DebugKeyPressed("KEY_P"))
		{
			Instance inst;
			float scale = render.GetDevice()->GetHeight() / 1024.0f;
			inst.pos.x = ((Sprite::ed_cam_pos.x + render.GetDevice()->GetWidth()) * 0.5f) / scale;
			inst.pos.y = Sprite::ed_cam_pos.y / scale + 512.0f;
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

	trans.size = sprite_asset->trans.size;

	if (state == Active)
	{
		Sprite::UpdateFrame(&sprite_asset->sprite, &frame_state, dt);
	}

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

		if (array)
		{
			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			for (int value_index = 0; value_index < 4; value_index++)
			{
				int prop = mapping[value_index];

				if (prop == -1)
				{
					continue;
				}

				switch (value_index)
				{
					case 0:
						inst.pos.x = *((float*)objects[i]->GetAddressOfProperty(prop));
						break;
					case 1:
						inst.pos.y = *((float*)objects[i]->GetAddressOfProperty(prop));
						break;
					case 2:
						inst.frame_state.horz_flipped = (*((int*)objects[i]->GetAddressOfProperty(prop)) > 0);
						break;
					case 3:
						inst.visible = *((int*)objects[i]->GetAddressOfProperty(prop));
						break;
				}
			}
		}

		if (inst.visible == 0)
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

		trans.pos = inst.pos;

		trans.BuildMatrices();

		Sprite::Draw(&trans, COLOR_WHITE, &sprite_asset->sprite, &inst.frame_state, true, false);
	}

	trans.pos = pos;

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}

void SpriteInst::AddInstance(float x, float y)
{
	Instance inst;
	inst.pos.x = x;
	inst.pos.y = y;
	inst.frame_state.looped = false;
	instances.push_back(inst);
}

#ifdef EDITOR
bool SpriteInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Instance& inst = instances[i];

		Vector2 pos = (inst.pos + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos;

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

	Gizmo::inst->enabled = ed;

	if (ed)
	{
		SetGizmo();
	}
}

void SpriteInst::SetGizmo()
{
	Gizmo::inst->trans2D = sel_inst != -1 ? &trans : nullptr;

	if (sel_inst != -1)
	{
		trans.pos = instances[sel_inst].pos;
		Gizmo::inst->pos2d = instances[sel_inst].pos;
	}

	Gizmo::inst->enabled = (sel_inst != -1);
}

int SpriteInst::GetInstCount()
{
	return (int)instances.size();
}
#endif