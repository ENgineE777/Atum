
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

Matrix* PhysBox::Trans()
{
	return &transform;
}

bool PhysBox::Is3DObject()
{
	return true;
}

void PhysBox::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysBox::Draw);
	GetScene()->AddToGroup(this, "PhysBox");
}

void PhysBox::Draw(float dt)
{
	if (body.body)
	{
		body.body->GetTransform(transform);
	}

	core.render.DebugBox(transform, color, Vector3(sizeX, sizeY, sizeZ));
}

bool PhysBox::Play()
{
	SceneObject::Play();

	body.object = this;
	body.body = PScene()->CreateBox(Vector3(sizeX, sizeY, sizeZ), transform, Matrix(), isStatic ? PhysObject::Static : PhysObject::Dynamic, 1);
	body.body->SetUserData(&body);

	return true;
}

void PhysBox::Release()
{
	RELEASE(body.body);
	SceneObject::Release();
}