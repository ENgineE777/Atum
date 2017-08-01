
#pragma once

#include "Support/Support.h"
#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"
#include "BoolWidget.h"
#include "IntWidget.h"
#include "FloatWidget.h"
#include "StringWidget.h"
#include "FilenameWidget.h"
#include "ColorWidget.h"
#include "EnumWidget.h"

struct MetaDataEnum
{
	int defIndex;
	std::vector<std::string> names;
	std::vector<int> values;
};

class MetaData
{
public:
	enum Type
	{
		Boolean,
		Integer,
		Float,
		String,
		FileName,
		Clor,
		Enum
	};

	union DefValue
	{
		bool  boolean;
		int   integer;
		float flt;
		int   string;
		float color[4];
		int   enumIndex;
	};

	std::vector<MetaDataEnum> enums;
	std::vector<std::string> defStrings;

	struct Property
	{
		int            offset;
		Type           type;
		DefValue       defvalue;
		byte*          value;
		std::string    catName;
		std::string    propName;
		ProperyWidget* widget;
	};

	bool inited;
	bool widgets_inited;
	std::vector<Property> properties;

	MetaData();
	virtual void Init() = 0;
	void Prepare(void* owner);
	void SetDefValuesPrepare();
	void Load(JSONReader* reader);
	void Save(JSONWriter* writer);
	void Copy(void* source);
	void PrepareWidgets(EUICategories* parent);
	bool IsValueWasChanged();
	void HideWidgets();
};

#define META_DATA_DECL(className)\
class MetaData##className : public MetaData\
{\
public:\
	virtual void Init();\
};\
static MetaData##className meta_data;\
virtual MetaData* GetMetaData();

#define META_DATA_DESC(className)\
className##::MetaData##className className##::meta_data;\
MetaData* className##::GetMetaData() { return &className##::meta_data; };\
void className##::MetaData##className##::Init()\
{

#define META_DATA_DESC_END()\
}


#define memberOFFSET(s,m) ((size_t)(&reinterpret_cast<s*>(100000)->m)-100000)

#define BASE_PROP(className, classMember, defValue, strCatName, strPropName, tp, defValueName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = tp;\
	prop.defvalue.##defValueName = defValue;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}

#define BOOL_PROP(className, classMember, defValue, strCatName, strPropName)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Boolean, boolean)

#define INT_PROP(className, classMember, defValue, strCatName, strPropName)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Integer, integer)

#define FLOAT_PROP(className, classMember, defValue, strCatName, strPropName)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Float, flt)

#define BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, tp)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = tp;\
	defStrings.push_back(defValue);\
	prop.defvalue.string = defStrings.size() - 1;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}


#define STRING_PROP(className, classMember, defValue, strCatName, strPropName)\
BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, String)

#define FILENAME_PROP(className, classMember, defValue, strCatName, strPropName)\
BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, FileName)

#define COLOR_PROP(className, classMember, defValue, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Clor;\
	memcpy(&prop.defvalue, &defValue, sizeof(float) * 4);\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}

#define ENUM_PROP(className, classMember, defValue, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.type = Enum;\
	MetaDataEnum enm;\
	enm.defIndex = defValue;

#define ENUM_ELEM(name, value)\
	enm.names.push_back(name);\
	enm.values.push_back(value);

#define ENUM_END\
	enums.push_back(enm);\
	prop.defvalue.enumIndex = enums.size() - 1;\
	properties.push_back(prop);\
}