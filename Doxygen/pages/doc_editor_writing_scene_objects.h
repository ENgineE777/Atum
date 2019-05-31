/**

\page doc_editor_writing_scene_objects Writing scene objects in C++

Edtor can be extented by writing your own scene objects. Scene object must be direved from
SceneObject. Assets should be dirived from SceneAsset. Instances of assets should be dirived from
SceneObjectInst.

META_DATA_DECL macros should be used in every definition of a class. 

Macroses META_DATA_DESC and META_DATA_DESC_END should be used to register editable properties.

Example of physical box scene object header:

\code
class PhysBox : public SceneObject
{
public:

	Matrix transform;
	float sizeX;
	float sizeY;
	float sizeZ;
	bool  isStatic;

	Color color;

	META_DATA_DECL(PhysBox) //this macros is needed for implementation of meta data 

	PhysObject* obj = nullptr;

	virtual ~PhysBox() = default;

	Matrix& Trans() override;
	bool Is3DObject() override;
	void Init() override;
	void Draw(float dt);

	bool Play() override;
	void Release() override;
};
\endcode

Example of physical box scene object cpp:

\code

#include "PhysBox.h"
#include "Services/Render/Render.h"

CLASSREG(SceneObject, PhysBox, "PhysBox") //Registration in class factory

META_DATA_DESC(PhysBox) //starting declaration of editable properties
BASE_SCENE_OBJ_PROP(PhysBox)  // declaring base properties like name, scene group and etc
COLOR_PROP(PhysBox, color, COLOR_YELLOW, "Geometry", "color") // declaring color
FLOAT_PROP(PhysBox, sizeX, 1.0f, "Geometry", "SizeX", "Size along X axis of a box") // declaring sizeX
FLOAT_PROP(PhysBox, sizeY, 1.0f, "Geometry", "SizeY", "Size along Y axis of a box") // declaring sizeY
FLOAT_PROP(PhysBox, sizeZ, 1.0f, "Geometry", "SizeZ", "Size along Z axis of a box") // declaring sizeZ
BOOL_PROP(PhysBox, isStatic, false, "Physics", "Is Static", "Set if object should be satic or dynamic") // declaring isStatic
META_DATA_DESC_END() // 

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
	Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysBox::Draw); // registration task int task manger
	GetScene()->AddToGroup(this, "PhysBox");
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

void PhysBox::Release()
{
	RELEASE(obj);
	SceneObject::Release();
}

\endcode
*/


