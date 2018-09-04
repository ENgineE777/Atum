
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

class SceneObjectComp : public Object
{
public:
	SceneObject* object = nullptr;
	const char* class_name = nullptr;

	virtual void Init();
	virtual void Load(JSONReader& reader);
	virtual void Save(JSONWriter& writer);
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
};

class SceneObjectInstComp : public SceneObjectComp
{
public:
	SceneObject* asset = nullptr;
};

class SceneAssetComp : public SceneObjectComp
{
public:
#ifdef EDITOR
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
			return dynamic_cast<StructType*>(ptr);
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