
#include "MeshInstance.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

CLASSREG(SceneObject, MeshInstance, "Mesh")

META_DATA_DESC(MeshInstance::Instance)
	COLOR_PROP(MeshInstance::Instance, color, COLOR_WHITE, "Prop", "color")
META_DATA_DESC_END()

META_DATA_DESC(MeshInstance)
	BASE_SCENE_OBJ_PROP(MeshInstance)
	ARRAY_PROP_INST_CALLGIZMO(MeshInstance, instances, Instance, "Prop", "inst", MeshInstance, sel_inst, SetGizmo)
META_DATA_DESC_END()

void MeshInstance::Instance::SetObject(asIScriptObject* set_object, vector<int>* set_mapping)
{
	mapping = set_mapping;
	object = set_object;

	int prop_index = mapping[0][IndexMesh];

	if (prop_index != -1)
	{
		*(asPWORD*)(object->GetAddressOfProperty(prop_index)) = (asPWORD)this;
	}
}

void MeshInstance::Instance::SetColor(Vector3& color)
{
	mesh->color = Color(color.x, color.y, color.z);
}

void MeshInstance::Instance::SetVisible(int set_visible)
{
	if (object)
	{
		int prop = mapping[0][IndexVisible];

		if (prop != -1)
		{
			*((int*)object->GetAddressOfProperty(prop)) = set_visible;
		}

		return;
	}

	mesh->visible = set_visible;
}

bool MeshInstance::Instance::IsVisible()
{
	if (object)
	{
		int prop = mapping[0][IndexVisible];

		if (prop != -1)
		{
			return (*((int*)object->GetAddressOfProperty(prop)) != 0);
		}
	}

	return (mesh->visible != 0);
}

void MeshInstance::Instance::SetTransform(const Matrix& transform)
{
	if (mesh)
	{
		mesh->transform = transform;
	}
}

Matrix MeshInstance::Instance::GetTransform()
{
	if (mesh)
	{
		return mesh->transform;
	}

	return Matrix();
}

Matrix MeshInstance::Instance::GetLocatorTransform(const string& name)
{
	Matrix mat;

	if (mesh)
	{
		mesh->GetLocatorTransform(name.c_str(), mat );
	}

	return mat;
}

MeshInstance::MeshInstance()
{

}

void MeshInstance::BindClassToScript()
{
	const char* brief = "Representation of instance of sprite asset\n"
		"\n"
		"Instance holds array of sprite instances. This array can be injected into a script array.\n"
		"\n"
		"This class ::SpriteInst is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(MeshInstance, brief)
	core.scripts.RegisterObjectMethod(script_class_name, "int AddInstance(float x, float y, bool auto_delete)", WRAP_MFN(MeshInstance, AddInstance), "Add instance. auto_delete conytols if instance should be aito deleted after animation of instance will be finished");
	core.scripts.RegisterObjectMethod(script_class_name, "void RemoveInstance(int index)", WRAP_MFN(MeshInstance, RemoveInstance), "Remove instance");
	core.scripts.RegisterObjectMethod(script_class_name, "void ClearInstances()", WRAP_MFN(MeshInstance, ClearInstances), "Delete all instances");

	brief = "Representation of instance of mesh asset\n"
		"\n"
		"Instance holds array of mesh instances. This array can be binded into a script array.\n"
		"\n"
		"This class ::MeshInstance is a representation on C++ side.\n";

	core.scripts.RegisterObjectType("Mesh", sizeof(MeshInstance::Instance), "gr_script_scene_objects", brief);
	core.scripts.RegisterObjectMethod("Mesh", "void SetTransform(const Matrix&in)", WRAP_MFN(MeshInstance::Instance, SetTransform), "Set transform");
	core.scripts.RegisterObjectMethod("Mesh", "Matrix GetTransform()", WRAP_MFN(MeshInstance::Instance, GetTransform), "Get transform");
	core.scripts.RegisterObjectMethod("Mesh", "Matrix GetLocatorTransform(string&in)", WRAP_MFN(MeshInstance::Instance, GetLocatorTransform), "Get locator transform");
	core.scripts.RegisterObjectMethod("Mesh", "void SetColor(Vector3&in)", WRAP_MFN(MeshInstance::Instance, SetColor), "Set color");
}

bool MeshInstance::InjectIntoScript(const char* type_name, int type, void* property, const char* prefix)
{
	if (StringUtils::IsEqual(type_name, "array"))
	{
		array = (CScriptArray*)property;

		scr_prefix = prefix;

		if (instances.size() > 0)
		{
			array->Resize((uint32_t)instances.size());

			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			MakeMapping(objects[0], prefix);

			int index = 0;
			for (auto& inst : instances)
			{
				inst.SetObject(objects[index], &mapping);

				for (auto& comp : components)
				{
					comp->InjectIntoScript(objects[index], index, prefix);
				}

				index++;
			}
		}
		else
		if (array->GetSize())
		{
			OnResize(0, array->GetSize());
		}

		array->listiner = this;
		array->ownerName = GetName();

		return true;
	}

	return SceneObject::InjectIntoScript(type_name, type, property, prefix);
}

void MeshInstance::MakeMapping(asIScriptObject* object, const char* prefix)
{
	const char* names[] = { "mesh", "visible" };
	int types[] = { /*mesh*/ asTYPEID_FLOAT, /*visible*/ asTYPEID_INT32 };

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
					if (value_index != Instance::MapIndices::IndexMesh && object->GetPropertyTypeId(index) != types[value_index])
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
				if (value_index != Instance::MapIndices::IndexMesh && object->GetPropertyTypeId(index) != types[value_index])
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

	mapped = true;
}

