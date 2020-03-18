
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

MeshInstance::MeshInstance()
{

}

bool MeshInstance::Is3DObject()
{
	return true;
}

void MeshInstance::Init()
{
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
				inst.mesh->transform.Move(editor.freecamera.pos + Vector(cosf(editor.freecamera.angles.x), sinf(editor.freecamera.angles.y), sinf(editor.freecamera.angles.x)) * 5.0f);
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
}

bool MeshInstance::CheckSelection(Vector2 ms, Vector start, Vector dir)
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