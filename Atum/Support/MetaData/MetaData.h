
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
#include "CallbackWidget.h"
#include "ArrayWidget.h"
#include "SpriteWidget.h"
#endif

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
		Enum,
		Callback,
		Array,
		Sprite
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
#ifdef EDITOR
		std::map<EUICategories*, ProperyWidget*> widgets;
		CallbackWidget::Callback callback;
#endif
		ArrayAdapter*  adapter = nullptr;
	};

	bool inited = false;
	void* owner = nullptr;
	void* root = nullptr;
	std::vector<Property> properties;

	MetaData() = default;
	virtual void Init() = 0;
	void Prepare(void* owner, void* root = nullptr);
	void SetDefValues();
	void Load(JSONReader& reader);
	void Save(JSONWriter& writer);
	void Copy(void* source);
	void BindToScript(class asIScriptEngine* engine, const char* script_class_name);
#ifdef EDITOR
	EUIEditBox* GetFloatEditBox(const char* name);
	void PrepareWidgets(EUICategories* parent);
	void UpdateWidgets();
	bool IsValueWasChanged();
	void HideWidgets();
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

#define BASE_PROP(className, classMember, defValue, strCatName, strPropName, tp, defValueName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = tp;\
	prop.defvalue.defValueName = defValue;\
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
	prop.type = Clor;\
	Color tmp = defValue;\
	memcpy(&prop.defvalue, &tmp, sizeof(float) * 4);\
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
	prop.defvalue.enumIndex = (int)enums.size() - 1;\
	properties.push_back(prop);\
}

#ifdef EDITOR
#define CALLBACK_PROP(className, set_callback, strCatName, strPropName)\
{\
	Property prop;\
	prop.type = Callback;\
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
	prop.type = Array;\
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
	prop.type = Array;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	prop.adapter = new ArrayAdapterImpl<structType>;\
	properties.push_back(prop);\
}


#define SPRITE_PROP(className, classMember, strCatName, strPropName)\
{\
	Property prop;\
	prop.offset = memberOFFSET(className, classMember);\
	prop.type = Sprite;\
	prop.catName = strCatName;\
	prop.propName = strPropName;\
	properties.push_back(prop);\
}