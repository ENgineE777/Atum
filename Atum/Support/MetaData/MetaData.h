
#pragma once

#include "Support/Support.h"
#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

#ifdef EDITOR
#include "BoolWidget.h"
#include "IntWidget.h"
#include "FloatWidget.h"
#include "StringWidget.h"
#include "FilenameWidget.h"
#include "ColorWidget.h"
#include "EnumWidget.h"
#include "EnumStringWidget.h"
#include "CallbackWidget.h"
#include "ArrayWidget.h"
#include "SpriteWidget.h"
#include "SceneObjectWidget.h"
#endif

/**
\ingroup gr_code_common_metadata
*/

struct MetaDataEnum
{
	int defIndex;
	std::vector<std::string> names;
	std::vector<int> values;
};

/**
\brief Meta data

This class stores meta data about properties of a class. This data allows to auto bind properties
to UI of the editor and alows searilization/deserialization

*/

class MetaData
{

public:

#ifndef DOXYGEN_SKIP
	enum Type
	{
		Boolean,
		Integer,
		Float,
		String,
		FileName,
		Color,
		Enum,
		EnumString,
		Callback,
		Array,
		Sprite,
		SceneObject
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

	struct ArrayAdapter
	{
		uint8_t* value = nullptr;
		int64_t sel_item_offset = -1;
		int32_t* sel_item = nullptr;
		virtual void Resize(int length) {};
		virtual int GetSize() { return 0; };
		virtual void PushBack() {};
		virtual void Delete(int index) {};
		virtual uint8_t* GetItem(int index) { return nullptr; };
		virtual MetaData* GetMetaData() { return nullptr; };
	};

	template <typename StructType>
	struct ArrayAdapterImpl : public ArrayAdapter
	{
		inline vector<StructType>* Vec()
		{
			return (vector<StructType>*)value;
		};
		void Resize(int length) override
		{
			Vec()->resize(length);
		};
		virtual int GetSize()
		{
			return (int)Vec()->size();
		};
		void PushBack() override
		{
			Vec()->push_back(StructType());
		};
		void Delete(int index) override
		{
			Vec()->erase(Vec()->begin() + index);
		};
		uint8_t* GetItem(int index) override
		{
			return (uint8_t*)&Vec()->at(index);
		};
		MetaData* GetMetaData() override
		{
			return &StructType::meta_data;
		}
	};

	struct Property
	{
		int64_t        offset;
		Type           type;
		DefValue       defvalue;
		uint8_t*       value;
		std::string    catName;
		std::string    propName;
		std::string    brief;
#ifdef EDITOR
		std::map<EUICategories*, ProperyWidget*> widgets;
		CallbackWidget::Callback callback;
		EnumStringWidget::Callback enum_callback;
#endif
		ArrayAdapter*  adapter = nullptr;
	};

	static class Scene* scene;

	bool inited = false;
	void* owner = nullptr;
	void* root = nullptr;
	std::vector<Property> properties;

	MetaData() = default;
	virtual void Init() = 0;

#endif

	/**
	\brief Prepare class to work with object
	*/
	void Prepare(void* owner, void* root = nullptr);

	/**
	\brief Set default value for every property
	*/
	void SetDefValues();

	/**
	\brief Deserialize data from JSON
	*/
	void Load(JSONReader& reader);

	/**
	\brief Post load call. Needed for proper set up of properties which type of SceneObject
	*/
	void PostLoad();

	/**
	\brief Serialize data to JSON
	*/
	void Save(JSONWriter& writer);

	/**
	\brief Copy values of all properties from source object to current active object
	*/
	void Copy(void* source);

	/**
	\brief Bind all properties in script class
	*/
	void BindToScript(const char* script_class_name);

#ifndef DOXYGEN_SKIP
#ifdef EDITOR
	void PrepareWidgets(EUICategories* parent);
	void UpdateWidgets();
	bool IsValueWasChanged();
	void HideWidgets();
#endif
#endif
};

#define META_DATA_DECL(className)\
class MetaDataImpl : public MetaData\
{\
public:\
	virtual void Init();\
};\
static MetaDataImpl meta_data;\
virtual MetaData* GetMetaData();

#define META_DATA_DESC(className)\
className::MetaDataImpl className::meta_data;\
MetaData* className::GetMetaData() { return &className::meta_data; };\
void className::MetaDataImpl::Init()\
{

#define META_DATA_DESC_END()\
}


#define memberOFFSET(s,m) ((size_t)(&reinterpret_cast<s*>(100000)->m)-100000)

#define BASE_PROP(className, classMember, defValue, strCatName, strPropName, tp, defValueName, doxy_brief)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = tp;\
	prop.defvalue.defValueName = defValue;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.brief = doxy_brief;\
	properties.push_back(prop);\
}

#define BOOL_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Boolean, boolean, doxy_brief)

#define INT_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Integer, integer, doxy_brief)

#define FLOAT_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
BASE_PROP(className, classMember, defValue, strCatName, strPropName, Float, flt, doxy_brief)

#define BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, tp)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = tp;\
	defStrings.push_back(defValue);\
	prop.defvalue.string = (int)defStrings.size() - 1;\
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
	prop.type = Type::Color;\
	::Color tmp = ::defValue;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}

#define ENUM_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.type = Type::Enum;\
	prop.brief = doxy_brief;\
	MetaDataEnum enm;\
	enm.defIndex = defValue;

#define ENUM_ELEM(name, value)\
	enm.names.push_back(name);\
	enm.values.push_back(value);

#define ENUM_END\
	enums.push_back(enm);\
	prop.defvalue.enumIndex = (int)enums.size() - 1;\
	properties.push_back(prop);\
}

#define SCENEOBJECT_PROP(className, classMember, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Type::SceneObject;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}

#ifdef EDITOR
#define STRING_ENUM_PROP(className, classMember, set_callback, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Type::EnumString;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.enum_callback = set_callback;\
	properties.push_back(prop);\
}
#endif

#ifdef EDITOR
#define CALLBACK_PROP(className, set_callback, strCatName, strPropName)\
{\
	Property prop;\
	prop.type = Type::Callback;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.callback = set_callback;\
	properties.push_back(prop);\
}
#endif

#define ARRAY_PROP_INST(className, classMember, structType, strCatName, strPropName, selItemClassName, selItemClassMember)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Type::Array;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.adapter = new ArrayAdapterImpl<structType>;\
	prop.adapter->sel_item_offset = memberOFFSET(selItemClassName, selItemClassMember);\
	properties.push_back(prop);\
}

#define ARRAY_PROP(className, classMember, structType, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Type::Array;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.adapter = new ArrayAdapterImpl<structType>;\
	properties.push_back(prop);\
}


#define SPRITE_PROP(className, classMember, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Type::Sprite;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}