bool MeshInstance::Is3DObject()
{
	return true;
}

void MeshInstance::Init()
{
	script_callbacks.push_back(ScriptCallback(GetScene(), "OnContactStart", "void", "%i%s%i"));
	script_callbacks.push_back(ScriptCallback(GetScene(), "OnContactEnd", "void", "%i%s%i"));

	Tasks(false)->AddTask(100, this, (Object::Delegate)&MeshInstance::Draw);
}

void MeshInstance::ApplyProperties()
{
	for (int index = 0; index < instances.size(); index++)
	{
		auto& inst = instances[index];

		if (inst.mesh == nullptr)
		{
			inst.mesh = core.meshes.LoadMesh(Asset()->mesh_name.c_str(), RenderTasks(false));

			if (sel_inst == index)
			{
				SetGizmo();
			}
		}

		if (inst.mesh != nullptr)
		{
			inst.mesh->color = inst.color;
		}
	}
}

void MeshInstance::Load(JSONReader& reader)
{
	SceneObjectInst::Load(reader);

	for (auto& inst : instances)
	{
		reader.EnterBlock("inst_trans");

		inst.mesh = core.meshes.LoadMesh(Asset()->mesh_name.c_str(), RenderTasks(false));
		reader.Read("trans", &inst.mesh->transform);

		reader.LeaveBlock();
	}
}

void MeshInstance::Save(JSONWriter& writer)
{
	SceneObjectInst::Save(writer);

	writer.StartArray("inst_trans");

	for (auto& inst : instances)
	{
		writer.StartBlock(nullptr);

		writer.Write("trans", &inst.mesh->transform);

		writer.FinishBlock();
	}

	writer.FinishArray();
}

void MeshInstance::Draw(float dt)
{
#ifdef EDITOR
	if (edited)
	{
		if (core.controls.DebugKeyPressed("KEY_I") && sel_inst != -1)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(sel_inst);
			}

			RELEASE(instances[sel_inst].mesh)
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
				inst = instances[sel_inst];
			}

			inst.mesh = core.meshes.LoadMesh(Asset()->mesh_name.c_str(), RenderTasks(false));

			if (add_copy)
			{
				inst.mesh->color = instances[sel_inst].color;
				inst.mesh->transform = instances[sel_inst].mesh->transform;
				inst.mesh->transform.Pos().x += 1.0f;
			}
			else
			{
				inst.mesh->transform.Pos() += editor.freecamera.pos + Vector3(cosf(editor.freecamera.angles.x), sinf(editor.freecamera.angles.y), sinf(editor.freecamera.angles.x)) * 5.0f;
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

	if (GetScene()->Playing() && mapped)
	{
		for (auto& inst : instances)
		{
			inst.mesh->visible = inst.IsVisible();
		}
	}
}

void MeshInstance::SyncArray()
{
	asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

	if (!mapped)
	{
		MakeMapping(objects[0], scr_prefix.c_str());
	}

	for (int index = 0; index < instances.size(); index++)
	{
		instances[index].SetObject(objects[index], &mapping);

		for (auto& comp : components)
		{
			comp->InjectIntoScript(objects[index], index, scr_prefix.c_str());
		}
	}
}

void MeshInstance::OnResize(int at, int delta)
{
	instances.insert(instances.begin() + at, delta, Instance());

	for (int i = at; i < at + (int)delta; i++)
	{
		instances[i].mesh = core.meshes.LoadMesh(Asset()->mesh_name.c_str(), RenderTasks(false));
	}

	SyncArray();
}

void MeshInstance::OnRemove(int start, asUINT count)
{
	for (int i = start; i < start + (int)count; i++)
	{
		RELEASE(instances[i].mesh)
	}

	instances.erase(instances.begin() + start, instances.begin() + start + count);

	SyncArray();
}

int MeshInstance::AddInstance()
{
	instances.emplace_back();
	Instance& inst = instances.back();

	int index = (int)instances.size() - 1;
	if (array)
	{
		array->Resize((uint32_t)instances.size());

		SyncArray();
	}

	return index;
}

void MeshInstance::RemoveInstance(int index)
{
	instances.erase(instances.begin() + index);

	if (array)
	{
		array->RemoveAt(index);
	}
}

void MeshInstance::ClearInstances()
{
	instances.clear();

	if (array)
	{
		array->RemoveRange(0, array->GetSize());
	}
}

void MeshInstance::Release()
{
	if (array)
	{
		array->listiner = nullptr;
	}

	for (auto& inst : instances)
	{
		RELEASE(inst.mesh)
	}

	SceneObjectInst::Release();
}

#ifdef EDITOR
bool MeshInstance::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	sel_inst = -1;

	for (int i = 0; i < instances.size(); i++)
	{
		auto instance = instances[i];

		if (Math::IntersectBBoxRay(instance.mesh->transform.Pos() + instance.mesh->GetBBMin(), instance.mesh->transform.Pos() + instance.mesh->GetBBMax(), start, dir))
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

void MeshInstance::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (!ed)
	{
		sel_inst = -1;
	}

	SetGizmo();
}

void MeshInstance::SetGizmo()
{
	if (sel_inst != -1 && instances[sel_inst].mesh)
	{
		Gizmo::inst->SetTrans3D(&instances[sel_inst].mesh->transform);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}
#endif