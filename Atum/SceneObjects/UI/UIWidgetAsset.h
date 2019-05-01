#pragma once
#include "Services/Scene/SceneAsset.h"
#include "SceneObjects/2D/Sprite.h"

/**
\page scene_assets_ui_base Base widget

Physic Box

*/


/**
\ingroup gr_code_scene_assets_ui
*/

class UIWidgetAsset : public SceneAsset
{
protected:

	enum Align
	{
		align_left = 0,
		align_center = 1,
		align_right = 2,
		align_top = 3,
		align_bottom = 4
	};

	enum Size
	{
		fixed = 0,
		fill_parent = 1,
		wrap_context = 2
	};

public:

	Transform2D trans;

	Align horzAlign = align_left;
	Align vertAlign = align_top;
	Size  horzSize = fixed;
	Size  vertSize = fixed;
	float rotate = 0.0f;
	Color color = COLOR_WHITE;

	Vector2 cur_anchor = 0.0f;
	Color   cur_color = COLOR_WHITE;

	bool focused = false;
	bool scaleChilds = false;
	bool clipChilds = false;

	Vector2 left_padding = 0.0f;
	Vector2 right_padding = 0.0f;

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
};

CLASSFACTORYDEF(UIWidgetAsset)
CLASSFACTORYDEF_END()


#define BASE_WIDGET_ASSET_PROP(className)\
STRING_PROP(className, name, "", "Common", "Name")\
BASE_SCENE_OBJ_STATE_PROP(className)

#define BASE_WIDGET_INST_PROP(className)\
BASE_SCENE_OBJ_STATE_PROP(className)


#define BIND_INST_TYPE_TO_SCRIPT(className, classAssetName)\
core.scripts.RegisterObjectType(script_class_name, sizeof(className), "gr_script_scene_objects");\
core.scripts.RegisterObjectMethod(script_class_name, "void SetState(int)", WRAP_MFN(className, SetState));\
core.scripts.RegisterObjectMethod(script_class_name, "int GetState()", WRAP_MFN(className, GetState));\
classAssetName::meta_data.BindToScript(script_class_name);