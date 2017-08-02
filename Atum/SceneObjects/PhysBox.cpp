
#include "PhysBox.h"
#include "Services/Render/Render.h"

CLASSDECLDECL(SceneObject, PhysBox)

META_DATA_DESC(PhysBox)
COLOR_PROP(PhysBox, color, COLOR_YELLOW, "Geometry", "color")
FLOAT_PROP(PhysBox, sizeX, 1.0f, "Geometry", "SizeX")
FLOAT_PROP(PhysBox, sizeY, 1.0f, "Geometry", "SizeY")
FLOAT_PROP(PhysBox, sizeZ, 1.0f, "Geometry", "SizeZ")
BOOL_PROP(PhysBox, isStatic, false, "Physics", "Is Static")
META_DATA_DESC_END()

PhysBox::PhysBox() : SceneObject()
{
}

PhysBox::~PhysBox()
{
}

void PhysBox::Init()
{
	Tasks()->AddTask(100, this, (Object::Delegate)&PhysBox::Draw);
	owner->AddToGroup(this, "PhysBox");
}

void PhysBox::Draw(float dt)
{
	render.DebugBox(transform, color, Vector(sizeX, sizeY, sizeZ));
}