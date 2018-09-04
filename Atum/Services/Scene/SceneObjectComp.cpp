
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