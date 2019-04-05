
#include "MetaData.h"
#include "Services/Scene/SceneObject.h"

void SceneObjectWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	nameLabel = new EUILabel(panel, "Not set", 90, 5, 95, 20);
	nameLabel->SetUserData(this);
}

void SceneObjectWidget::SetData(void* set_owner, void* data)
{
	owner = set_owner;
	ref = (SceneObjectRef*)data;

	nameLabel->SetText(ref->object ? ref->object->GetName() : "Not set");
}

void SceneObjectWidget::SetObject(SceneObject* object)
{
	ref->object = object;
	ref->is_asset = object->IsAsset();
	object->GetUIDs(ref->uid, ref->child_uid);

	Object* object_owner = (Object*)owner;

	if (!MetaData::scene->FindByUID(ref->uid, ref->child_uid, ref->is_asset))
	{
		ref->uid = ref->child_uid = 0;
		ref->object = nullptr;
	}

	changed = true;

	nameLabel->SetText(ref->object ? ref->object->GetName() : "Not set");
}