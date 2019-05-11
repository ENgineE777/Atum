
#include "PhysBox.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, PhysBox, "PhysBox")

META_DATA_DESC(PhysBox)
BASE_SCENE_OBJ_PROP(PhysBox)
COLOR_PROP(PhysBox, color, COLOR_YELLOW, "Geometry", "color")
FLOAT_PROP(PhysBox, sizeX, 1.0f, "Geometry", "SizeX", "Size along X axis of a box")
FLOAT_PROP(PhysBox, sizeY, 1.0f, "Geometry", "SizeY", "Size along Y axis of a box")
FLOAT_PROP(PhysBox, sizeZ, 1.0f, "Geometry", "SizeZ", "Size along Z axis of a box")
BOOL_PROP(PhysBox, isStatic, false, "Physics", "Is Static", "Set if object should be satic or dynamic")
META_DATA_DESC_END()

Matrix& PhysBox::Trans()
{
	return transform;
}

bool PhysBox::Is3DObject()
{
	return true;
}

void PhysBox::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysBox::Draw);
	owner->AddToGroup(this, "PhysBox");
}

void PhysBox::Draw(float dt)
{
	if (obj)
	{
		obj->GetTransform(transform);
	}

	core.render.DebugBox(transform, color, Vector(sizeX, sizeY, sizeZ));
}

bool PhysBox::Play()
{
	SceneObject::Play();
	obj = PScene()->CreateBox(Vector(sizeX, sizeY, sizeZ), Trans(), Matrix(), isStatic ? PhysObject::Static : PhysObject::Dynamic, 1);

	return true;
}

void PhysBox::Stop()
{
	SceneObject::Stop();
	RELEASE(obj);
}