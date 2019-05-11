#pragma once
#include "Services/Scene/SceneAsset.h"
#include "SceneObjects/2D/Sprite.h"

/**
\ingroup gr_code_scene_assets_ui
*/

/**
\brief Base class of UI widget

Base class of UI widget from which derived all UI widgets

*/

class UIWidgetAsset : public SceneAsset
{
public:

	enum Align
	{
		align_left = 0 /*!< Aligment to the left */,
		align_center = 1 /*!< Aligment at the center */,
		align_right = 2 /*!< Aligment to the right */,
		align_top = 3 /*!< Aligment to the top */,
		align_bottom = 4 /*!< Aligment to the bottom */
	};

	enum Size
	{
		fixed = 0 /*!< Size set up directly via properties of a widget */,
		fill_parent = 1 /*!< Size equals to size of a parent */,
		wrap_context = 2 /*!< Size depends from a widget context */
	};

	/**
	\brief Transformation of a widget
	*/
	Transform2D trans;

	/**
	\brief Horizontal aligment of a widget
	*/
	Align horzAlign = align_left;

	/**
	\brief Vertical aligment of a widget
	*/
	Align vertAlign = align_top;

	/**
	\brief Type of width of a widget
	*/
	Size  horzSize = fixed;

	/**
	\brief Type of height of a widget
	*/
	Size  vertSize = fixed;

	/**
	\brief Rotation a widget
	*/
	float rotate = 0.0f;

	/**
	\brief Color a widget
	*/
	Color color = COLOR_WHITE;

	/**
	\brief Left-Top padding of a widget
	*/
	Vector2 left_padding = 0.0f;

	/**
	\brief Right-Bottom padding of a widget
	*/
	Vector2 right_padding = 0.0f;

	/**
	\brief Should be childs sacled in case size of a widget was changed
	*/
	bool scaleChilds = false;

	/**
	\brief Should be childs clipped by size of a widget
	*/
	bool clipChilds = false;

#ifndef DOXYGEN_SKIP

	Vector2 cur_anchor = 0.0f;
	Color   cur_color = COLOR_WHITE;

	bool focused = false;

	UIWidgetAsset* source = nullptr;
	bool           source_is_asset = false;
	UIWidgetAsset* parent = nullptr;
	std::vector<UIWidgetAsset*> childs;

	virtual ~UIWidgetAsset();

	virtual void Init();
	
	void Release() override;

	void ApplyProperties() override;
	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

	virtual void CalcState();
	virtual void Draw(float dt);
	void AddChild(UIWidgetAsset* node, int index = -1);
	void DeleteChild(UIWidgetAsset* node);
	void DeleteChilds();
	virtual void SetSource(UIWidgetAsset* source, bool remove_from_prev);

	uint32_t GetParentUID() override;
	SceneObject* GetChild(uint32_t uid) override;
#ifdef EDITOR
	bool IsAsset() override;
	void SaveAssetData(JSONWriter& writer) override;
	void SetOwner(Scene* owner) override;
	bool UsingCamera2DPos() override;
	void SetEditMode(bool ed) override;
	virtual void StoreProperties();
	virtual void RestoreProperties();
#endif

#endif
};

CLASSFACTORYDEF(UIWidgetAsset)
CLASSFACTORYDEF_END()


#define BASE_WIDGET_ASSET_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")\
BASE_SCENE_OBJ_STATE_PROP(className)

#define BASE_WIDGET_INST_PROP(className)\
BASE_SCENE_OBJ_STATE_PROP(className)


#define BIND_INST_TYPE_TO_SCRIPT(className, classAssetName, brief)\
core.scripts.RegisterObjectType(script_class_name, sizeof(className), "gr_script_scene_objects", brief);\
core.scripts.RegisterObjectMethod(script_class_name, "void SetState(int)", WRAP_MFN(className, SetState), "Setting state of a widget");\
core.scripts.RegisterObjectMethod(script_class_name, "int GetState()", WRAP_MFN(className, GetState), "Getting state of a widget");\
classAssetName::meta_data.BindToScript(script_class_name);