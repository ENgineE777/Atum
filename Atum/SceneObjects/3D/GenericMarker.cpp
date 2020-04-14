
#include "GenericMarker.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

META_DATA_DESC(GenericMarker::Instance)
	FLOAT_PROP(GenericMarker::Instance, radius, 1.0f, "Prop", "radius", "radius of a marker")
	COLOR_PROP(GenericMarker::Instance, color, COLOR_WHITE, "Prop", "color")
META_DATA_DESC_END()

CLASSREG(SceneObject, GenericMarker, "GenericMarker")

META_DATA_DESC(GenericMarker)
	BASE_SCENE_OBJ_PROP(GenericMarker)
	STRING_PROP(GenericMarker, scene_group, "", "Prop", "scene_group")
	BOOL_PROP(GenericMarker, full_shade, true, "Prop", "full_shade", "full_shade")
	ARRAY_PROP_INST_CALLGIZMO(GenericMarker, instances, Instance, "Prop", "inst", GenericMarker, sel_inst, SetGizmo)
META_DATA_DESC_END()

bool GenericMarker::Is3DObject()
{
	return true;
}

void GenericMarker::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&GenericMarker::Draw);
}

void GenericMarker::ApplyProperties()
{
	GetScene()->DelFromAllGroups(this);

	if (scene_group.c_str()[0] != 0)
	{
		GetScene()->AddToGroup(this, scene_group.c_str());
	}
}

void GenericMarker::Load(JSONReader& reader)
{
	SceneObject::Load(reader);

	for (auto& inst : instances)
	{
		reader.EnterBlock("inst_trans");

		reader.Read("trans", &inst.transform);

		reader.LeaveBlock();
	}
}

void GenericMarker::Save(JSONWriter& writer)
{
	SceneObject::Save(writer);

	writer.StartArray("inst_trans");

	for (auto& inst : instances)
	{
		writer.StartBlock(nullptr);

		writer.Write("trans", &inst.transform);

		writer.FinishBlock();
	}

	writer.FinishArray();
}

void GenericMarker::MakeMapping(asIScriptObject* object, const char* prefix)
{
	const char* names[] = { "transform", "radius" };
	int types[] = { /*transform*/ asTYPEID_FLOAT, /*radius*/ asTYPEID_FLOAT };

	int count = (sizeof(names) / sizeof(const char*));
	mapping.resize(count);

	for (int value_index = 0; value_index < count; value_index++)
	{
		mapping[value_index] = -1;

		if (prefix[0])
		{
			char str[256];
			StringUtils::Printf(str, 256, "%s_%s", prefix, names[value_index]);

			for (int index = 0; index < (int)object->GetPropertyCount(); index++)
			{
				if (StringUtils::IsEqual(str, object->GetPropertyName(index)))
				{
					if (value_index != 0 && object->GetPropertyTypeId(index) != types[value_index])
					{
						MESSAGE_BOX("Error on an injection into script", StringUtils::PrintTemp("Property %s_%s not set as %s for %s", prefix, names[value_index], types[value_index] == asTYPEID_FLOAT ? "float" : "int", GetName()))
					}
					else
					{
						mapping[value_index] = index;
					}

					break;
				}
			}
		}

		if (mapping[value_index] != -1)
		{
			continue;
		}

		for (int index = 0; index < (int)object->GetPropertyCount(); index++)
		{
			if (StringUtils::IsEqual(names[value_index], object->GetPropertyName(index)))
			{
				if (value_index != 0 && object->GetPropertyTypeId(index) != types[value_index])
				{
					MESSAGE_BOX("Error on an injection into script", StringUtils::PrintTemp("Property %s not set as %s for %s", names[value_index], types[value_index] == asTYPEID_FLOAT ? "float" : "int", GetName()))
				}
				else
				{
					mapping[value_index] = index;
				}

				break;
			}
		}
	}
}

bool GenericMarker::InjectIntoScript(const char* type_name, int type, void* property, const char* prefix)
{
	if (StringUtils::IsEqual(type_name, "array"))
	{
		CScriptArray* array = (CScriptArray*)property;

		if (instances.size() > 0)
		{
			array->Resize((uint32_t)instances.size());

			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			MakeMapping(objects[0], prefix);

			int index = 0;
			for (auto& inst : instances)
			{
				*((Matrix*)objects[index]->GetAddressOfProperty(mapping[0])) = inst.transform;
				*((float*)objects[index]->GetAddressOfProperty(mapping[1])) = inst.radius;

				index++;
			}
		}

		return true;
	}
	else
	if (type & asTYPEID_SCRIPTOBJECT)
	{
		asIScriptObject* object = (asIScriptObject*)property;

		if (instances.size() > 0)
		{
			MakeMapping(object, prefix);

			*((Matrix*)object->GetAddressOfProperty(mapping[0])) = instances[0].transform;
			*((float*)object->GetAddressOfProperty(mapping[1])) = instances[0].radius;
		}

		return true;
	}

	return true;
}

void GenericMarker::Draw(float dt)
{
	if (GetScene()->Playing() || GetState() == SceneObject::State::Invisible)
	{
		return;
	}

#ifdef EDITOR
	if (edited)
	{
		if (core.controls.DebugKeyPressed("KEY_I") && sel_inst != -1)
		{
			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
		}

		bool add_center = core.controls.DebugKeyPressed("KEY_P");
		bool add_copy = core.controls.DebugKeyPressed("KEY_O");

		if (add_center || (add_copy && sel_inst != -1))
		{
			Instance inst;

			if (add_copy)
			{
				inst.color = instances[sel_inst].color;
				inst.transform = instances[sel_inst].transform;
				inst.transform.Pos().x += 1.0f;
			}
			else
			{
				inst.transform.Pos() += editor.freecamera.pos + Vector3(cosf(editor.freecamera.angles.x), sinf(editor.freecamera.angles.y), sinf(editor.freecamera.angles.x)) * 5.0f;
			}

			instances.push_back(inst);

			for (auto comp : components)
			{
				comp->InstAdded(add_copy ? sel_inst : -1);
			}

			sel_inst = (int)instances.size() - 1;

			SetGizmo();
		}
	}
#endif

	for (auto& inst : instances)
	{
		core.render.DebugSphere(inst.transform.Pos(), inst.color, inst.radius, full_shade);
	}
}

#ifdef EDITOR
bool GenericMarker::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	sel_inst = -1;

	for (int i = 0; i < instances.size(); i++)
	{
		auto instance = instances[i];

		if (Math::IntersectSphereRay(instance.transform.Pos(), instance.radius, start, dir))
		{
			sel_inst = i;

			if (IsEditMode())
			{
				SetGizmo();
			}

			return true;
		}
	}

	return false;
}

void GenericMarker::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (!ed)
	{
		sel_inst = -1;
	}

	SetGizmo();
}

void GenericMarker::SetGizmo()
{
	if (sel_inst != -1)
	{
		Gizmo::inst->SetTrans3D(&instances[sel_inst].transform);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}

#endif