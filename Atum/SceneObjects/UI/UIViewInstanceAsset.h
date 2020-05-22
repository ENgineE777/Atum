#pragma once
#include "UIWidgetAsset.h"

class UIViewInstanceAsset : public UIWidgetAsset
{
public:
	META_DATA_DECL(UIViewInstanceAsset)

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;

	void Draw(float dt) override;
	void Release() override;

#ifdef EDITOR
	void SaveInstancesRef(JSONWriter& writer) override;
#endif
};

/**
\page scene_assets_ui_view_inst Instance

UI view which is always a root widget

This class ::UIViewInstance is a representation on C++ side.

This class Script::UIView is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
color             | Color of a widget
alpha             | Transparancy of a widget

*/


/**
\ingroup gr_code_scene_objects_ui
*/

/**
\brief Representation of UI view instance

UI view which is always a root widget

This class Script::UIView is a representation in a script.

*/

class UIViewInstance : public UIViewInstanceAsset
{
public:
	META_DATA_DECL(UIViewInstance)

#ifndef DOXYGEN_SKIP

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;

#ifdef EDITOR
	bool IsAsset() override;
	bool UsingOwnCamera() override;
	void AddWidgetToTreeView(EUITreeView* treeview, UIWidgetAsset* widget, void* parent_item);
	void AddChildsToTree(EUITreeView* treeview) override;
#endif

#endif
};