
#include "SceneAssets/SpriteAsset.h"
#include "SceneObjectComp.h"

void SceneObjectComp::Init()
{
}

void SceneObjectComp::Load(JSONReader& reader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(reader);
}

void SceneObjectComp::Save(JSONWriter& writer)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(writer);
}

void SceneObjectComp::ApplyProperties()
{

}

void SceneObjectComp::InjectIntoScript(const char* type, void* property)
{

}

void SceneObjectComp::Play()
{

}

void SceneObjectComp::Stop()
{

}

void SceneObjectComp::Release()
{
	delete this;
}


#ifdef EDITOR
void SceneObjectComp::Copy(SceneObjectComp* src)
{
	src->GetMetaData()->Copy(src);
	ApplyProperties();
}

void SceneObjectComp::ShowPropWidgets(EUICategories* objCat)
{
	if (objCat)
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->PrepareWidgets(objCat);
	}
	else
	{
		GetMetaData()->HideWidgets();
	}
}

void SceneObjectComp::SetEditMode(bool ed)
{
	edited = ed;
}

bool SceneObjectComp::IsEditMode()
{
	return edited;
}

void SceneObjectComp::CheckSelection(Vector2 ms)
{

}

void SceneObjectComp::ResizeInst(int count)
{

}

void SceneObjectComp::InstAdded()
{

}

void SceneObjectComp::InstDeleted(int index)
{
}
#endif

void SceneObjectInstComp::Load(JSONReader& reader)
{
	SceneObjectComp::Load(reader);

	SceneAsset* asset = ((SceneObjectInst*)object)->asset;

	for (auto comp : asset->components)
	{
		if (StringUtils::IsEqual(((SceneAssetComp*)comp)->inst_class_name, class_name))
		{
			asset_comp = comp;
			break;
		}
	}
}