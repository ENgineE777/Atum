
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Services/Script/Libs/scriptarray.h"

/**
\page scene_assets_common_ScriptMetaDataAsset Script meta data

This assets holds array of fields which can be used as additional script parameteres of
a scene object. To apply this asset to a scene object firstly Script Meta Data Component should be
added a object. After addinng component needed asset should be set in parameters of component.
After that it will be possible to set up values for each parameter in ScriptMetaDataAsset.

This class ::ScriptMetaDataAsset is a representation on C++ side.

Script property parameters
--------------------------

Name              | Description
------------------| -------------
data_type         | Type of a property
name              | Name of a property

Parameters
----------

Name              | Description
------------------| -------------
properties        | Array of additional script properties

*/


/**
\ingroup gr_code_scene_assets_common
*/

/**
\brief Representation of a script meta data

This class holds fields and they value that can be injected into script for injected script objects.

*/

class ScriptMetaDataAsset : public SceneAsset
{
public:

#ifndef DOXYGEN_SKIP
	META_DATA_DECL(ScriptMetaDataAsset)

	enum Type
	{
		Bool,
		Int,
		Float
	};

	struct MetaType
	{
		META_DATA_DECL(MetaType)

		Type type;
		string name;
	};

	vector<MetaType> properties;

	virtual ~ScriptMetaDataAsset() = default;

	void Init() override;
#endif
};


#define CLASSREG(baseClass, className, shortName)\
CLASSREGEX(baseClass, className, className, shortName)\
CLASSREGEX_END(baseClass, className)
