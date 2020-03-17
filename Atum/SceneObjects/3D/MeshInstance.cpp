
#include "MeshInstance.h"

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