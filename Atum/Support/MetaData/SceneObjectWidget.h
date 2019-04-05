
#pragma once

#include "ProperyWidget.h"

class SceneObjectWidget : public ProperyWidget
{
public:

	EUILabel* nameLabel;

	struct SceneObjectRef* ref = nullptr;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* owner, void* data) override;
	void SetObject(class SceneObject* object);
};