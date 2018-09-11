
#pragma once

#include "EUI.h"

class SceneObjCmpWidget : public EUIWidget::Listener
{
public:
	EUIPanel* panel;
	EUIButton* edBtn;
	EUIButton* delBtn;
	EUIComboBox* cbox;
	EUIListBox* lbox;
	class SceneObject* obj;

	void Init(EUICategories* parent);
	void Prepare(class SceneObject* obj);
	void Show(bool show);
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
};