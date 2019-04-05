
#pragma once

#include "Project.h"
#include "Support/MetaData/ProperyWidget.h"

class LayerEntryWidget : public ProperyWidget, public EUIWidget::Listener
{
public:

	Project::Layer* layer = nullptr;

	EUIComboBox* cbox;
	EUIButton* deleteBtn;

	void Init(Project::Layer* layer);
	void SetData(void* owner, void* data) override {};
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
};