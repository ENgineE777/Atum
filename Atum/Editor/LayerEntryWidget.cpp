

#include "LayerEntryWidget.h"
#include "Editor.h"
#include "Support\StringUtils.h"

void LayerEntryWidget::Init(Project::Layer* set_layer)
{
	layer = set_layer;

	ProperyWidget::Init(editor.layers_cat, "", layer->name.c_str());

	cbox = new EUIComboBox(panel, 110, 5, 100, 20, 100);
	cbox->SetListener(-1, this, 0);

	cbox->AddItem("Normal");
	cbox->AddItem("Unselectable");
	cbox->AddItem("Invisible");

	cbox->SetCurString(layer->state);

	deleteBtn = new EUIButton(panel, "X", 210, 5, 20, 20);
	deleteBtn->SetListener(-1, this, 0);
}

void LayerEntryWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == deleteBtn)
	{
		panel->Release();

		project.DeleteLayer(layer);

		delete this;
	}
}

void LayerEntryWidget::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	layer->state = (Project::Layer::State)index;
}