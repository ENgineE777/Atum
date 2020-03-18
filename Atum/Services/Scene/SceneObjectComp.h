
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>
#include "Services/TaskExecutor/TaskExecutor.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Scene.h"

#ifdef EDITOR
#include "Editor/Gizmo.h"
#endif

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneObjectComp

This class extends functionality of scene objects.

*/

class SceneObjectComp : public Object
{
public:

#ifndef DOXYGEN_SKIP
	SceneObject* object = nullptr;
	const char* class_name = nullptr;
	const char* script_class_name = nullptr;
	bool edited = false;

	virtual void Init();
	virtual void Load(JSONReader& reader);
	virtual void Save(JSONWriter& writer);
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
	virtual void Play();
	virtual void Release();

	virtual void BindClassToScript();
	virtual void InjectIntoScript(asIScriptObject* object, int index, const char* prefix);
#endif

#ifdef EDITOR
	virtual void Copy(SceneObjectComp* src);
	virtual void ShowPropWidgets(EUICategories* objCat);
	virtual void ResizeInst(int count);
	virtual void InstAdded(int create_from);
	virtual void InstDeleted(int index);
	virtual void SetEditMode(bool ed);
	virtual bool IsEditMode();
	virtual void CheckSelection(Vector2 ms, Vector start, Vector dir);
#endif
};

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneObjectInstComp

This is a child of SceneObjectComp and used as components for instances of a asset.

*/

class SceneObjectInstComp : public SceneObjectComp
{
#ifndef DOXYGEN_SKIP
public:
	SceneObjectComp* asset_comp = nullptr;
	void Load(JSONReader& reader) override;
#endif
};

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneAssetComp

This is a childs of SceneObjectComp and used as component for a asset.

*/

class SceneAssetComp : public SceneObjectComp
{
public:
#ifndef DOXYGEN_SKIP
	const char* inst_class_name = nullptr;
#endif
};

CLASSFACTORYDEF(SceneObjectComp)
	struct CheckClass
	{
		virtual bool IsSameClass(Object* ptr) = 0;
	};
	template <typename StructType>
	struct CheckClassImpl : public CheckClass
	{
		bool IsSameClass(Object* ptr) override
		{
#ifdef EDITOR
			return dynamic_cast<StructType*>(ptr);
#else
			return false;
#endif
		}
	};
	virtual void Init() = 0;
	bool IsApplicable(Object* ptr)
	{
		if (inclusive.size())
		{
			for (auto item : inclusive)
			{
				if (item->IsSameClass(ptr))
				{
					return true;
				}
			}
			return false;
		}
		else
		if (exclusive.size())
		{
			for (auto item : exclusive)
			{
				if (item->IsSameClass(ptr))
				{
					return false;
				}
			}
		}

		return true;
	}
	vector<CheckClass*> inclusive;
	vector<CheckClass*> exclusive;
	bool only_for_assets = false;
	bool only_for_instances = false;
CLASSFACTORYDEF_END()

#define COMPINCL(className)\
inclusive.push_back(new CheckClassImpl<className>());

#define COMPEXCL(className)\
exclusive.push_back(new CheckClassImpl<className>());

#define COMP_ASSETS()\
only_for_assets = true;

#define COMP_INSTANCES()\
only_for_instances = true;

#define COMPREG(className, shortName)\
CLASSREGEX(SceneObjectComp, className, className, shortName)\
void Init() override {

#define COMPREG_END(className)\
};\
CLASSREGEX_END(SceneObjectComp, className)